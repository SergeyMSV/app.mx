#include "devGNSS.h"
#include "devDataSetConfig.h"

namespace dev
{

tGNSS::tModGnssReceiver::tBoard::tBoard(tModGnssReceiver* obj, boost::asio::io_context& io)
	:m_pObj(obj), tSerialPort(io, g_Settings.GetSerialPortGNSS().ID, g_Settings.GetSerialPortGNSS().BR)
{

}

tGNSS::tModGnssReceiver::tBoard::~tBoard()
{

}

void tGNSS::tModGnssReceiver::tBoard::OnReceived(utils::tVectorUInt8& data)
{
	m_pObj->OnReceived(data);
}

}
