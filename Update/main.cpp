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

tUpdateList GetUpdateList(const std::string& host, const std::string& target);
utils::tVectorUInt8 GetUpdate(const std::string& host, const std::string& target);

std::string AddPath(const std::string& path, const std::string& part)
{
	return utils::linux::CorrPath(path + "/" + part);
}

std::string GetUpdateFile(const dev::tDataSetConfig& dsConfig)
{
	std::string DeviceType = dsConfig.GetDevice().Type;

	std::cout << "Device: " << DeviceType << " " << dsConfig.GetDevice().Version.ToString() << '\n';

	dev::config::tUpdateServer UpdateServer = dsConfig.GetUpdateServer();

	tUpdateList UpdateList = GetUpdateList(UpdateServer.Host, UpdateServer.Target + UpdateServer.TargetList);

	for (auto& i : UpdateList)
		std::cout << "List: " << i.first << " " << i.second << '\n';

	auto UpdateItem = std::find_if(UpdateList.begin(), UpdateList.end(), [&](const tUpdatePair& item) { return item.first.find(DeviceType) != std::string::npos; });
	if (UpdateItem == UpdateList.end())
		return {};

	std::cout << "Update: " << UpdateItem->first << " " << UpdateItem->second << '\n';

	size_t VersionPos = UpdateItem->first.find_last_of('_') + 1;
	utils::tVersion Version(UpdateItem->first.substr(VersionPos));
	std::cout << "Version: " << Version.ToString() << " - ";
	if (!(Version > dsConfig.GetDevice().Version))
	{
		std::cout << "rejected\n";
		return {};
	}
	std::cout << "accepted\n";

	utils::tVectorUInt8 UpdateData = GetUpdate(UpdateServer.Host, UpdateServer.Target + UpdateItem->second);
	if (UpdateData.empty())
		return {};

	std::cout << "Update data size: " << std::to_string(UpdateData.size()) << '\n';

	std::string Path = dsConfig.GetUpdatePath();

	if (std::filesystem::exists(Path))
		std::filesystem::remove_all(Path);

	std::filesystem::create_directory(Path);

	Path = AddPath(Path, UpdateItem->first);

	std::fstream File(Path, std::ios::binary | std::ios::out);
	if (!File.good())
		return {};

	File.write((char*)UpdateData.data(), UpdateData.size());
	File.close();

	std::cout << "Update data saved: " << Path << '\n';

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
		{
			std::cout << "There is no acute update on the server." << UpdateFileZip << '\n';
			return static_cast<int>(utils::tExitCode::EX_OK);
		}

		utils::arch_7z2201_simple::Decompress(UpdateFileZip);

		std::filesystem::remove(UpdateFileZip);

		std::string CmdCDPathMX = "cd " + DsConfig.GetUpdatePath() + "/mx; ";
		utils::linux::CmdLine(CmdCDPathMX + "chmod 544 setup.sh");
		utils::linux::CmdLine(CmdCDPathMX + "./setup.sh");
	}
	catch (std::exception& e)
	{
		std::cerr << "[ERR] " << e.what() << "\n";

		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}

