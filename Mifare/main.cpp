#include <devConfig.h>
#include <devDataSet.h>
#include <devDataSetConfig.h>
#include <devMFRC522.h>

#include <sharePipe.h>
#include <utilsBase.h>
#include <utilsPacketTWR.h>
#include <utilsPath.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>

namespace TWR = utils::packet_TWR;

enum class tCardConfigType
{
	None,
	Email,
	GoogleDisk,
};

class tCard1KBase : public card_mfr::tCardClassic1K
{
	dev::tMFRC522* m_pCardRW = nullptr;

public:
	tCard1KBase() = delete;
	explicit tCard1KBase(dev::tMFRC522* cardRW) :m_pCardRW(cardRW)
	{
		if (m_pCardRW == nullptr)
			return;

		if (ReadCard(card_mfr::tKeyID::A, {}, false))
			return;

		if (ReadCard(card_mfr::tKeyID::A, card_mfr::tKey(0xa5a4a3a2a1a0), true))
			return;
	}

	tCardConfigType GetType() const { return tCardConfigType::Email; }

protected:
	std::optional<card_mfr::tSector> Card_ReadSector(std::size_t sectorIdx, card_mfr::tKeyID keyID, card_mfr::tKey key) override
	{
		return m_pCardRW->GetCardClassicSector(static_cast<int>(sectorIdx), keyID, key);
	}

private:
	bool ReadCard(card_mfr::tKeyID keyID, card_mfr::tKey key, bool cardEn)
	{
		if (cardEn)
		{
			if (!m_pCardRW->IsAnyCardPresent())
				return false;

			std::vector<std::uint8_t> CardID = m_pCardRW->ReadCardID();
			if (CardID.empty())
			{
				m_pCardRW->HaltCard();
				return false;
			}
		}

		for (int i = 0; i < GetSectorQty(); ++i)
		{
			std::optional<card_mfr::tSector> Sector = ReadSector(i, keyID, key);
			if (!Sector.has_value() || Sector->GetStatus() != card_mfr::tStatus::OK)
				break;

			// [TBD] parse MAD and detect type of card in terms of applications
		}
		m_pCardRW->HaltCard();
		return this->good();
	}
};

class tCardConfig : public tCard1KBase
{
public:
	tCardConfigType GetType() const { return tCardConfigType::Email; }
};

class tCardConfigEmail : private tCardConfig
{
public:
	std::string GetUser() { return {}; }
	bool SetUser(const std::string& val) { return false; }
	std::string GetPassword() { return {}; }
	bool SetPassword(const std::string& val) { return {}; }
	std::string GetEmail() { return {}; }
	bool SetEmail(const std::string& val) { return {}; }
	std::string GetHost() { return {}; }
	bool SetHost(const std::string& val) { return {}; }
	std::uint16_t GetPort() { return {}; }
	void SetPort(std::uint16_t val) {  }
	bool GetTLS() { return false; }
	void SetTLS(bool val) { }
	bool GetAuth() { return false; }
	void GetAuth(bool val) {  }
	bool Write()
	{

		return false;
	}
};

//void WriteCard(card_mfr::tCard1K& card)
//{
//	static bool OnceDoneIt = false;
//	if (OnceDoneIt)
//		return;
//	OnceDoneIt = true;
//
//	tCardConfig CardConfig = static_cast<tCardConfig>(card);
//
//	card_mfr::tSector Sector = CardConfig.GetSector(2);
//	//Sector.Write()
//
//	std::cout << "WriteCard: " << (int)CardConfig.GetType() << '\n';
//
//	// [TBD] Do it here ...
//}

int main(int argc, char* argv[])
{
	try
	{
		const std::filesystem::path Path(argv[0]);
		const std::string AppName = utils::path::GetAppNameMain(Path);
		std::string PathFileConfig = utils::path::GetPathConfigExc(AppName).string();
		std::string PathFileMX = utils::path::GetPathConfigExc("mx").string();

		dev::tDataSetConfig DsConfig(PathFileConfig, PathFileMX);

		dev::tDataSet DataSet;

		share::tPipeO<std::string> PipeO(DsConfig.GetPipe().Path, [&]() { return DataSet.GetCard().ToJSON(); });

#ifdef MXTWR_CLIENT
		boost::asio::io_context ioc;
		share_port::tGPIO PortRST(ioc, TWR::tEndpoint::SPI0_CS0);
#else // MXTWR_CLIENT
		tPortGPIO PortRST(DsConfig.GetSPI().ID);
#endif // MXTWR_CLIENT
		PortRST.SetState(true);

		share_config::tSPIPort SPIPortCfg = DsConfig.GetSPI();

		dev::tMFRC522 DevCardRW(SPIPortCfg);
		std::cout << DevCardRW.GetVersionString() << '\n';
		//DevCardRW.SetAntennaGain(dev::tMFRC522_RxGain::RxGain_48dB);
		std::cout << "Antenna gain: " << (int)DevCardRW.GetAntennaGain() << " dB (min 18, max 48 dB)\n";
		std::cout << "Scan PICC to see UID, SAK, type, and data blocks...\n";

		
		card_mfr::tCardUL CardToWrite{}; // [TBD] - it needs mutex
		share::tPipeI<std::string> PipeI(DsConfig.GetPipe().Path, [&](const std::string& value)
			{
				// [TBD]
				// 0. parse JSON and convert to dev::tCard
				// 1. check that the card is not the same
				// 2. check that the card is the same type
				// 3. write data to the card
				// 
				// #include <boost/property_tree/json_parser.hpp>
				// 
				// Mfrc522.Write(..) - mutex
				std::cout << value << '\n';
			});

		std::vector<std::uint8_t> PrevCardID{};
		while (true)
		{
			if (!DevCardRW.IsNewCardPresent())
			{
				if (!PrevCardID.empty())
				{
					PrevCardID = {};
					DataSet.ClearCard();
					std::cout << "\nno card\n";
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				continue;
			}

			std::vector<std::uint8_t> CardID = DevCardRW.ReadCardID();
			if (CardID.empty())
			{
				std::cout << "\nno serial\n";
				continue;
			}

			if (CardID == PrevCardID)
			{
				std::cout << '.';
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				DevCardRW.HaltCard();
				continue;
			}

			PrevCardID = CardID;

			switch (DevCardRW.GetCardType())
			{
			case card_mfr::tCardType::MIFARE_UL:
			{
				card_mfr::tCardUL Card = DevCardRW.GetCardUltralight();
				std::cout << Card.ToJSON() << '\n';
				break;
			}
			case card_mfr::tCardType::MIFARE_1K:
			{
				tCard1KBase Card(&DevCardRW);
				std::cout << Card.ToJSON() << '\n';
				break;
			}
			default:
				std::cout << "The card of this type is not supported.\n";
				break;
			}

			// [TEST] Wiriting MIFARE UL
			/*dev::tCard::tLock Lock = Card.GetLock();
			std::size_t UMUnlockedSize = Card.GetUserMemoryUnlockedSize();
			static bool Written = false;
			if (!Written)
			{
				Written = true;
				std::vector<std::uint8_t> Data;
				for (int i = 0x33; i < 0xFF; ++i)
					Data.push_back(i);
				Card.WriteUserMemoryUnlocked(Data);
				std::cout << "WriteUserMemory: " << (DevCardRW.WriteCard(Card) ? "OK" : "ERR") << '\n';
			}*/
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return static_cast<int>(utils::tExitCode::EX_IOERR);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}
