#include <devConfig.h>

#include <utilsWeb.h>

#include <sstream>
#include <stdexcept>

tUpdateList GetUpdateList(const std::string& host, const std::string& target)
{
	std::optional<std::string> DataRcvd = utils::web::HttpsReqSyncString(host, target);
	if (!DataRcvd.has_value())
		throw std::runtime_error("the update-server is not available");

	using tUpdatePair = std::pair<std::string, std::string>;
	std::deque<tUpdatePair> UpdateList;

	std::stringstream SStr(DataRcvd.value());
	std::string StrHead;
	std::string StrValue;
	while (!SStr.eof())
	{
		std::string StrTemp;
		SStr >> StrTemp;

		if (StrTemp.find("FW") != std::string::npos)
		{
			StrHead = StrTemp;
			StrValue.clear();
		}
		else if (!StrHead.empty() && !StrTemp.empty())
		{
			StrValue = StrTemp;
		}

		if (StrValue.empty())
			continue;

		UpdateList.emplace_back(StrHead, StrValue);

		StrHead.clear();
		StrValue.clear();
	}

	return UpdateList;
}

utils::tVectorUInt8 GetUpdate(const std::string& host, const std::string& target)
{
	std::optional<utils::tVectorUInt8> DataRcvd = utils::web::HttpsReqSyncVector(host, target);
	if (!DataRcvd.has_value())
		throw std::runtime_error("the update-server is not available");

	return DataRcvd.value();
}

