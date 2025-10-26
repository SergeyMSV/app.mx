#pragma once

#include <utilsVersion.h>

#include <boost/property_tree/ptree.hpp>

namespace share
{
namespace config
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
	std::uint8_t QtyMax = 0;

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
	tPipe(const std::string& pipeName, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

namespace port
{

struct tUART_Config // Universal Asynchronous Receiver/Transmitter
{
	std::string ID;
	std::uint32_t BR = 0;

	tUART_Config() = default;
	tUART_Config(const std::string& baseName, const std::string& platformID, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

using tSerial_Config = tUART_Config;

struct tSPI_Config
{
	std::string ID;
	std::uint8_t Mode = 0;
	std::uint8_t Bits = 0;
	std::uint32_t Frequency_hz = 0;
	std::uint16_t Delay_us = 0;

	tSPI_Config() = default;
	tSPI_Config(const std::string& portID, const std::string& baseName, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tGPIO_Config
{
	std::string ID;

	tGPIO_Config() = default;
	tGPIO_Config(const std::string& baseName, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tIP_Config
{
	std::uint16_t Value = 0;

	tIP_Config() = default;
	explicit tIP_Config(std::uint16_t value) :Value(value) {}
	tIP_Config(const std::string& baseName, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

using tUDP_Config = tIP_Config;
using tTCP_Config = tIP_Config;

}
}
}
