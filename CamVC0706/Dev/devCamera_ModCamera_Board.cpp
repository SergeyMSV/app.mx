#include "devCamera.h"
#include "devDataSetConfig.h"

namespace dev
{

tCam::tModCam::tBoardCtrl::tBoardCtrl(tModCam* obj, boost::asio::io_context& io)
	:m_pObj(obj), tSerialPort(io, g_Settings.GetSerialPortCtrl().ID, g_Settings.GetSerialPortCtrl().BR)
{

}

tCam::tModCam::tBoardCtrl::~tBoardCtrl()
{

}

void tCam::tModCam::tBoardCtrl::OnReceived(std::vector<std::uint8_t>& data)
{
	m_pObj->OnReceivedCtrl(data);
}

tCam::tModCam::tBoardData::tBoardData(tModCam* obj, boost::asio::io_context& io)
	:m_pObj(obj), tSerialPort(io, g_Settings.GetSerialPortData().ID, g_Settings.GetSerialPortData().BR)
{

}

tCam::tModCam::tBoardData::~tBoardData()
{

}

void tCam::tModCam::tBoardData::OnReceived(std::vector<std::uint8_t>& data)
{
	m_pObj->OnReceivedCtrl(data);
}

}
