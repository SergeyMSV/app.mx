using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json.Nodes;

namespace utils.twr
{
    public abstract class UDPClient : IDisposable // [TBD] -> UDPClient - just for DALLAS because it requests data and doesn't receive everything
    {
        public event EventHandler<ConnectedEventArgs>? Connected;

        UdpClient? m_UDPClient;
        IPEndPoint? m_EnpointRemote;
        protected string m_TWREndpoint = "";
        protected UInt32 m_TWREndpointUARTBaudrate = 0;

        public bool IsOpen { get; private set; } = false;
        public bool LogEnabled { get; set; } = false;

        CancellationTokenSource? m_cts;
        Task? m_receiveLoopTask;
        TaskCompletionSource<bool>? m_CloseTcs;
        readonly SemaphoreSlim m_closeLock = new(1, 1);
        volatile bool m_disposed;

        public UDPClient(ushort udpPortLocal, IPEndPoint udpEndpointRemote, bool logEnabled, string uartID, UInt32 uartBR)
        {
            m_EnpointRemote = udpEndpointRemote;
            LogEnabled = logEnabled;
            m_TWREndpoint = uartID;
            m_TWREndpointUARTBaudrate = uartBR;

            try
            {
                m_UDPClient = new UdpClient(udpPortLocal)
                {
                    EnableBroadcast = false,
                    DontFragment = true,
                };
            }
            catch (Exception ex)
            {
                LogWriteError(ex.Message);
                return;
            }

            m_cts = new();
            m_receiveLoopTask = ReceiveLoopAsync(m_cts.Token);
        }

        public void Dispose()
        {
            if (m_disposed)
                return;
            m_disposed = true;
            m_cts?.Cancel();
            try
            {
                m_receiveLoopTask?.Wait(1000);
            }
            catch { }
            m_UDPClient?.Close();
        }

        public async Task<bool> Open()
        {
            return await SendInternal(Cmds.MakeGetVersion());
        }

        async Task ReceiveLoopAsync(CancellationToken ct)
        {
            if (m_UDPClient == null)
                return;
            while (!ct.IsCancellationRequested)
            {
                try
                {
                    UdpReceiveResult Res = await m_UDPClient.ReceiveAsync(ct);
                    if (!IsExpectedSender(Res.RemoteEndPoint))
                    {
                        LogWriteTrace("Ignored packet from " + Res.RemoteEndPoint);
                        continue;
                    }
                    string ResStr = Encoding.UTF8.GetString(Res.Buffer);
                    await PacketDecoder(Res.RemoteEndPoint, ResStr);
                }
                catch (OperationCanceledException) when (ct.IsCancellationRequested)
                {
                    break;
                }
                catch (ObjectDisposedException)
                {
                    break;
                }
                catch (Exception ex)
                {
                    LogWriteError(ex.Message);
                    try
                    {
                        await Task.Delay(100, ct);
                    }
                    catch (OperationCanceledException)
                    {
                        break;
                    }
                }
            }
        }

        bool IsExpectedSender(IPEndPoint ep)
        {
            if (m_EnpointRemote == null)
                return true;
            return ep.Port == m_EnpointRemote.Port && ep.Address.Equals(m_EnpointRemote.Address);
        }

        public async Task<bool> Close(int timeoutMs = 1000)
        {
            await m_closeLock.WaitAsync();
            try
            {
                var Tcs = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
                m_CloseTcs = Tcs;
                if (!await SendInternal(Cmds.MakeClose(m_TWREndpoint)))
                    return false;
                Task Done = await Task.WhenAny(Tcs.Task, Task.Delay(timeoutMs));
                return Done == Tcs.Task && Tcs.Task.Result;
            }
            finally
            {
                m_CloseTcs = null;
                m_closeLock.Release();
            }
        }

        protected abstract Task ReceivedCmdOpen();
        protected abstract Task ReceivedCmdClose();
        protected abstract void ReceivedCmd(IPEndPoint ep, JsonNode rspJson);

        async Task PacketDecoder(IPEndPoint ep, string rsp)
        {
            try
            {
                LogWriteTrace("Received from " + ep.ToString() + "\n" + rsp);

                JsonNode? Node = JsonNode.Parse(rsp);
                if (Node == null)
                    return;
                string Cmd = Node["cmd"]?.ToString() ?? "unknown";
                string Response = Node["rsp"]?.ToString() ?? "";

                switch (Cmd)
                {
                    case "version":
                        {
                            string Ver = Node["version"]?.ToString() ?? "unknown";
                            Connected?.Invoke(this, new(ep, Ver));
                            await SendInternal(twr.Cmds.MakeOpen(m_TWREndpoint, m_TWREndpointUARTBaudrate));
                            break;
                        }
                    case "open":
                        {
                            IsOpen = Response == "ok";
                            if (!IsOpen)
                                break;
                            await ReceivedCmdOpen();
                            break;
                        }
                    case "close": // It means that the existed connection already closed.
                        {
                            IsOpen = false;
                            m_CloseTcs?.TrySetResult(Response == "ok");
                            await ReceivedCmdClose();
                            break;
                        }
                    default:
                        {
                            ReceivedCmd(ep, Node);
                            break;
                        }
                }
            }
            catch (Exception ex)
            {
                LogWriteError(ex.Message);
            }
        }

        async Task<bool> SendInternal(string msg)
        {
            if (m_UDPClient == null)
                return false;
            try
            {
                Byte[] Req = Encoding.UTF8.GetBytes(msg);
                await m_UDPClient.SendAsync(Req, Req.Length, m_EnpointRemote);
                LogWriteTrace("Sent to " + (m_EnpointRemote?.ToString() ?? "unknown endpoint") + " " + msg);
                return true;
            }
            catch (Exception ex)
            {
                LogWriteError(ex.Message);
                return false;
            }
        }

        public async Task<bool> Send(string msg)
        {
            if (!IsOpen)
                return false;
            return await SendInternal(msg);
        }

        protected void LogWriteError(string message)
        {
            Log.WriteError(message);
        }

        protected void LogWriteTrace(string message)
        {
            if (LogEnabled)
                Log.WriteTrace(message);
        }
    }
}
