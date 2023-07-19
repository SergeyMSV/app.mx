#include <devConfig.h>

#include <utilsBase.h>
#include <utilsLinux.h>
#include <utilsPath.h>

#include <devDataSetConfig.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <7z2201/utilsArch_7z2201_simple.h>
#include <openssl/sha.h>

tUpdateList GetUpdateList(const std::string& host, const std::string& target);
utils::tVectorUInt8 GetUpdate(const std::string& host, const std::string& target);

std::string AddPath(const std::string& path, const std::string& part)
{
	return utils::linux::CorrPath(path + "/" + part);
}

std::string GetUpdateFile(const dev::tDataSetConfig& dsConfig)
{
	std::string DeviceType = dsConfig.GetDevice().Type;

	dev::config::tUpdateServer UpdateServer = dsConfig.GetUpdateServer();

	tUpdateList UpdateList = GetUpdateList(UpdateServer.Host, UpdateServer.Target + UpdateServer.TargetList);

	auto UpdateItem = std::find_if(UpdateList.begin(), UpdateList.end(), [&](const tUpdateItem& item) { return item.Head.find(DeviceType) != std::string::npos; });
	if (UpdateItem == UpdateList.end())
		return {};

	std::cerr << "Update: " << UpdateItem->Head;

	size_t VersionPos = UpdateItem->Head.find_last_of('_') + 1;
	utils::tVersion Version(UpdateItem->Head.substr(VersionPos));
	if (!(Version > dsConfig.GetDevice().Version))
	{
		std::cerr << "; version " << Version.ToString() << " rejected.\n";
		return {};
	}

	utils::tVectorUInt8 UpdateData = GetUpdate(UpdateServer.Host, UpdateServer.Target + UpdateItem->RefValue);
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

	std::string Path = dsConfig.GetUpdatePath();

	if (std::filesystem::exists(Path))
		std::filesystem::remove_all(Path);

	std::filesystem::create_directory(Path);

	Path = AddPath(Path, UpdateItem->Head);

	std::fstream File(Path, std::ios::binary | std::ios::out);
	if (!File.good())
		return {};

	File.write((char*)UpdateData.data(), UpdateData.size());
	File.close();

	return Path;
}

int main(int argc, char* argv[])
{
	try
	{
		const std::string AppName = utils::GetAppNameMain(argv[0]);

		std::string PathFileConfig = utils::linux::GetPathConfigExc(AppName);
		std::string PathFileDevice = utils::linux::GetPathConfigExc("mxdevice");

		dev::tDataSetConfig DsConfig(PathFileConfig, PathFileDevice);
		
		std::string UpdateFileZip = GetUpdateFile(DsConfig);
		if (UpdateFileZip.empty())
			return static_cast<int>(utils::tExitCode::EX_OK);

		utils::arch_7z2201_simple::Decompress(UpdateFileZip);

		std::filesystem::remove(UpdateFileZip);

		std::string CmdCDPathMX = "cd " + DsConfig.GetUpdatePath() + "/mx; ";
		utils::linux::CmdLine(CmdCDPathMX + "chmod 544 setup.sh");
		utils::linux::CmdLine(CmdCDPathMX + "./setup.sh");
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}

