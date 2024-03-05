#include "main_server.h"
#include <shareLANTag.h>
#include <utilsLinux.h>

void tLANTagServer::OnReceived(const share_network_udp::tEndpoint& endpoint, const tVectorUInt8& data)
{
	if (data.size() > 20) // [#] max. size of an incoming data
		return;

	auto ToVector = [](const std::string& ans)->tVectorUInt8
	{
		tVectorUInt8 Data(ans.begin(), ans.end());
		Data.push_back(0);
		return Data;
	};

	const std::string CmdStr(data.begin(), data.end());
	if (CmdStr == "get_tag")
	{
		std::shared_ptr<dev::tDataSetConfig> DataSetConfig = m_DataSetConfig.lock();
		if (!DataSetConfig)
			return;
		share::tLANTag Tag(DataSetConfig->GetPlatform().ID, utils::linux::CmdLine("hostname"));
		Send(endpoint, ToVector(Tag.ToJSON()));
	}
	// Send nothing in response to unknown request.
}

void tLANTagServer::OnSent(boost::shared_ptr<tVectorUInt8> packet, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	//std::cout << "HandleSend: " << packet->size() << " cerr: " << error << " --- bytes: " << bytes_transferred << '\n';
}