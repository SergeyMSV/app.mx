#include "sharePortGPIO.h"

#ifdef MXTWR_CLIENT

namespace share_port
{

tGPIO::tGPIO(boost::asio::io_context& ioc, TWR::tEndpoint ep)
	:tTWRClient(ioc), m_Endpoint(ep)
{
}

tGPIO::~tGPIO()
{
}

bool tGPIO::GetState()
{
	return m_State;
}

void tGPIO::SetState(bool state)
{
	TWR::tChipControl ChipControl;
	ChipControl.Field.Reset = state ? 1 : 0;
	Transaction_SPI_SetChipControl(m_Endpoint, ChipControl);
}

}
#else // MXTWR_CLIENT

#ifdef MXTWR_SERVER

#include <fstream>

namespace share_port
{

tGPIO::tGPIO(const std::string& id)
	:m_ID(id)
{
#if !defined(_WIN32)
	std::fstream File(m_ID, std::ios::in); // check if the file exists
	if (!File.good())
	{
		m_ErrMsg = "can't open file for reading";
		return;
	}
	File.close();
#endif // _WIN32
}

tGPIO::~tGPIO()
{

}

bool tGPIO::GetState()
{
	return m_State;
}

void tGPIO::SetState(bool state)
{
	if (!IsReady())
		return;
	std::fstream File(m_ID, std::ios::out);
	if (!File.good())
	{
		m_ErrMsg = "can't open file for writing";
		return;
	}
	m_State = state;
	File.put(state ? '1' : '0');
	File.close();
}

}
#endif // MXTWR_SERVER
#endif // MXTWR_CLIENT
