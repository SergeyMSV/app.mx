#pragma once
#include <devConfig.h>

#include <utilsBase.h>

#include <fstream>
#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h> 
#else // _WIN32
#include <sys/stat.h> // mkfifo
#include <fcntl.h> // open
#include <unistd.h> // write, close, unlink
#endif // _WIN32

namespace share
{

template <class T>
class tPipeO
{
	std::string m_Name;
	std::thread m_Thread;
	std::function<T()> m_Callback;
	bool m_Active;

public:
	tPipeO() = delete;
	tPipeO(const std::string& name, std::function<T()> callback)
		:m_Name(name + "_o"), m_Callback(callback), m_Active(true)
	{
#ifndef _WIN32
		unlink(m_Name.c_str()); // delete a name and possibly the file it refers
		int Ret = mkfifo(m_Name.c_str(), 0666); // create the named pipe (fifo) with permission
		if (Ret < 0)
			THROW_RUNTIME_ERROR(std::string("Error when creating FIFO. ") + std::string(strerror(errno)) + ".");
#endif // _WIN32

		m_Thread = std::thread([&]()
			{
				while (m_Active)
				{
					Write();
				}
			});
	}
	tPipeO(const tPipeO&) = delete;
	tPipeO(tPipeO&&) = delete;
	~tPipeO()
	{
		m_Active = false;
		// Blocking operation (ConnectNamedPipe, "open" for fifo) must be stopped
		std::ifstream ClosePipe(m_Name);
		ClosePipe.close();
		m_Thread.join();
#ifndef _WIN32
		unlink(m_Name.c_str());
#endif // _WIN32
	}
	tPipeO operator=(const tPipeO&) = delete;
	tPipeO operator=(tPipeO&&) = delete;

private:
	void Write()
	{
#ifdef _WIN32
		constexpr DWORD PipeSize = 8192;
		HANDLE PipeHnd = CreateNamedPipeA(m_Name.c_str(), PIPE_ACCESS_OUTBOUND, PIPE_TYPE_MESSAGE, PIPE_UNLIMITED_INSTANCES, PipeSize, PipeSize, NMPWAIT_USE_DEFAULT_WAIT, NULL);
		if (PipeHnd == INVALID_HANDLE_VALUE)
			THROW_RUNTIME_ERROR("Pipe hasn't been opened: " + m_Name);

		if (ConnectNamedPipe(PipeHnd, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED))
		{
			T Data = m_Callback();
			DWORD cbWritten = 0;
			WriteFile(
				PipeHnd,                          // handle to pipe 
				Data.data(),                      // buffer to write from 
				static_cast<DWORD>(Data.size()),  // number of bytes to write 
				&cbWritten,                       // number of bytes written 
				NULL);                            // not overlapped I/O 
		}
		CloseHandle(PipeHnd);
#else // _WIN32
		int PipeHnd = open(m_Name.c_str(), O_WRONLY);
		std::string Data = m_Callback();
		write(PipeHnd, Data.data(), Data.size());
		close(PipeHnd);
		// If there is no pause here the pipe throws exception "Broken pipe" and sends wrong data.
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif // _WIN32
	}
};

template <class T>
class tPipeI
{
	std::string m_Name;
	std::thread m_Thread;
	std::function<void(const T&)> m_Callback;
	bool m_Active;

public:
	tPipeI() = delete;
	tPipeI(const std::string& name, std::function<void(const T&)> callback)
		:m_Name(name + "_i"), m_Callback(callback), m_Active(true)
	{
#ifndef _WIN32
		unlink(m_Name.c_str()); // delete a name and possibly the file it refers
		int Ret = mkfifo(m_Name.c_str(), 0666); // create the named pipe (fifo) with permission
		if (Ret < 0)
			THROW_RUNTIME_ERROR(std::string("Error when creating FIFO. ") + std::string(strerror(errno)) + ".");
#endif // _WIN32

		m_Thread = std::thread([&]()
			{
				while (m_Active)
				{
					Read();
				}
			});
	}
	tPipeI(const tPipeI&) = delete;
	tPipeI(tPipeI&&) = delete;
	~tPipeI()
	{
		m_Active = false;
		// Blocking operation (ConnectNamedPipe, "open" for fifo) must be stopped
		std::ofstream ClosePipe(m_Name);
		ClosePipe.close();
		m_Thread.join();
#ifndef _WIN32
		unlink(m_Name.c_str());
#endif // _WIN32
	}
	tPipeI operator=(const tPipeI&) = delete;
	tPipeI operator=(tPipeI&&) = delete;

private:
	void Read()
	{
		constexpr std::size_t ReadBufferSize = 1024; // [#] It must be of the same size or more than the largest incoming message otherwise ReadFile returns 0.
#ifdef _WIN32
		constexpr DWORD PipeSize = 8192;
		HANDLE PipeHnd = CreateNamedPipeA(m_Name.c_str(), PIPE_ACCESS_INBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES, PipeSize, PipeSize, NMPWAIT_USE_DEFAULT_WAIT, NULL);
		if (PipeHnd == INVALID_HANDLE_VALUE)
			THROW_RUNTIME_ERROR("Pipe hasn't been opened: " + m_Name);

		if (ConnectNamedPipe(PipeHnd, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED))
		{
			T Data(ReadBufferSize, 0);
			DWORD cbRead = 0;
			BOOL Res = ReadFile(
				PipeHnd,                          // handle to pipe 
				Data.data(),                      // buffer to write from 
				static_cast<DWORD>(Data.size()),  // number of bytes to write 
				&cbRead,                          // number of bytes written 
				NULL);                            // not overlapped I/O 

			if (Res && cbRead > 0)
			{
				Data.resize(cbRead);
				m_Callback(Data);
			}
		}
		CloseHandle(PipeHnd);
#else // _WIN32
		int PipeHnd = open(m_Name.c_str(), O_RDONLY);
		T Data(ReadBufferSize, 0);
		std::size_t cbRead = read(PipeHnd, Data.data(), Data.size());
		close(PipeHnd);
		if (cbRead > 0)
		{
			Data.resize(cbRead);
			m_Callback(Data);
		}
#endif // _WIN32
	}
};

}