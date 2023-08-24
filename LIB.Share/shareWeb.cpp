#include "shareConfig.h"
#include "shareWeb.h"

#include <utilsLinux.h>
#include <utilsWeb.h>

#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>

#include <7z2201/utilsArch_7z2201_simple.h>
#include <openssl/sha.h>

struct tUpdateItem
{
	std::string Head;
	std::string RefValue;
	std::string SHA256;
};
using tUpdateList = std::deque<tUpdateItem>;

tUpdateList GetUpdateList(const std::string& host, const std::string& target)
{
	std::optional<std::string> DataRcvd = utils::web::HttpsReqSyncString(host, target);
	if (!DataRcvd.has_value())
		THROW_RUNTIME_ERROR("the update-server is not available");

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
		THROW_RUNTIME_ERROR("the update-server is not available");

	return DataRcvd.value();
}

std::string GetUpdateFile(const share_config::tUpdateServer& server, const share_config::tDevice& device, const std::string& path)
{
	tUpdateList UpdateList = GetUpdateList(server.Host, server.Target + server.TargetList);

	auto UpdateItem = std::find_if(UpdateList.begin(), UpdateList.end(), [&](const tUpdateItem& item) { return item.Head.find(device.Type) != std::string::npos; });
	if (UpdateItem == UpdateList.end())
		return {};

	std::cerr << "Update: " << UpdateItem->Head;

	size_t VersionPos = UpdateItem->Head.find_last_of('_') + 1;
	utils::tVersion Version(UpdateItem->Head.substr(VersionPos));
	if (!(Version > device.Version))
	{
		std::cerr << "; version " << Version.ToString() << " rejected.\n";
		return {};
	}

	utils::tVectorUInt8 UpdateData = GetUpdate(server.Host, server.Target + UpdateItem->RefValue);
	if (UpdateData.empty())
		return {};

	std::vector<uint8_t> Sha256Out(SHA256_DIGEST_LENGTH);
	SHA256((uint8_t*)UpdateData.data(), UpdateData.size(), Sha256Out.data());

	std::stringstream SStrSHA256;
	std::for_each(Sha256Out.cbegin(), Sha256Out.cend(), [&SStrSHA256](uint8_t data) { SStrSHA256 << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(data); });
	if (SStrSHA256.str() != UpdateItem->SHA256)
	{
		std::cerr << "; SHA256 rejected. Calculated SHA256 doesn't match the valid one.\n";
		std::cerr << "Valid: " << UpdateItem->SHA256 << '\n';
		std::cerr << "Calc:  " << SStrSHA256.str() << '\n';
		return {};
	}

	std::cerr << '\n';

	std::filesystem::path Path = std::filesystem::weakly_canonical(path);

	if (std::filesystem::exists(Path))
		std::filesystem::remove_all(Path);

	std::filesystem::create_directory(Path);

	Path /= UpdateItem->Head;

	std::fstream File(Path, std::ios::binary | std::ios::out);
	if (!File.good())
		return {};

	File.write((char*)UpdateData.data(), UpdateData.size());
	File.close();

	return Path.string();
}
