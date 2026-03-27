#pragma once
#include <devConfig.h>

#include "devDataSetConfig.h"

#include <utilsPacketNMEAType.h>

#include <ctime>
#include <mutex>
#include <optional>
#include <string>

namespace dev
{

struct tDataSetGNSS
{
	std::string ReceiverModel;

	struct tDateTime
	{
		std::time_t Seconds = 0;
		std::uint32_t Milliseconds = 0;

		bool operator == (const tDateTime&) const = default;
		bool operator != (const tDateTime&) const = default;
	};

	std::optional<tDateTime> DateTime;
	bool Valid = false;
	std::optional<double> Latitude;
	std::optional<double> Longitude;
	std::optional<double> Altitude;
	std::optional<double> Speed;
	std::optional<double> Course;
	std::string Mode;

	struct tSatellite
	{
		std::uint8_t ID;			// Satellite ID (GPS: 1-32, SBAS 33-64 (33=PRN120), GLONASS: 65-96) 
		std::uint8_t Elevation;		// Degree (Maximum 90)
		std::uint16_t Azimuth;		// Degree (Range 0 to 359)
		std::uint8_t SNR;			// SNR (C/No) 00-99 dB-Hz, null when not tracking
	};
	std::vector<tSatellite> Satellites;

	tDataSetGNSS() = default;

	bool operator == (const tDataSetGNSS&) const = default;
	bool operator != (const tDataSetGNSS&) const = default;

	std::string ToString() const;
	std::string ToJSON() const;

	void ToFile(const share::config::tOutFile& configOut) const;

private:
	std::string ToString(tDateTime time) const;
	std::string ToString(std::time_t time) const;
	//std::string ToStringOutdated(tDateTime time) const;
	//std::string ToStringOutdated(std::time_t time) const;
};

}
