#include <devConfig.h>

#include <utilsWeb.h>

#include <sstream>
#include <stdexcept>

tUpdateList GetUpdateList(const std::string& host, const std::string& target)
{
	std::optional<std::string> DataRcvd = utils::web::HttpsReqSyncString(host, target);
	if (!DataRcvd.has_value())
		throw std::runtime_error("the update-server is not available");

	tUpdateList UpdateList;

	enum class tLinePart : int
	{
		Head,
		RefValue,
		SHA256,
		Unknown,
	};
	tLinePart LinePart;
	std::stringstream SStr(DataRcvd.value());
	tUpdateItem UpdateItem;
	while (!SStr.eof())
	{
		std::string StrTemp;
		SStr >> StrTemp;

		if (StrTemp.find("FW") != std::string::npos)
			LinePart = tLinePart::Head;
		
		switch (LinePart)
		{
		case tLinePart::Head:
		{
			UpdateItem = {};
			UpdateItem.Head = StrTemp;
			LinePart = tLinePart::RefValue;
			break;
		}
		case tLinePart::RefValue:
		{
			UpdateItem.RefValue = StrTemp;
			LinePart = tLinePart::SHA256;
			break;
		}
		case tLinePart::SHA256:
		{
			UpdateItem.SHA256 = StrTemp;
			LinePart = tLinePart::Unknown;
			break;
		}
		case tLinePart::Unknown:
		default:
			continue; // just ignore this data
		}

		if (UpdateItem.SHA256.empty())
			continue;

		UpdateList.push_back(UpdateItem);
		UpdateItem = {};
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

