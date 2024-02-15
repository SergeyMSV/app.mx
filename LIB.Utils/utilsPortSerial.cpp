#include "utilsPortSerial.h"

#include <chrono>
#include <thread>

namespace utils
{
	namespace port_serial
	{

tPortOneWireSync::tPort::tPort(boost::asio::io_context& io, const std::string& id, std::uint32_t baudRate, tCharSize charSize, tStopBits stopBits, tParity parity, tFlowControl flowControl)
	: tPortSerialAsync(io, id, baudRate, charSize, stopBits, parity, flowControl)
{
}

bool tPortOneWireSync::tPort::IsReceived(std::size_t qty) const
{
	std::lock_guard<std::mutex> Lock(m_Mtx);
	return m_DataRcv.size() == qty;
}

std::vector<std::uint8_t> tPortOneWireSync::tPort::GetReceived()
{
	std::lock_guard<std::mutex> Lock(m_Mtx);
	std::vector<std::uint8_t> Data = std::move(m_DataRcv);
	return Data;
}

void tPortOneWireSync::tPort::ResetReceived()
{
	std::lock_guard<std::mutex> Lock(m_Mtx);
	m_DataRcv.clear();
}

void tPortOneWireSync::tPort::OnReceived(std::vector<std::uint8_t>& data)
{
	std::lock_guard<std::mutex> Lock(m_Mtx);
	m_DataRcv.insert(m_DataRcv.end(), data.begin(), data.end());
}

tPortOneWireSync::tGuardBR::tGuardBR(tPort* port, std::uint32_t baudRate) :m_pPort(port)
{
	if (!m_pPort)
		return;
	m_BR = m_pPort->GetBaudRate();
	m_pPort->SetBaudRate(baudRate);
}
tPortOneWireSync::tGuardBR::~tGuardBR()
{
	if (!m_pPort)
		return;
	m_pPort->SetBaudRate(m_BR);
}

tPortOneWireSync::tPortOneWireSync(boost::asio::io_context& io, const std::string& id, tSpeed speed)
	: m_Port(io, id, GetBaudRate(speed), tCharSize::character_size(8), tStopBits::one, tParity::none, tFlowControl::none)
{}

tPortOneWireSync::tStatus tPortOneWireSync::Reset()
{
	m_Port.ResetReceived();
	tGuardBR BR(&m_Port, 9600);

	m_Port.Send({ 0x0F }); // Pull DQ line low, then up. UART transmits LSB first.

	constexpr std::size_t RspSize = 2;
	std::vector<std::uint8_t> Data = GetReceived(RspSize);
	if (Data.size() != RspSize)
		return tStatus::NoRsp;
	if (Data[0] != 0x0F)
		return tStatus::PortDamaged;
	if (Data[1] == 0x00)
		return tStatus::ShortCircuit;
	if (Data[1] == 0xF0)
		return tStatus::Presence;
	return tStatus::Success;
}

std::vector<std::uint8_t> tPortOneWireSync::Transaction(std::vector<std::uint8_t> tx, std::size_t rxSize)
{
	if (tx.empty())
		return {};
	tx.resize(tx.size() + rxSize, 0xFF);
	std::vector<std::uint8_t> Rx = OW_Transaction(tx);
	if (Rx.size() != tx.size())
		return {};
	Rx.erase(Rx.begin(), Rx.begin() + 1);
	return Rx;
}

void tPortOneWireSync::SendBit(bool tx)
{
	const std::vector<std::uint8_t> DataSend(1, tx ? 0xFF : 0x00);
	m_Port.Send(DataSend);
	GetReceived(DataSend.size());
}

std::vector<bool> tPortOneWireSync::ReceiveBits(std::size_t rxSize)
{
	std::vector<std::uint8_t> DataSend(rxSize, 0xFF);
	m_Port.Send(DataSend);
	std::vector<std::uint8_t> Rx = GetReceived(DataSend.size());
	std::vector<bool> Res;
	for (std::uint32_t i = 0; i < Rx.size(); ++i)
	{
		Res.push_back(BitToByte(Rx[i]));
	}
	return Res;
}

std::vector<std::uint8_t> tPortOneWireSync::OW_Transaction(const std::vector<std::uint8_t>& tx)
{
	const std::vector<std::uint8_t> DataSend = ByteToBit(tx);
	m_Port.Send(DataSend);
	return BitToByte(GetReceived(DataSend.size()));
}

std::vector<std::uint8_t> tPortOneWireSync::GetReceived(std::size_t rxSize)
{
	for (int i = 0; i < 1000; ++i) // [#] 100 ms is MAX period of time for a transaction but in fact more than the above mentioned period of time.
	{
		if (m_Port.IsReceived(rxSize))
			break;
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
	return m_Port.GetReceived();
}

std::vector<std::uint8_t> tPortOneWireSync::ByteToBit(const std::vector<std::uint8_t>& data) const
{
	std::vector<std::uint8_t> DataBits;
	for (auto& k : data)
	{
		for (int i = 0; i < 8; ++i)
		{
			DataBits.push_back((k & (1 << i)) ? 0xFF : 0x00);
		}
	}
	return DataBits;
}

std::vector<std::uint8_t> tPortOneWireSync::BitToByte(const std::vector<std::uint8_t>& data) const
{
	std::vector<std::uint8_t> DataBytes;
	std::uint8_t Byte = 0;
	for (int i = 0; i < data.size(); ++i)
	{
		int bitIdx = i % 8;
		Byte |= BitToByte(data[i]) << bitIdx;

		if (bitIdx == 7)
		{
			DataBytes.push_back(Byte);
			Byte = 0;
		}
	}
	return DataBytes;
}

std::uint32_t tPortOneWireSync::GetBaudRate(tSpeed val)
{
	switch (val)
	{
	case tSpeed::Slow: return 57600;
	case tSpeed::Norm: return 115200;
	case tSpeed::Fast: return 230400;
	}
	return 57600;
}

	}
}
