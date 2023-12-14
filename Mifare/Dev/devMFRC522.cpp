#include "devMFRC522.h"

#include <utilsBase.h>

#include <iostream>//[TEST]
#include <iomanip>//[TEST]

#include <algorithm>
#include <chrono>
#include <sstream>
#include <thread>

#include <utilsMIFARE.h>//[TEST]

namespace dev
{

tMFRC522::tMFRC522(const share_config::tSPIPort& confSPI)
	:m_PortSPI(confSPI.ID, confSPI.Mode, confSPI.Bits, confSPI.Frequency_hz, confSPI.Delay_us), m_MFRC522(m_PortSPI)
{
	m_MFRC522.PCD_Init(); // Init MFRC522 card
	std::this_thread::sleep_for(std::chrono::milliseconds(4)); // Optional delay. Some board do need more time after init to be ready, see Readme
}

tMFRC522::~tMFRC522()
{
}

std::uint8_t tMFRC522::GetAntennaGain()
{
	std::uint8_t RxGain = 0;
	{
		std::lock_guard<std::recursive_mutex> Lock(m_MFRC522_mtx);
		RxGain = m_MFRC522.PCD_GetAntennaGain() >> 4;
	}

	switch (RxGain) // returns in dB
	{
	case 0:
	case 2: return 18;
	case 1:
	case 3: return 23;
	case 4: return 33;
	case 5: return 38;
	case 6: return 43;
	case 7: return 48;
	}
	return 0;
}

void tMFRC522::SetAntennaGain(tMFRC522_RxGain value)
{
	std::lock_guard<std::recursive_mutex> Lock(m_MFRC522_mtx);
	m_MFRC522.PCD_SetAntennaGain(value);
}

bool tMFRC522::IsAnyCardPresent()
{
	std::lock_guard<std::recursive_mutex> Lock(m_MFRC522_mtx);
	return m_MFRC522.PICC_IsAnyCardPresent();
}

bool tMFRC522::IsNewCardPresent()
{
	std::lock_guard<std::recursive_mutex> Lock(m_MFRC522_mtx);
	return m_MFRC522.PICC_IsNewCardPresent();
}

std::vector<std::uint8_t> tMFRC522::ReadCardID()
{
	{
		std::lock_guard<std::recursive_mutex> Lock(m_MFRC522_mtx);
		if (!m_MFRC522.PICC_ReadCardSerial())
			return {};
	}
	return GetCardID();
}

utils::MIFARE::tCardType tMFRC522::GetCardType() const
{
	switch (m_MFRC522.PICC_GetType(m_MFRC522.uid.sak))
	{
	case MFRC522::PICC_Type::PICC_TYPE_MIFARE_UL:	return card_mfr::tCardType::MIFARE_UL;
	case MFRC522::PICC_Type::PICC_TYPE_MIFARE_MINI:	return card_mfr::tCardType::MIFARE_Mini;
	case MFRC522::PICC_Type::PICC_TYPE_MIFARE_1K:	return card_mfr::tCardType::MIFARE_1K;
	case MFRC522::PICC_Type::PICC_TYPE_MIFARE_4K:	return card_mfr::tCardType::MIFARE_4K;
	}
	return  utils::MIFARE::tCardType::None;
}

std::vector<std::uint8_t> tMFRC522::GetCardID() const
{
	return std::vector<std::uint8_t>(m_MFRC522.uid.uidByte, m_MFRC522.uid.uidByte + m_MFRC522.uid.size);
}

card_mfr::tSector tMFRC522::GetCardClassicSector(int sectorIdx, card_mfr::tKeyID keyID, card_mfr::tKey key)
{
	std::uint8_t BlockFirst = 0;
	std::uint8_t BlockQty = 0;
	if (sectorIdx < 32) // Sectors 0..31 has 4 blocks each
	{
		BlockQty = 4;
		BlockFirst = sectorIdx * BlockQty;
	}
	// [TBD] The following statement is not tested yet.
	else if (sectorIdx < 40) // Sectors 32-39 has 16 blocks each
	{
		BlockQty = 16;
		BlockFirst = 128 + (sectorIdx - 32) * BlockQty;
	}
	else
		return {};

	std::lock_guard<std::recursive_mutex> Lock(m_MFRC522_mtx);

	MFRC522::MIFARE_Key SectorKey;
	key.CopyTo(SectorKey.keyByte);

	card_mfr::tSector Sector(BlockQty, keyID);

	for (int8_t i = 0; i < BlockQty; ++i)
	{
		const byte BlockIndex = BlockFirst + i;

		auto Stat = m_MFRC522.PCD_Authenticate(MFRC522::PICC_Command::PICC_CMD_MF_AUTH_KEY_A, BlockIndex, &SectorKey, &m_MFRC522.uid);
		if (Stat != MFRC522::StatusCode::STATUS_OK)
		{
			Sector.SetStatus(card_mfr::tStatus::ErrAuth);
			return Sector;
		}

		card_mfr::tBlockCRC BlockRead{}; // Block size + CRC size = 18
		std::uint8_t ReadBytes = static_cast<std::uint8_t>(BlockRead.size());
		Stat = m_MFRC522.MIFARE_Read(BlockIndex, BlockRead.data(), &ReadBytes);
		if (Stat != MFRC522::StatusCode::STATUS_OK)
		{
			Sector.SetStatus(card_mfr::tStatus::ErrRead);
			return Sector;
		}

		Sector.SetBlock(i, BlockRead);
	}

	Sector.SetStatus(card_mfr::tStatus::OK);
	return Sector;
}

card_mfr::tCardUL tMFRC522::GetCardUltralight()
{
	card_mfr::tStatus Status = card_mfr::tStatus::OK;
	card_mfr::tCardUL Card(GetCardID());
	for (int blockIdx = 0; blockIdx < 4; ++blockIdx)
	{
		std::lock_guard<std::recursive_mutex> Lock(m_MFRC522_mtx);
		card_mfr::tBlockCRC BlockRead{}; // Block size + CRC size = 18. Buffer size, at least 18 bytes. Also number of bytes returned if StatusCode::STATUS_OK.
		std::uint8_t ReadBytes = static_cast<std::uint8_t>(BlockRead.size());
		MFRC522::StatusCode Stat = m_MFRC522.MIFARE_Read(blockIdx * 4, BlockRead.data(), &ReadBytes);
		if (Stat == MFRC522::StatusCode::STATUS_OK)
		{
			Card.SetBlock(blockIdx, BlockRead);
		}
		else //THROW_RUNTIME_ERROR("Read error: " + std::to_string(Cerr));
		{
			Status = card_mfr::tStatus::ErrRead;
			break;
		}		
	}
	HaltCard();
	Card.SetStatus(Status);
	return Card;
}

bool tMFRC522::WriteCard(const card_mfr::tCardUL& card)
{
	if (card.GetType() == utils::MIFARE::tCardType::MIFARE_UL)// || card.GetUID() != GetUID())
	//if (card.SAK != GetSAK() || card.UID != GetUID())
		return false;
	const card_mfr::tLock CardLock = card.GetLock();
	std::vector<std::uint8_t> UserMemory = card.GetUserMemory();
	// pageAddr = 2 - Lock Bytes (irreversible)
	// pageAddr = 3 - OTP (irreversible)
	// The lockingand block - locking bits are set by a WRITE command to page 2. Bytes 2 and 3 
	// of the WRITE command, and the contents of the lock bytes are bitwise OR’edand the
	// result then becomes the new contents of the lock bytes.This process is irreversible if a bit
	// is set to logic 1, it cannot be changed back to logic 0.
	for (std::uint8_t i = 0, pageAddr = 4; i < UserMemory.size() && pageAddr <= 0x0F; i += 4, ++pageAddr)
	{
		if (!card_mfr::tCardUL::IsPageAvailable(CardLock, pageAddr))
			continue;
		std::size_t Size = UserMemory.size() - i;
		if (Size > 4)
			Size = 4;
		const auto UserMemoryIter = UserMemory.begin() + i;
		std::vector<std::uint8_t> UserMemoryChunk(UserMemoryIter, UserMemoryIter + Size);
		if (!WriteUL(static_cast<std::uint8_t>(pageAddr), UserMemoryChunk)) // it stops writing if a page is not written because of the lock, for example
			return false;
	}
	return true;
}

void tMFRC522::HaltCard()
{
	std::lock_guard<std::recursive_mutex> Lock(m_MFRC522_mtx);
	m_MFRC522.PICC_HaltA(); // Halt the PICC before stopping the encrypted session.
	if (GetCardType() != card_mfr::tCardType::MIFARE_UL)
		m_MFRC522.PCD_StopCrypto1();
}


//bool tMFRC522::Write(std::uint8_t blockAddr, std::vector<std::uint8_t> data)
//{
//	if (data.empty() || data.size() > 16)
//		return false;
// 
//	std::lock_guard<std::mutex> Lock(m_MFRC522_mtx);
// 
//	MFRC522::StatusCode Cerr = m_MFRC522.MIFARE_Write(blockAddr, data.data(), data.size());
//	if (Cerr != MFRC522::StatusCode::STATUS_OK)
//		return false;//THROW_RUNTIME_ERROR("Write error: " + std::to_string(Cerr));
//	return true;
//}

bool tMFRC522::WriteUL(std::uint8_t pageAddr, std::vector<std::uint8_t> data)
{
	if (data.empty() || data.size() > 16)
		return false;

	std::lock_guard<std::recursive_mutex> Lock(m_MFRC522_mtx);

	MFRC522::StatusCode Cerr = m_MFRC522.MIFARE_Ultralight_Write(pageAddr, data.data(), static_cast<std::uint8_t>(data.size()));
	if (Cerr != MFRC522::StatusCode::STATUS_OK)
		return false;//THROW_RUNTIME_ERROR("Write error: " + std::to_string(Cerr));
	return true;
}

std::string tMFRC522::GetVersionString()
{
	MFRC522::PCD_Version Version;
	{
		std::lock_guard<std::recursive_mutex> Lock(m_MFRC522_mtx);

		Version = m_MFRC522.PCD_GetVersion();
	}
	if (Version == MFRC522::PCD_Version::Version_Unknown) // When 0x00 or 0xFF is returned, communication probably failed
		return "WARNING: Communication failure, is the MFRC522 properly connected?";

	std::stringstream SStr;
	SStr << "Firmware Version: 0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(Version) << ' ';
	switch (Version)
	{
	case 0xb2: SStr << "FM17522_1"; break;
	case 0x88: SStr << "FM17522"; break;
	case 0x89: SStr << "FM17522E"; break;
	case 0x90: SStr << "v0.0"; break;
	case 0x91: SStr << "v1.0"; break;
	case 0x92: SStr << "v2.0"; break;
	case 0x12: SStr << "counterfeit chip"; break;
	default: SStr << "(unknown)"; break;
	}
	return SStr.str();
}

std::string tMFRC522::ToString(MFRC522::StatusCode code)
{
#undef STATUS_TIMEOUT // defined in winnt.h
	switch (code)
	{
	case MFRC522::StatusCode::STATUS_OK: return "Success.";
	case MFRC522::StatusCode::STATUS_ERROR: return "Error in communication.";
	case MFRC522::StatusCode::STATUS_COLLISION: return "collision detected.";
	case MFRC522::StatusCode::STATUS_TIMEOUT: return "Timeout in communication.";
	case MFRC522::StatusCode::STATUS_NO_ROOM: return "A buffer is not big enough.";
	case MFRC522::StatusCode::STATUS_INTERNAL_ERROR: return "Internal error in the code. Should not happen.";
	case MFRC522::StatusCode::STATUS_INVALID: return "Invalid argument.";
	case MFRC522::StatusCode::STATUS_CRC_WRONG: return "The CRC_A does not match.";
	case MFRC522::StatusCode::STATUS_MIFARE_NACK: return "A MIFARE PICC responded with NAK.";
	}
	return "Unknown error";
}

}