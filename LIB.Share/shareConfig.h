#pragma once

#include <utilsBase.h>

#include <boost/property_tree/ptree.hpp>

namespace share_config
{

struct tDevice
{
	std::string Type;
	utils::tVersion Version;

	tDevice() = default;
	explicit tDevice(const boost::property_tree::ptree& pTree);
	tDevice(const std::string& type, const utils::tVersion& version);
};

struct tPlatform
{
	std::string ID;

	tPlatform() = default;
	explicit tPlatform(const boost::property_tree::ptree& pTree);
};

struct tUpdateServer
{
	std::string Host;
	std::string Target;
	std::string TargetList;

	tUpdateServer() = default;
	explicit tUpdateServer(const boost::property_tree::ptree& pTree);
};

struct tOutFile
{
	std::string Path;
	std::string Prefix;
	uint8_t QtyMax = 0;

	tOutFile() = default;
	tOutFile(const std::string& baseName, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tOutFileCap : public tOutFile
{
	std::uint32_t Capacity = 0;

	tOutFileCap() = default;
	tOutFileCap(const std::string& baseName, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tPipe
{
	std::string Path;

	tPipe() = default;
	tPipe(const std::string& pipeName, const std::string& platformID, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tSerialPort
{
	std::string ID;
	std::uint32_t BR = 0;

	tSerialPort() = default;
	tSerialPort(const std::string& baseName, const std::string& platformID, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tSPIPort
{
	std::string ID;
	std::uint8_t Mode = 0;
	std::uint8_t Bits = 0;
	std::uint32_t Frequency_hz = 0;
	std::uint16_t Delay_us = 0;

	tSPIPort() = default;
	tSPIPort(const std::string& portID, const std::string& baseName, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tGPIOPort
{
	std::string ID;

	tGPIOPort() = default;
	tGPIOPort(const std::string& baseName, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

}