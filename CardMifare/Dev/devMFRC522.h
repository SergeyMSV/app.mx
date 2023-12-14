#pragma once

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

namespace card_classic = utils::card_MIFARE::classic;
namespace card_ul = utils::card_MIFARE::ultralight;
namespace card = utils::card_MIFARE;

namespace dev
{

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
	
	std::uint8_t GetAntennaGain();

	bool IsAnyCardPresent(); // All of cards in state IDLE and HALT are invited. Sleeping cards in state HALT are NOT ignored.
	bool IsNewCardPresent(); // Only "new" cards in state IDLE are invited. Sleeping cards in state HALT are ignored.

	std::vector<std::uint8_t> ReadCardID();

	card::tCardType GetCardType() const;
	std::vector<std::uint8_t> GetCardID() const;
	
	card_classic::tCardMini GetCard_MIFARE_ClassicMini(card_classic::tKeyID keyID, card_classic::tKey key);
	card_classic::tCard1K GetCard_MIFARE_Classic1K(card_classic::tKeyID keyID, card_classic::tKey key);
	card_classic::tCard4K GetCard_MIFARE_Classic4K(card_classic::tKeyID keyID, card_classic::tKey key);
	card_ul::tCard GetCard_MIFARE_Ultralight();

	bool WriteCard(const card_ul::tCard& card); // [TBD] that shall be a template for different types of cards

	void HaltCard();

private:
	template <class T>
	void ReadCard_MIFARE_Classic(T& card, card_classic::tKeyID keyID, card_classic::tKey key);
	std::optional<card_classic::tSector> GetCard_MIFARE_ClassicSector(int index, card_classic::tKeyID keyID, card_classic::tKey key);

	std::vector<std::uint8_t> Read(std::uint8_t blockAddr, std::uint8_t size);
	//bool Write(std::uint8_t blockAddr, std::vector<std::uint8_t> data); // MIFARE Classic : The block(0 - 0xff) number. MIFARE Ultralight : The page(2 - 15) to write to.
	bool WriteUL(std::uint8_t pageAddr, std::vector<std::uint8_t> data); // MIFARE Ultralight : The page(2 - 15) to write to.

public:
	std::string GetVersionString();
	std::string ToString(MFRC522::StatusCode code);
};

}
