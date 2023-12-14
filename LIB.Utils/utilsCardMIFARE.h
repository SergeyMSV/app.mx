///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsCardMIFARE
// 2023-11-20
// Standard ISO/IEC 114882, C++20
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <array>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace utils
{
namespace card_MIFARE
{

enum class tCardType : std::uint8_t
{
	None,
	MIFARE_Mini,
	MIFARE_1K,
	MIFARE_4K,
	MIFARE_UL,
};

std::string to_string(tCardType value);

using tUID = std::array<std::uint8_t, 7>;

namespace classic
{

enum class tKeyID { None, A, B, };
enum class tBlockID { Block_0, Block_1, Block_2, Block_3_Trailer, };

using tBlock = std::array<std::uint8_t, 16>;
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
	bool IsR_Data(tKeyID keyID, tBlockID blockID) const;
	bool IsW_Data(tKeyID keyID, tBlockID blockID) const;
	bool IsIncr_Data(tKeyID keyID, tBlockID blockID) const;
	bool IsDecr_Data(tKeyID keyID, tBlockID blockID) const;
	//bool IsTransportConfiguration() const { m_Access.Value == 0x698007FF; } // and both Key A and B shall be equal to 0xFFFFFFFFFFFF

	std::uint32_t GetValue() const { return m_Access.Value; }

	std::string ToString() const;

private:
	tAccessBlock GetAccessBlock(tBlockID blockID) const;
};

class tKey
{
	static constexpr std::size_t KeySize = 6;
	static constexpr std::uint64_t ValueMAX = 0x0000FFFFFFFFFFFF;

	std::uint64_t m_Value = ValueMAX;

public:
	tKey() = default;
	explicit tKey(std::uint64_t value);
	tKey(std::vector<std::uint8_t>::const_iterator itBegin, std::vector<std::uint8_t>::const_iterator itEnd);

	static constexpr std::size_t size() { return KeySize; }

	std::uint64_t Get() const { return m_Value; }

	void CopyTo(std::uint8_t(&key)[KeySize]) const;

	void Increment();
	void Decrement();

	std::string ToString() const;
};

class tSector
{
	static constexpr std::size_t SectorSize = 64;
	static constexpr std::size_t BlockSize = 16;
	static constexpr std::size_t AccessPos = 54;
	static constexpr std::size_t KeyAPos = 48;
	static constexpr std::size_t KeyBPos = 58;

	tKeyID m_KeyID = tKeyID::None;
	tKey m_Key{};
	std::vector<std::uint8_t> m_Payload;

public:
	tSector() = default;
	explicit tSector(tKeyID keyID) :m_KeyID(keyID) {}
	tSector(tKeyID keyID, const tKey& key) :m_KeyID(keyID), m_Key(key) {}
	tSector(tKeyID keyID, const tKey& key, const std::vector<std::uint8_t>& sector);
	tSector(const tSector& val) = default;

	tKey GetKey() const { return m_Key; }

	void push(std::vector<std::uint8_t> sector);
	void push_back_block(std::vector<std::uint8_t> block);
	void push_back_block(tBlock block);
	bool good() const { return m_KeyID != tKeyID::None && m_Payload.size() == SectorSize && GetAccess().good(); } // sector payload size
	tAccess GetAccess() const;
	std::optional<tKey> GetKeyA() const { return {}; }
	std::optional<tKey> GetKeyB() const;
	std::optional<tBlock> GetBlock(tBlockID blockID) const;

	std::string ToJSON() const;
	std::string ToString() const;

private:
	tKey GetKey(int pos) const;
};

class tSector0 : public tSector
{
public:
	tSector0() = default;
	explicit tSector0(tKeyID keyID) :tSector(keyID) {}
	tSector0(tKeyID keyID, const tKey& key) :tSector(keyID, key) {}
	tSector0(tKeyID keyID, const tKey& key, const std::vector<std::uint8_t>& sector) :tSector(keyID, key, sector) {}
	tSector0(const tSector& val) :tSector(val) {}

	std::optional<tNUID> GetNUID() const;
	std::optional<tUID> GetUID() const;

	std::string ToString() const;
};

template <tCardType CardType, std::size_t SectorQty>
class tCardClassic
{
	std::vector<std::uint8_t> m_ID;
	std::array<tSector, SectorQty> m_Sectors;

public:
	tCardClassic() = default;

	bool good() const { return static_cast<tSector0>(m_Sectors[0]).good(); }

	void insert(std::size_t index, const tSector& sector) // [TBD] std::move
	{
		if (index >= SectorQty)
			return;
		m_Sectors[index] = sector; // [TBD] std::move
	}

	std::vector<std::uint8_t> GetID() const { return m_ID; };
	void SetID(const std::vector<std::uint8_t>& id) { m_ID = id; }

	static constexpr tCardType GetType() { return CardType; }
	std::optional<tNUID> GetNUID() const { return static_cast<tSector0>(m_Sectors[0]).GetNUID(); }
	std::optional<tUID> GetUID() const { return static_cast<tSector0>(m_Sectors[0]).GetUID(); }

	static constexpr std::size_t GetSectorQty() { return SectorQty; }
	tSector GetSector(std::size_t index) const
	{
		if (index < SectorQty)
			return m_Sectors[index];
		return {};
	}
};

class tCardMini : public tCardClassic<tCardType::MIFARE_Mini, 5>
{
public:
	std::string ToJSON() const;
	std::string ToString() const;
};

class tCard1K : public tCardClassic<tCardType::MIFARE_1K, 16>
{
public:
	std::string ToJSON() const;
	std::string ToString() const;
};

class tCard4K : public tCardClassic<tCardType::MIFARE_4K, 40>
{
	std::string ToJSON() const;
	std::string ToString() const;
};

}

namespace ultralight
{

enum class tBlockID { Block_0_System, Block_1, Block_2, Block_3, };
using tBlock = std::array<std::uint8_t, 16>;

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

class tCard
{
	static constexpr std::size_t SectorSize = 64;
	static constexpr std::size_t BlockSize = 16;
	static constexpr std::size_t SystemMemorySize = 16;
	static constexpr std::size_t UserMemorySize = 48;
	static constexpr std::size_t PageSize = 4;
	static constexpr std::size_t LockPos = 10;

	std::vector<std::uint8_t> m_ID;
	std::vector<std::uint8_t> m_Payload;

public:
	tCard() = default;
	explicit tCard(const std::vector<std::uint8_t>& id)
		:m_ID(id)
	{}
	tCard(const std::vector<std::uint8_t>& id, const std::vector<std::uint8_t>& payload)
		:m_ID(id), m_Payload(payload)
	{}

	void push(std::vector<std::uint8_t> sector);
	void push_back_block(std::vector<std::uint8_t> block);
	void push_back_block(tBlock block);
	bool good() const { return m_Payload.size() == SectorSize; }

	std::vector<std::uint8_t> GetID() const { return m_ID; };
	void SetID(const std::vector<std::uint8_t>& id) { m_ID = id; }

	static constexpr tCardType GetType() { return tCardType::MIFARE_UL; }
	std::optional<tUID> GetUID() const;

	tLock GetLock() const;
	void SetLock(tLock lock);
	std::optional<tBlock> GetBlock(tBlockID blockID) const;
	std::vector<std::uint8_t> GetUserMemory() const;
	void SetUserMemory(const std::vector<std::uint8_t>& data); // data size must be equal to UserMemorySize
	std::size_t GetUserMemoryUnlockedSize() const;
	std::vector<std::uint8_t> ReadUserMemoryUnlocked(); // data size must be less or equal to UserMemorySize which is not locked
	void WriteUserMemoryUnlocked(std::vector<std::uint8_t> data); // data size must be less or equal to UserMemorySize which is not locked

	std::string ToJSON() const;

	static bool IsPageAvailable(tLock lock, std::size_t pageIndex) { return  pageIndex >= 4 && pageIndex < 16 && !(lock.Value & (1 << pageIndex)); }
};

}
}
}