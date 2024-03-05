#include "main_server.h"
#include <shareLANTag.h>
#include <utilsLinux.h>

tVectorUInt8 tLANTagServer::OnReceived(const tVectorUInt8& data)
{
	return HandlePacket(data);
}

tVectorUInt8 tLANTagServer::HandlePacket(const tVectorUInt8& cmd)
{
	if (cmd.size() > 20) // [#] max. size of an incoming cmd
		return {};

	auto ToVector = [](const std::string& ans)->tVectorUInt8
	{
		tVectorUInt8 Data(ans.begin(), ans.end());
		Data.push_back(0);
		return Data;
	};

	const std::string CmdStr(cmd.begin(), cmd.end());
	if (CmdStr == "get_tag")
	{
		std::shared_ptr<dev::tDataSetConfig> DataSetConfig = m_DataSetConfig.lock();
		if (!DataSetConfig)
			return {};
		share::tLANTag Tag(DataSetConfig->GetPlatform().ID, utils::linux::CmdLine("hostname"));
		return ToVector(Tag.ToJSON());
	}

	return {}; // Send nothing in response to unknown request.
}

void tLANTagServer::OnSent(boost::shared_ptr<tVectorUInt8> packet, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	//std::cout << "HandleSend: " << packet->size() << " cerr: " << error << " --- bytes: " << bytes_transferred << '\n';
}