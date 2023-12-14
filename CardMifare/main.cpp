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
		std::cout << "Antenna gain: " << (int)DevCardRW.GetAntennaGain() << '\n';
		std::cout << "Scan PICC to see UID, SAK, type, and data blocks...\n";

		
		utils::card_MIFARE::ultralight::tCard CardToWrite{}; // [TBD] - it needs mutex
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
			case card::tCardType::MIFARE_UL:
			{
				card_ul::tCard Card = DevCardRW.GetCard_MIFARE_Ultralight();
				std::cout << Card.ToJSON() << '\n';
				break;
			}
			case card::tCardType::MIFARE_1K:
			{
				card_classic::tCard1K Card = DevCardRW.GetCard_MIFARE_Classic1K(/*keys*/);
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
