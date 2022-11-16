#include "devCamera.h"
#include "devDataSetConfig.h"

namespace dev
{

tCamera::tModCamera::tBoardCtrl::tBoardCtrl(tModCamera* obj, boost::asio::io_context& io)
	:m_pObj(obj), tSerialPort(io, g_Settings.GetSerialPortCtrl().ID, g_Settings.GetSerialPortCtrl().BR)
{

}

tCamera::tModCamera::tBoardCtrl::~tBoardCtrl()
{

}

void tCamera::tModCamera::tBoardCtrl::OnReceived(utils::tVectorUInt8& data)
{
	m_pObj->OnReceivedCtrl(data);
}

tCamera::tModCamera::tBoardData::tBoardData(tModCamera* obj, boost::asio::io_context& io)
	:m_pObj(obj), tSerialPort(io, g_Settings.GetSerialPortData().ID, g_Settings.GetSerialPortData().BR)
{

}

tCamera::tModCamera::tBoardData::~tBoardData()
{

}

void tCamera::tModCamera::tBoardData::OnReceived(utils::tVectorUInt8& data)
{
	m_pObj->OnReceivedCtrl(data);
}

}
