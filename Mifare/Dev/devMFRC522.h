#pragma once

#include <devConfig.h>
#include <devDataSet.h>

#include <Arduino.h>
// These header are to be here in order to avoid troubles with defined 'F' in Arduino.h
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>

#include <shareConfig.h>
#include <sharePortGPIO.h>
#include <sharePortSPI.h>

#ifdef MXTWR_CLIENT
#define _WIN32_WINNT 0x0601
#include <boost/asio.hpp>
#endif // MXTWR_CLIENT

#include <mutex>

namespace dev
{

using tMFRC522_RxGain = MFRC522Constants::PCD_RxGain;

class tMFRC522
{
	class tPortSPI : public MFRC522DriverSPI
	{
#ifdef MXTWR_CLIENT
		boost::asio::io_context m_ioc;
#endif // MXTWR_CLIENT
		share_port::tSPI m_SPI;

	public:
		tPortSPI() = delete;
		tPortSPI(const std::string& id, std::uint8_t mode, std::uint8_t bits, std::uint32_t speed_hz, std::uint16_t delay_us)
#ifdef MXTWR_CLIENT
			:m_SPI(m_ioc, share_port::TWR::tEndpoint::SPI0_CS0) // [#]
#else // MXTWR_CLIENT
			:m_SPI(id, mode, bits, speed_hz, delay_us)
#endif // MXTWR_CLIENT
		{}
		~tPortSPI() {}

		std::vector<std::uint8_t> Transaction(const std::vector<std::uint8_t>& tx) override
		{
			return m_SPI.Transaction(tx);
		}
	};

	tPortSPI m_PortSPI;
	MFRC522 m_MFRC522;
	std::recursive_mutex m_MFRC522_mtx;

public:
	tMFRC522() = delete;
	explicit tMFRC522(const share_config::tSPIPort& confSPI);
	tMFRC522(const tMFRC522&) = delete;
	tMFRC522(tMFRC522&&) = delete;
	~tMFRC522();
	
	std::uint8_t GetAntennaGain(); // returns in dB
	void SetAntennaGain(tMFRC522_RxGain value);

	bool IsNewCardPresent();

	std::vector<std::uint8_t> ReadCardID();

	card_mfr::tCardType GetCardType() const;
	std::vector<std::uint8_t> GetCardID() const;

	card_mfr::tSector GetCardClassicSector(int sectorIdx, card_mfr::tKeyID keyID, card_mfr::tKey key);
	card_mfr::tCardUL GetCardUltralight();

	bool WriteCard(const card_mfr::tCardUL& card); // [TBD] that shall be a template for different types of cards

	void HaltCard();

private:
	//bool Write(std::uint8_t blockAddr, std::vector<std::uint8_t> data); // MIFARE Classic : The block(0 - 0xff) number. MIFARE Ultralight : The page(2 - 15) to write to.
	bool WriteUL(std::uint8_t pageAddr, std::vector<std::uint8_t> data); // MIFARE Ultralight : The page(2 - 15) to write to.

public:
	std::string GetVersionString();
	std::string ToString(MFRC522::StatusCode code);
};

}
