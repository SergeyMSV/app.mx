///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsMIFARE
// 2023-11-20
// C++20
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <array>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace utils
{
namespace mifare
{

enum class tCardType : std::uint8_t
{
	None,
	MIFARE_Mini,
	MIFARE_1K,
	MIFARE_4K,
	MIFARE_UL,
};

enum class tStatus : std::uint8_t
{
	Default,		// the sector is just created, and not filled
	OK,				// the sector read and filled correctly
	ErrAuth = 0x80,	// Authentification failed
	ErrRead,		// Reading failed
};

std::string to_string(tCardType value);

using tUID = std::array<std::uint8_t, 7>;

using tBlock = std::array<std::uint8_t, 16>;
using tBlockCRC = std::array<std::uint8_t, 18>;

enum class tKeyID { None, A, B, };

using tNUID = std::array<std::uint8_t, 4>;

class tAccess
{
	union
	{
		struct
		{
			std::uint32_t C1_0_ : 1;
			std::uint32_t C1_1_ : 1;
			std::uint32_t C1_2_ : 1;
			std::uint32_t C1_3_ : 1;
			std::uint32_t C2_0_ : 1;
			std::uint32_t C2_1_ : 1;
			std::uint32_t C2_2_ : 1;
			std::uint32_t C2_3_ : 1;
			std::uint32_t C3_0_ : 1;
			std::uint32_t C3_1_ : 1;
			std::uint32_t C3_2_ : 1;
			std::uint32_t C3_3_ : 1;
			std::uint32_t C1_0 : 1;
			std::uint32_t C1_1 : 1;
			std::uint32_t C1_2 : 1;
			std::uint32_t C1_3 : 1;
			std::uint32_t C2_0 : 1;
			std::uint32_t C2_1 : 1;
			std::uint32_t C2_2 : 1;
			std::uint32_t C2_3 : 1;
			std::uint32_t C3_0 : 1;
			std::uint32_t C3_1 : 1;
			std::uint32_t C3_2 : 1;
			std::uint32_t C3_3 : 1;
			std::uint32_t UserData : 8;
		}Field;
		std::uint32_t Value = 0;
	} m_Access;

	struct tAccessBlock
	{
		bool C1 = false;
		bool C2 = false;
		bool C3 = false;
	};

public:
	tAccess() = default;
	explicit tAccess(std::uint32_t value);

	bool good() const;

	bool IsR_KeyA(tKeyID keyID) const { return false; }
	bool IsW_KeyA(tKeyID keyID) const;
	bool IsR_KeyB(tKeyID keyID) const;
	bool IsW_KeyB(tKeyID keyID) const;
	bool IsR_Access(tKeyID keyID) const;
	bool IsW_Access(tKeyID keyID) const;
	//bool IsR_UserData(tKeyID keyID) const { return true; }
	//bool IsW_UserData(tKeyID keyID) const { return true; }
	bool IsR_Data(std::size_t sectorIdx, std::size_t blockIdx, tKeyID keyID) const;
	bool IsW_Data(std::size_t sectorIdx, std::size_t blockIdx, tKeyID keyID) const;
	bool IsIncr_Data(std::size_t sectorIdx, std::size_t blockIdx, tKeyID keyID) const;
	bool IsDecr_Data(std::size_t sectorIdx, std::size_t blockIdx, tKeyID keyID) const;
	//bool IsTransportConfiguration() const { m_Access.Value == 0x698007FF; } // and both Key A and B shall be equal to 0xFFFFFFFFFFFF

	std::uint32_t GetValue() const { return m_Access.Value; }

	std::string ToString() const;

	enum class tCluster
	{
		_0,
		_1,
		_2,
		_3_Trailer
	};
	static tCluster GetCluster(std::size_t sectorIdx, std::size_t blockIdx);

private:
	tAccessBlock GetAccessBlock(std::size_t sectorIdx, std::size_t blockIdx) const;
	static bool IsTrailer(std::size_t sectorIdx, std::size_t blockIdx);
};

class tKey
{
	static constexpr std::size_t KeySize = 6;
	static constexpr std::uint64_t ValueMAX = 0x0000FFFFFFFFFFFF;

	std::uint64_t m_Value = ValueMAX;

public:
	tKey() = default;
	explicit tKey(std::uint64_t value);
	template<class T>
	tKey(T itBegin, T itEnd)
	{
		if (std::distance(itBegin, itEnd) != KeySize)
			m_Value = ValueMAX;
		m_Value = 0;
		std::copy_n(itBegin, std::min(KeySize, sizeof(m_Value)), (char*)&m_Value);
	}

	static constexpr std::size_t size() { return KeySize; }

	std::uint64_t Get() const { return m_Value; }

	void CopyTo(std::uint8_t(&key)[KeySize]) const;

	void Increment();
	void Decrement();

	std::string ToString() const;
};

class tSector
{
	static constexpr std::size_t BlockSize = 16;
	static constexpr std::size_t AccessBlockPos = 6;
	static constexpr std::size_t KeyABlockPos = 0;
	static constexpr std::size_t KeyBBlockPos = 10;

	tKeyID m_KeyID = tKeyID::None;
	tStatus m_Status = tStatus::Default;

protected:
	std::vector<tBlock> m_Blocks;

public:
	tSector() = default;
	tSector(std::size_t blockQty, tKeyID keyID);
	tSector(std::size_t blockQty, tKeyID keyID, tStatus status);
	tSector(const tSector& val) = default;

	tStatus GetStatus() const { return m_Status; }
	void SetStatus(tStatus val) { m_Status = val; }

	bool good() const { return m_KeyID != tKeyID::None && GetAccess().good(); } // sector payload size
	std::size_t size() const { return m_Blocks.size(); }

	tAccess GetAccess() const;
	std::optional<tKey> GetKeyA() const { return {}; }
	std::optional<tKey> GetKeyB() const;
	std::optional<tBlock> GetBlock(std::size_t blockIdx) const;
	bool SetBlock(std::size_t blockIdx, const tBlock& block);
	bool SetBlock(std::size_t blockIdx, const tBlockCRC& block);

	std::string ToJSON() const;
	std::string ToString() const;

	virtual std::vector<std::uint8_t> GetPayload() const;

private:
	tKey GetKey(int pos) const;
	virtual std::optional<tBlock> GetBlockSystem() const { return {}; }
	std::optional<tBlock> GetBlockTrailer() const { return GetBlock(m_Blocks.size() - 1); }
};

class tSector0 : public tSector // This kind of sector contains manufacturer data.
{
public:
	tSector0() = default;
	explicit tSector0(tKeyID keyID) :tSector(4, keyID) {}
	tSector0(tKeyID keyID, tStatus status) :tSector(4, keyID, status)	{}
	tSector0(const tSector& val) :tSector(val) {}

	std::optional<tNUID> GetNUID() const;
	std::optional<tUID> GetUID() const;

	std::string ToString() const;

	std::vector<std::uint8_t> GetPayload() const override;

private:
	std::optional<tBlock> GetBlockSystem() const override { return GetBlock(0); }
};

class tSector4 : public tSector // This kind of sector consists of 4 blocks.
{
public:
	tSector4() = default;
	explicit tSector4(tKeyID keyID) :tSector(4, keyID) {}
	tSector4(tKeyID keyID, tStatus status) :tSector(4, keyID, status) {}
	tSector4(const tSector& val) :tSector(val) {}
};

class tSector16 : public tSector // This kind of sector consists of 16 blocks.
{
public:
	tSector16() = default;
	explicit tSector16(tKeyID keyID) :tSector(16, keyID) {}
	tSector16(tKeyID keyID, tStatus status) :tSector(16, keyID, status) {}
	tSector16(const tSector& val) :tSector(val) {}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace mad
{

union tInfoByte
{
	struct
	{
		std::uint8_t CPS : 6; // Pointer to CPS (Card Publisher Sector); 0x10 shall not be used, 0x28...0x3F shall not be used
		std::uint8_t RFU : 2;
	}Field;
	std::uint8_t Value{};

};

union tGPB // General Purpose Byte
{
	struct
	{
		std::uint8_t ADV : 2; // 01 -> MAD1 (Sectors 1...0x0F), 10 -> MAD2 (Sectors 1...0x27)
		std::uint8_t RFU : 4; // Default value is 0xA
		std::uint8_t MA : 1; // 0 -> Monoapplication card, 1 -> Multiapplication card
		std::uint8_t DA : 1; // 0 -> Sector 0 doesn't contain MAD, 1 -> MAD available
	}Field;
	std::uint8_t Value{}; // Default value is 0x69
};

enum class tFCCode : std::uint8_t// Function Cluster Code
{
	CardAdministration, // For administration codes
	MiscApp_1, // miscellaneous applications
	MiscApp_7 = 0x07,
	Airlines,
	FerryTrafic,
	RailwayServices,
	Transport = 0x12,
	CityTraffic = 0x18,
	BusServices = 0x20,
	CompanyServices = 0x38,
	CityCardServices = 0x40,
	AccessControlSecurity_1 = 0x47,
	AccessControlSecurity_2,
	AccessControlSecurity_3 = 0x51,
	AccessControlSecurity_4,
	AccessControlSecurity_5,
	AccessControlSecurity_6,
	AdministrationServices = 0x80,
	ElectronicPurse = 0x88,
	Metering = 0x97,
	Telephone,
	FleetManagement = 0xC9,
	InfoServices = 0xD8,
	Computer = 0xE8,
	Mail=0xF0,
	MiscApp_8 = 0xF8,
	MiscApp_9 = 0xF9,
	MiscApp_10 = 0xFA,
	MiscApp_11 = 0xFB,
	MiscApp_12 = 0xFC,
	MiscApp_13 = 0xFD,
	MiscApp_14 = 0xFE,
	MiscApp_15 = 0xFF
};

enum class tAppCode : std::uint8_t // Application Code
{
	CardAdministration_SectorFree,			// hexsector is free
	CardAdministration_SectorDefect,		// hexsector is defect, e.g. access keys are destroyed or unknown
	CardAdministration_SectorReserved,		// hexsector is reserved
	CardAdministration_SectorAddDir,		// hexsector contains additional directory info (useful only for future cards)
	CardAdministration_SectorCPS,			// hexsector contains card holder information in ASCII format
	CardAdministration_SectorNotApplicable,	// hexsector not applicable (above memory size)
	MiscApp_7_Email = 0x51,	// My Application
	MiscApp_7_CloudDrive = 0x52,	// My Application
};

struct tAID // Application identifier
{
	tAppCode AppCode{};
	tFCCode FCCode{};
};

class tMAD
{
	tInfoByte m_InfoByte;
	tGPB m_GPB;
	std::array<tAID, 15> m_AIDs;

	tMAD() = default;

public:
	static std::optional<tMAD> GetMAD(const tSector0& sector);

	//std::size_t GetCardPublisherSectorIdx() const;
};
}
///////////////////////////////////////////////////////////////////////////////////////////////////
template <tCardType CardType, std::size_t SectorQty>
class tCardClassic
{
	std::vector<std::uint8_t> m_ID;
	std::array<tSector, SectorQty> m_Sectors;

public:
	tCardClassic() = default;

	bool good() const { return static_cast<tSector0>(m_Sectors[0]).good(); }

	std::vector<std::uint8_t> GetID() const { return m_ID; };
	void SetID(const std::vector<std::uint8_t>& id) { m_ID = id; }

	static constexpr tCardType GetType() { return CardType; }
	std::optional<tNUID> GetNUID() const { return static_cast<tSector0>(m_Sectors[0]).GetNUID(); }
	std::optional<tUID> GetUID() const { return static_cast<tSector0>(m_Sectors[0]).GetUID(); }

	static constexpr std::size_t GetSectorQty() { return SectorQty; }
	tSector GetSector(std::size_t sectorIdx) const
	{
		if (sectorIdx < SectorQty)
			return m_Sectors[sectorIdx];
		return {};
	}
	void SetSector(std::size_t sectorIdx, const tSector& sector) // [TBD] std::move
	{
		if (sectorIdx >= SectorQty)
			return; // [TBD] may be an exception should be thrown
		if (sectorIdx < 32 && sector.size() != 4)
			return; // [TBD] may be an exception should be thrown
		if (sectorIdx > 31 && sectorIdx < 40 && sector.size() != 16)
			return; // [TBD] may be an exception should be thrown
		m_Sectors[sectorIdx] = sector; // [TBD] std::move
	}

	std::optional<tSector> ReadSector(std::size_t sectorIdx, tKeyID keyID, tKey key)
	{
		std::optional<tSector> Sector = Card_ReadSector(sectorIdx, keyID, key);
		if (Sector.has_value())
			SetSector(sectorIdx, *Sector);
		return Sector;
	}

	std::optional<mad::tMAD> GetMAD() const
	{
		return mad::tMAD::GetMAD(m_Sectors[0]);
	}

protected:
	virtual std::optional<tSector> Card_ReadSector(std::size_t sectorIdx, tKeyID keyID, tKey key) { return {}; }
};

class tCardClassicMini : public tCardClassic<tCardType::MIFARE_Mini, 5>
{
public:
	std::string ToJSON() const;
	std::string ToString() const;
};

class tCardClassic1K : public tCardClassic<tCardType::MIFARE_1K, 16>
{
public:
	std::string ToJSON() const;
	std::string ToString() const;
};

class tCardClassic4K : public tCardClassic<tCardType::MIFARE_4K, 40>
{
public:
	std::string ToJSON() const;
	std::string ToString() const;
};

// --- Ultralight

union tLock
{
	struct
	{
		std::uint16_t BL_OTP : 1;
		std::uint16_t BL_09_04 : 1;
		std::uint16_t BL_15_10 : 1;
		std::uint16_t L_OTP : 1;
		std::uint16_t L_04 : 1;
		std::uint16_t L_05 : 1;
		std::uint16_t L_06 : 1;
		std::uint16_t L_07 : 1;
		std::uint16_t L_08 : 1;
		std::uint16_t L_09 : 1;
		std::uint16_t L_10 : 1;
		std::uint16_t L_11 : 1;
		std::uint16_t L_12 : 1;
		std::uint16_t L_13 : 1;
		std::uint16_t L_14 : 1;
		std::uint16_t L_15 : 1;
	}Field;
	std::uint16_t Value = 0;
};

class tCardUL
{
	static constexpr std::size_t SectorSize = 64;
	static constexpr std::size_t BlockSize = 16;
	static constexpr std::size_t SystemMemorySize = 16;
	static constexpr std::size_t UserMemorySize = 48;
	static constexpr std::size_t PageSize = 4;
	static constexpr std::size_t LockPos = 10;

	std::vector<std::uint8_t> m_ID;
	std::array<tBlock, 4> m_Blocks{};
	tStatus m_Status = tStatus::Default;

public:
	tCardUL() = default;
	explicit tCardUL(const std::vector<std::uint8_t>& id) :m_ID(id) {}

	tStatus GetStatus() const { return m_Status; }
	void SetStatus(tStatus val) { m_Status = val; }

	bool good() const { return true; }// return m_Payload.size() == SectorSize; [TBD] remove

	std::vector<std::uint8_t> GetID() const { return m_ID; };
	void SetID(const std::vector<std::uint8_t>& id) { m_ID = id; }

	static constexpr tCardType GetType() { return tCardType::MIFARE_UL; }
	std::optional<tUID> GetUID() const;

	tLock GetLock() const;
	void SetLock(tLock lock);
	std::optional<tBlock> GetBlock(std::size_t blockIdx) const;
	bool SetBlock(std::size_t blockIdx, const tBlock& block);
	bool SetBlock(std::size_t blockIdx, const tBlockCRC& block);

	std::vector<std::uint8_t> GetUserMemory() const;
	void SetUserMemory(const std::vector<std::uint8_t>& data); // data size must be equal to UserMemorySize
	std::size_t GetUserMemoryUnlockedSize() const;
	std::vector<std::uint8_t> ReadUserMemoryUnlocked(); // data size must be less or equal to UserMemorySize which is not locked
	void WriteUserMemoryUnlocked(std::vector<std::uint8_t> data); // data size must be less or equal to UserMemorySize which is not locked

	std::string ToJSON() const;
	//std::string ToString() const;

	static bool IsPageAvailable(tLock lock, std::size_t pageIdx) { return  pageIdx >= 4 && pageIdx < 16 && !(lock.Value & (1 << pageIdx)); }

private:
	static std::size_t GetBlockIdxForPage(int pageIdx);
};

}
}
