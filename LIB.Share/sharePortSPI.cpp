#include "sharePortSPI.h"

#ifdef MXTWR_CLIENT

namespace share_port
{

tSPI::tSPI(boost::asio::io_context& ioc, TWR::tEndpoint ep)
	:tTWRClient(ioc), m_Endpoint(ep)
{
}

tSPI::~tSPI()
{
}

std::uint8_t tSPI::GetMode()
{
	return m_Mode;
}

bool tSPI::SetMode(std::uint8_t val)
{
	m_Mode = val;
	return true;
}

std::uint8_t tSPI::GetBits()
{
	return m_Bits;
}

bool tSPI::SetBits(std::uint8_t val)
{
	m_Bits = val;
	return true;
}

std::uint32_t tSPI::GetSpeed()
{
	return m_Speed;
}

bool tSPI::SetSpeed(std::uint32_t val)
{
	m_Speed = val;
	return true;
}

std::vector<std::uint8_t> tSPI::Transaction(const std::vector<std::uint8_t>& tx)
{
	return Transaction_SPI_Request(m_Endpoint, tx);
}

}
#else // MXTWR_CLIENT

#if !defined(_WIN32)

#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

namespace share_port
{

tSPI::tSPI(const std::string& id, std::uint8_t mode, std::uint8_t bits, std::uint32_t speed_hz, std::uint16_t delay_us)
	:m_Delay(delay_us)
{
	//[TBD] switch on the device

	m_FileSPI = open(id.c_str(), O_RDWR);
	if (m_FileSPI < 0)
	{
		m_ErrMsg = "can't open device";
		return;
	}
	if (!SetMode(mode))
		return;
	if (!SetBits(bits))
		return;
	SetSpeed(speed_hz);
}

tSPI::~tSPI()
{
	//[TBD] switch off the device
}

std::uint8_t tSPI::GetMode()
{
	std::uint8_t Mode = 0;
	int ret = ioctl(m_FileSPI, SPI_IOC_RD_MODE, &Mode);
	if (ret != -1)
		return Mode;
	m_ErrMsg = "can't get spi mode";
	return UINT8_MAX;
}

bool tSPI::SetMode(std::uint8_t val)
{
	m_Mode = val;
	int ret = ioctl(m_FileSPI, SPI_IOC_WR_MODE, &m_Mode);
	if (ret != -1)
		return true;
	m_ErrMsg = "can't set spi mode";
	return false;
}

std::uint8_t tSPI::GetBits()
{
	std::uint8_t Bits = 0;
	int ret = ioctl(m_FileSPI, SPI_IOC_RD_BITS_PER_WORD, &Bits);
	if (ret != -1)
		return Bits;
	m_ErrMsg = "can't get bits per word";
	return UINT8_MAX;
}

bool tSPI::SetBits(std::uint8_t val)
{
	m_Bits = val;
	int ret = ioctl(m_FileSPI, SPI_IOC_WR_BITS_PER_WORD, &m_Bits);
	if (ret != -1)
		return true;
	m_ErrMsg = "can't set bits per word";
	return false;
}

std::uint32_t tSPI::GetSpeed()
{
	std::uint32_t Speed = 0;
	int ret = ioctl(m_FileSPI, SPI_IOC_RD_MAX_SPEED_HZ, &Speed);
	if (ret != -1)
		return Speed;
	m_ErrMsg = "can't get max speed hz";
	return UINT32_MAX;
}

bool tSPI::SetSpeed(std::uint32_t val)
{
	m_Speed = val;
	int ret = ioctl(m_FileSPI, SPI_IOC_WR_MAX_SPEED_HZ, &m_Speed);
	if (ret != -1)
		return true;
	m_ErrMsg = "can't set max speed hz";
	return false;
}

std::vector<std::uint8_t> tSPI::Transaction(const std::vector<std::uint8_t>& tx)
{
	std::vector<std::uint8_t> rx;
	rx.resize(tx.size());

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx.data(),
		.rx_buf = (unsigned long)rx.data(),
		.len = tx.size(),
		.speed_hz = m_Speed,
		.delay_usecs = m_Delay,
		.bits_per_word = m_Bits,
	};

	int ret = ioctl(m_FileSPI, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		m_ErrMsg = "can't send spi message";

	return rx;
}

}
#else // _WIN32

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>

#ifdef MXTWR_SERVER
namespace share_port
{

tSPI::tSPI(const std::string& id, std::uint8_t mode, std::uint8_t bits, std::uint32_t speed_hz, std::uint16_t delay_us)
	:m_Mode(mode), m_Bits(bits), m_Speed(speed_hz), m_Delay(delay_us)
{
	//m_ErrMsg = "Test Error Message";
}

tSPI::~tSPI()
{

}

std::uint8_t tSPI::GetMode()
{
	return m_Mode;
}

bool tSPI::SetMode(std::uint8_t val)
{
	m_Mode = val;
	return true;
}

std::uint8_t tSPI::GetBits()
{
	return m_Bits;
}

bool tSPI::SetBits(std::uint8_t val)
{
	m_Bits = val;
	return true;
}

std::uint32_t tSPI::GetSpeed()
{
	return m_Speed;
}

bool tSPI::SetSpeed(std::uint32_t val)
{
	m_Speed = val;
	return true;
}

std::vector<std::uint8_t> tSPI::Transaction(const std::vector<std::uint8_t>& tx)
{
	std::cout << "SPI-> ";
	for (auto i : tx)
		std::cout << std::right << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(i) << ' ';
	std::cout << '\n';

	std::vector<std::uint8_t> rx = tx;
	std::reverse(rx.begin(), rx.end());
	return rx;
}

}
#endif // MXTWR_SERVER
#endif // _WIN32
#endif // MXTWR_CLIENT
