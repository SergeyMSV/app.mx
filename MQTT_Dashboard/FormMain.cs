using MQTTnet;
using System.Text;

namespace MQTT_Dashboard
{
    public partial class FormMain : Form
    {
        IMqttClient? m_mqttClient;
        bool m_closingAfterDisconnect;

        public FormMain()
        {
            InitializeComponent();
            topicTextBox.Text = "748C93B777A5BBAB0E6-101/#";
        }

        private Task OnMessageReceived(MqttApplicationMessageReceivedEventArgs e)
        {
            var payload = Encoding.UTF8.GetString(e.ApplicationMessage.Payload);
            LogMessage($"{e.ApplicationMessage.Topic}: {payload}");
            return Task.CompletedTask;
        }

        private void LogMessage(string message)
        {
            if (IsDisposed || !logTextBox.IsHandleCreated)
                return;

            if (logTextBox.InvokeRequired)
                logTextBox.BeginInvoke(() => logTextBox.AppendText(message + Environment.NewLine));
            else
                logTextBox.AppendText(message + Environment.NewLine);
        }

        private async void buttonConnect_Click(object sender, EventArgs e) => await ConnectAsync();
        private async void buttonSubscribe_Click(object sender, EventArgs e) => await SubscribeAsync(topicTextBox.Text);
        private async void buttonDisconnect_Click(object sender, EventArgs e) => await DisconnectAsync();

        private async Task ConnectAsync()
        {
            if (m_mqttClient?.IsConnected == true)
            {
                LogMessage("Already connected.");
                return;
            }

            await DisposeClientAsync();

            m_mqttClient = new MqttClientFactory().CreateMqttClient();
            m_mqttClient.ApplicationMessageReceivedAsync += OnMessageReceived;

            var options = new MqttClientOptionsBuilder()
                .WithTcpServer("test.mosquitto.org", 1883)
                //.WithClientId($"MqttClient-lalala")
                .WithClientId($"MqttClient-{Guid.NewGuid():N}")
                .Build();

            try
            {
                await m_mqttClient.ConnectAsync(options);
                LogMessage("Connected to MQTT broker.");
            }
            catch (Exception ex)
            {
                LogMessage($"Error connecting: {ex}");
                await DisposeClientAsync();
            }
        }

        private async Task SubscribeAsync(string topic)
        {
            if (m_mqttClient?.IsConnected != true)
            {
                LogMessage("Client is not connected.");
                return;
            }

            if (string.IsNullOrWhiteSpace(topic))
            {
                LogMessage("Cannot subscribe to an empty topic.");
                return;
            }

            try
            {
                var filter = new MqttTopicFilterBuilder().WithTopic(topic).Build();
                await m_mqttClient.SubscribeAsync(filter);
                LogMessage($"Subscribed to topic: {topic}");
            }
            catch (Exception ex)
            {
                LogMessage($"Error subscribing: {ex}");
            }
        }

        private async Task DisconnectAsync()
        {
            if (m_mqttClient == null)
                return;

            try
            {
                if (m_mqttClient.IsConnected)
                {
                    await m_mqttClient.DisconnectAsync();
                    LogMessage("Disconnected from MQTT broker.");
                }
            }
            catch (Exception ex)
            {
                LogMessage($"Error disconnecting: {ex}");
            }
            finally
            {
                await DisposeClientAsync();
            }
        }

        private Task DisposeClientAsync()
        {
            if (m_mqttClient != null)
            {
                m_mqttClient.ApplicationMessageReceivedAsync -= OnMessageReceived;
                m_mqttClient.Dispose();
                m_mqttClient = null;
            }
            return Task.CompletedTask;
        }

        protected override async void OnFormClosing(FormClosingEventArgs e)
        {
            base.OnFormClosing(e);

            if (m_closingAfterDisconnect || m_mqttClient == null)
                return;

            e.Cancel = true;
            m_closingAfterDisconnect = true;
            await DisconnectAsync();
            Close();
        }
    }
}