#include "utilsMIFARE.h"

#include <algorithm>
#include <iomanip>

namespace utils
{
namespace mifare
{

template <class T>
std::string ToStringHEX(const T& val, bool space)
{
	std::stringstream SStr;
	std::for_each(val.begin(), val.end(), [&SStr, &space](std::uint8_t val) { SStr << std::setfill('0') << std::setw(2) << std::hex << (int)val << (space ? " " : ""); });
	return SStr.str();
}

template <class T>
std::string ToStringHEX(const T& val)
{
	return ToStringHEX(val, false);
}

std::string to_string(tCardType value)
{
	std::stringstream SStr;
	SStr << std::setfill('0') << std::setw(2) << std::hex << (int)value;
	return SStr.str();
}

std::string to_string(tStatus value)
{
	switch (value)
	{
	case tStatus::Default:	return "default";
	case tStatus::OK:			return "OK";
	case tStatus::ErrAuth:	return "err:auth";
	case tStatus::ErrRead:	return "err:read";
	}
	return "UNKNOWN";
}

tAccess::tAccess(std::uint32_t value)
{
	m_Access.Value = value;
}

bool tAccess::good() const
{
	return
		m_Access.Field.C1_0 != m_Access.Field.C1_0_ &&
		m_Access.Field.C2_0 != m_Access.Field.C2_0_ &&
		m_Access.Field.C3_0 != m_Access.Field.C3_0_ &&
		m_Access.Field.C1_1 != m_Access.Field.C1_1_ &&
		m_Access.Field.C2_1 != m_Access.Field.C2_1_ &&
		m_Access.Field.C3_1 != m_Access.Field.C3_1_ &&
		m_Access.Field.C1_2 != m_Access.Field.C1_2_ &&
		m_Access.Field.C2_2 != m_Access.Field.C2_2_ &&
		m_Access.Field.C3_2 != m_Access.Field.C3_2_ &&
		m_Access.Field.C1_3 != m_Access.Field.C1_3_ &&
		m_Access.Field.C2_3 != m_Access.Field.C2_3_ &&
		m_Access.Field.C3_3 != m_Access.Field.C3_3_;
}

bool tAccess::IsW_KeyA(tKeyID keyID) const
{
	bool AccKeyA = keyID == tKeyID::A && !m_Access.Field.C1_3 && !m_Access.Field.C2_3;
	bool AccKeyB = keyID == tKeyID::B &&
		(!m_Access.Field.C1_3 && m_Access.Field.C2_3 && m_Access.Field.C3_3 ||
			m_Access.Field.C1_3 && !m_Access.Field.C2_3 && !m_Access.Field.C3_3);
	return AccKeyA || AccKeyB;
}

bool tAccess::IsR_KeyB(tKeyID keyID) const
{
	return keyID == tKeyID::A && !m_Access.Field.C1_3 && !(m_Access.Field.C2_3 && m_Access.Field.C3_3);
}

bool tAccess::IsW_KeyB(tKeyID keyID) const
{
	return IsW_KeyA(keyID);
}

bool tAccess::IsR_Access(tKeyID keyID) const
{
	bool AccKeyB = keyID == tKeyID::B &&
		(m_Access.Field.C1_3 && !m_Access.Field.C3_3 ||
			!m_Access.Field.C1_3 && m_Access.Field.C2_3 && m_Access.Field.C3_3 ||
			m_Access.Field.C1_3 && m_Access.Field.C3_3);
	return keyID == tKeyID::A || AccKeyB;
}

bool tAccess::IsW_Access(tKeyID keyID) const
{
	bool AccKeyA = keyID == tKeyID::A && !m_Access.Field.C1_3 && !m_Access.Field.C2_3 && m_Access.Field.C3_3;
	bool AccKeyB = keyID == tKeyID::B && m_Access.Field.C3_3 &&
		(!m_Access.Field.C1_3 && m_Access.Field.C2_3 || m_Access.Field.C1_3 && !m_Access.Field.C2_3);
	return AccKeyA || AccKeyB;
}

bool tAccess::IsR_Data(std::size_t sectorIdx, std::size_t blockIdx, tKeyID keyID) const
{
	if (IsTrailer(sectorIdx, blockIdx))
		return false;
	auto Check = [&keyID](tAccessBlock a)
	{
		bool AccKeyA = keyID == tKeyID::A && (!a.C3 || !a.C1 && !a.C2 && a.C3);
		bool AccKeyB = keyID == tKeyID::B && (!a.C3 || a.C1 != a.C2 && a.C3 || !a.C1 && !a.C2 && a.C3);
		return AccKeyA || AccKeyB;
	};
	return Check(GetAccessBlock(sectorIdx, blockIdx));
}

bool tAccess::IsW_Data(std::size_t sectorIdx, std::size_t blockIdx, tKeyID keyID) const
{
	if (IsTrailer(sectorIdx, blockIdx))
		return false;
	auto Check = [&keyID](tAccessBlock a)
	{
		bool AccKeyA = keyID == tKeyID::A && !a.C1 && !a.C2 && !a.C3;
		bool AccKeyB = keyID == tKeyID::B && (!a.C1 && a.C2 == a.C3 || a.C1 && !a.C3);
		return AccKeyA || AccKeyB;
	};
	return Check(GetAccessBlock(sectorIdx, blockIdx));
}

bool tAccess::IsIncr_Data(std::size_t sectorIdx, std::size_t blockIdx, tKeyID keyID) const
{
	if (IsTrailer(sectorIdx, blockIdx))
		return false;
	auto Check = [&keyID](tAccessBlock a)
	{
		bool AccKeyA = keyID == tKeyID::A && !a.C1 && !a.C2 && !a.C3;
		bool AccKeyB = keyID == tKeyID::B && !a.C3 && a.C1 == a.C2;
		return AccKeyA || AccKeyB;
	};
	return Check(GetAccessBlock(sectorIdx, blockIdx));
}

bool tAccess::IsDecr_Data(std::size_t sectorIdx, std::size_t blockIdx, tKeyID keyID) const
{
	if (IsTrailer(sectorIdx, blockIdx))
		return false;
	auto Check = [&keyID](tAccessBlock a)
	{
		return !a.C3 && a.C1 == a.C2 || !a.C1 && !a.C2 && a.C3;
	};
	return Check(GetAccessBlock(sectorIdx, blockIdx));
}

std::string tAccess::ToString() const
{
	if (!good())
		return "wrong";
	std::stringstream SStr;
	SStr << (int)m_Access.Field.C1_0 << ' ' << (int)m_Access.Field.C2_0 << ' ' << (int)m_Access.Field.C3_0 << "   ";
	SStr << (int)m_Access.Field.C1_1 << ' ' << (int)m_Access.Field.C2_1 << ' ' << (int)m_Access.Field.C3_1 << "   ";
	SStr << (int)m_Access.Field.C1_2 << ' ' << (int)m_Access.Field.C2_2 << ' ' << (int)m_Access.Field.C3_2 << "   ";
	SStr << (int)m_Access.Field.C1_3 << ' ' << (int)m_Access.Field.C2_3 << ' ' << (int)m_Access.Field.C3_3 << "   ";
	SStr << std::setw(2) << std::setfill('0') << std::hex << (int)m_Access.Field.UserData;
	return SStr.str();
}

tAccess::tCluster tAccess::GetCluster(std::size_t sectorIdx, std::size_t blockIdx)
{
	if (sectorIdx < 32)
		return static_cast<tCluster>(blockIdx);

	if (blockIdx < 5) // 0-4
		return tCluster::_0;
	if (blockIdx < 10) // 5-9
		return tCluster::_1;
	if (blockIdx < 15) // 10-14
		return tCluster::_2;
	return tCluster::_3_Trailer; // 15
}

tAccess::tAccessBlock tAccess::GetAccessBlock(std::size_t sectorIdx, std::size_t blockIdx) const
{
	switch (GetCluster(sectorIdx, blockIdx))
	{
	case tCluster::_0: return { !!m_Access.Field.C1_0, !!m_Access.Field.C2_0, !!m_Access.Field.C3_0 };
	case tCluster::_1: return { !!m_Access.Field.C1_1, !!m_Access.Field.C2_1, !!m_Access.Field.C3_1 };
	case tCluster::_2: return { !!m_Access.Field.C1_2, !!m_Access.Field.C2_2, !!m_Access.Field.C3_2 };
	case tCluster::_3_Trailer: return { !!m_Access.Field.C1_3, !!m_Access.Field.C2_3, !!m_Access.Field.C3_3 };
	}
	return {};
}

bool tAccess::IsTrailer(std::size_t sectorIdx, std::size_t blockIdx)
{
	return GetCluster(sectorIdx, blockIdx) == tCluster::_3_Trailer;
}

tKey::tKey(std::uint64_t value) :m_Value(value)
{
	if (m_Value > ValueMAX)
		m_Value = ValueMAX;
}

void tKey::CopyTo(std::uint8_t(&key)[KeySize]) const
{
	std::copy_n((std::uint8_t*)&m_Value, std::min(KeySize, sizeof(m_Value)), key);
}

void tKey::Increment()
{
	if (++m_Value > ValueMAX)
		m_Value = 0;
}

void tKey::Decrement()
{
	if (m_Value-- == 0)
		m_Value = ValueMAX;
}

std::string tKey::ToString() const
{
	std::stringstream SStr;
	std::vector<std::uint8_t> ValueArray;
	std::copy_n((char*)&m_Value, std::min(KeySize, sizeof(m_Value)), std::back_inserter(ValueArray));
	SStr << ToStringHEX(std::vector<std::uint8_t>(ValueArray), true);
	return SStr.str();
}

tSector::tSector(std::size_t bloclQty, tKeyID keyID)
	:m_KeyID(keyID)
{
	m_Blocks.resize(bloclQty);
}

tSector::tSector(std::size_t bloclQty, tKeyID keyID, tStatus status)
	:m_KeyID(keyID), m_Status(status)
{
	m_Blocks.resize(bloclQty);
}

tAccess tSector::GetAccess() const
{
	if (m_Blocks.empty())
		return {};
	std::uint32_t Value = 0;
	std::copy_n(m_Blocks[m_Blocks.size() - 1].begin() + AccessBlockPos, sizeof(Value), (char*)&Value);
	return tAccess(Value);
}

std::optional<tKey> tSector::GetKeyB() const
{
	tAccess Access = GetAccess();
	if (!Access.good() || !Access.IsR_KeyB(m_KeyID))
		return {};
	return GetKey(KeyBBlockPos);
}

std::optional<tBlock> tSector::GetBlock(std::size_t blockIdx) const
{
	if (!good() || blockIdx >= m_Blocks.size())
		return {};
	return m_Blocks[blockIdx];
}

bool tSector::SetBlock(std::size_t blockIdx, const tBlock& block)
{
	if (blockIdx >= m_Blocks.size())
		return false;
	m_Blocks[blockIdx] = block;
	return true;
}

bool tSector::SetBlock(std::size_t blockIdx, const tBlockCRC& block)
{
	if (blockIdx >= m_Blocks.size()) // [TBD] Check CRC
		return false;
	std::copy_n(block.begin(), std::min(block.size(), m_Blocks[blockIdx].size()), m_Blocks[blockIdx].begin());
	return true;
}

std::string tSector::ToJSON() const
{
	std::stringstream SStr;
	SStr << "{";
	std::optional<tBlock> BlockSystem = GetBlockSystem();
	if (BlockSystem.has_value())
		SStr << "\"system\":\"" << ToStringHEX(*BlockSystem) << "\",";
	std::vector<std::uint8_t> Payload = GetPayload();
	SStr << "\"payload\":\"" << ToStringHEX(Payload) << '\"';
	std::optional<tBlock> BlockTrailer = GetBlockTrailer();
	if (BlockTrailer.has_value())
		SStr << ",\"trailer\":\"" << ToStringHEX(*BlockTrailer) << '\"';
	SStr << ",\"status\":\"" << to_string(m_Status) << '\"';
	SStr << '}';
	return SStr.str();
}

std::string tSector::ToString() const
{
	std::stringstream SStr;
	for (std::size_t i = 0; i < m_Blocks.size(); ++i)
	{
		std::optional<tBlock> Block = GetBlock(i);
		if (Block.has_value())
		{
			SStr << ToStringHEX(*Block, true);
			SStr << (!((i + 1) % 2) ? "\n" : "   ");
		}
	}

	auto KeyToSStr = [&SStr](const std::optional<tKey>& key)
	{
		if (!key.has_value())
		{
			SStr << "xx xx xx xx xx xx";
			return;
		}
		SStr << key.value().ToString();
	};
	SStr << " Key A: ";
	KeyToSStr(GetKeyA());
	SStr << "   ";
	SStr << " Key B: ";
	KeyToSStr(GetKeyB());
	SStr << "   ";
	tAccess Access = GetAccess();
	SStr << " Access: " << Access.ToString();
	SStr << "   ";
	SStr << " Status: " << to_string(m_Status) << '\n';
	return SStr.str();
}

std::vector<std::uint8_t> tSector::GetPayload() const
{
	if (!good())
		return {};
	std::vector<std::uint8_t> Payload;
	for (std::size_t i = 0; i < m_Blocks.size() - 1; ++i) // Block_3_Trailer is not a part of the payload
		Payload.insert(Payload.end(), m_Blocks[i].begin(), m_Blocks[i].end());
	return Payload;
}

tKey tSector::GetKey(int pos) const
{
	if (!good())
		return {};
	auto Begin = m_Blocks[m_Blocks.size() - 1].begin() + pos;
	return tKey(Begin, Begin + tKey::size());
}

std::optional<tNUID> tSector0::GetNUID() const
{
	std::optional<tBlock> Block = GetBlock(0);
	if (!Block.has_value())
		return {};
	tNUID NUID{};
	std::copy_n(Block->begin(), NUID.size(), NUID.begin());
	return NUID;
}

std::optional<tUID> tSector0::GetUID() const
{
	std::optional<tBlock> Block = GetBlock(0);
	if (!Block.has_value())
		return {};
	tUID UID{};
	std::copy_n(Block->begin(), UID.size(), UID.begin());
	return UID;
}

std::string tSector0::ToString() const
{
	std::stringstream SStr;
	SStr << tSector::ToString();
	SStr << " NUID: ";
	std::optional<tNUID> NUID = GetNUID();
	if (NUID.has_value())
		SStr << ToStringHEX(*NUID, true);
	SStr << "\n UID:  ";
	std::optional<tUID> UID = GetUID();
	if (UID.has_value())
		SStr << ToStringHEX(*UID, true);
	SStr << '\n';
	return SStr.str();
}

std::vector<std::uint8_t> tSector0::GetPayload() const
{
	if (!good())
		return {};
	std::vector<std::uint8_t> Payload;
	for (std::size_t i = 1; i < m_Blocks.size() - 1; ++i) // Block_0 and Block_3_Trailer are not parts of the payload
		Payload.insert(Payload.end(), m_Blocks[i].begin(), m_Blocks[i].end());
	return Payload;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace mad
{
std::uint8_t CRC8_MIFARE(const std::uint8_t* data, std::size_t dataSize)
{
	std::uint8_t Crc = 0xC7; // The documentation says the preset is 0xE3 but the bits have to be mirrored:	0xe3 = 1110 0011 <=> 1100 0111 = 0xc7
	constexpr std::uint8_t Poly = 0x1d; // x^8 + x^4 + x^3 + x^2 + 1 => 0x11d
	for (int i = 0; i < dataSize; ++i)
	{
		Crc ^= data[i];
		for (int i = 0; i < 8; ++i)
		{
			int Overrun = Crc & 0x80;
			Crc <<= 1;
			if (Overrun)
				Crc ^= Poly;
		}
	}
	return Crc;
}

std::optional<tMAD> tMAD::GetMAD(const tSector0& sector)
{
	std::vector<std::uint8_t> Payload = sector.GetPayload();
	if (Payload.size() != 32)
		return{};
	std::optional<tBlock> BlockTrailer = sector.GetBlock(3);
	if (!BlockTrailer.has_value())
		return {};
	tGPB GPB{};
	GPB.Value = BlockTrailer.value()[9];
	if (!GPB.Field.DA)
		return {};
	std::uint8_t CRC = CRC8_MIFARE(Payload.data() + 1, Payload.size() - 1);
	if (CRC != Payload[0])
		return {};
	tMAD MAD;
	MAD.m_InfoByte.Value = Payload[1];
	for (std::size_t i = 0, pldIdx = 2; i < MAD.m_AIDs.size() && pldIdx < Payload.size(); ++i)
	{
		MAD.m_AIDs[i].AppCode = static_cast<tAppCode>(Payload[pldIdx++]);
		MAD.m_AIDs[i].FCCode = static_cast<tFCCode>(Payload[pldIdx++]);
	}
	MAD.m_GPB = GPB;
	return MAD;
}

}
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
std::string CardToJSON(const T& card)
{
	std::stringstream SStr;
	SStr << "{";
	auto ID = card.GetID();
	SStr << "\"type\":\"" << to_string(card.GetType()) << "\",";
	if (!ID.empty())
		SStr << "\"id\":\"" << ToStringHEX(ID) << "\",";
	auto UID = card.GetUID();
	if (UID.has_value())
		SStr << "\"uid\":\"" << ToStringHEX(*UID) << "\",";
	auto NuID = card.GetNUID();
	if (NuID.has_value())
		SStr << "\"nuid\":\"" << ToStringHEX(*NuID) << "\",";
	SStr << "\"sectors\":[\n";
	for (std::size_t i = 0; i < card.GetSectorQty(); ++i)
	{
		tSector Sector = card.GetSector(i);
		if (i == 0)
		{
			SStr << static_cast<tSector0>(Sector).ToJSON();
		}
		else
		{
			SStr << Sector.ToJSON();
		}
		SStr << (i == card.GetSectorQty() - 1 ? "" : ",") << '\n';
	}
	SStr << "]\n}";
	return SStr.str();
}

template <class T>
std::string CardToString(const T& card)
{
	std::stringstream SStr;
	for (std::size_t i = 0; i < card.GetSectorQty(); ++i)
	{
		SStr << card.GetSector(i).ToString();
	}
	auto ID = card.GetID();
	if (!ID.empty())
		SStr << " ID:  " << ToStringHEX(ID, true) << '\n';
	return SStr.str();
}

std::string tCardClassicMini::ToJSON() const
{
	return CardToJSON(*this);
}

std::string tCardClassicMini::ToString() const
{
	return CardToString(*this);
}

std::string tCardClassic1K::ToJSON() const
{
	return CardToJSON(*this);
}

std::string tCardClassic1K::ToString() const
{
	return CardToString(*this);
}

std::string tCardClassic4K::ToJSON() const
{
	return CardToJSON(*this);
}

std::string tCardClassic4K::ToString() const
{
	return CardToString(*this);
}

std::optional<tUID> tCardUL::GetUID() const
{
	if (!good())
		return {};
	tUID UID{};
	tUID::iterator UIDBegin = UID.begin();
	for (std::size_t i = 0; UIDBegin != UID.end(); ++i)
	{
		if (i == 3 || i == 8) // UID check bytes
			continue;
		*UIDBegin++ = m_Blocks[0][i];
	}
	return UID;
}

tLock tCardUL::GetLock() const
{
	if (!good())
		return {};
	const auto LockIter = m_Blocks[0].begin() + LockPos;
	tLock Lock;
	std::copy(LockIter, LockIter + 2, reinterpret_cast<std::uint8_t*>(&Lock.Value));
	return Lock;
}

void tCardUL::SetLock(tLock lock)
{
	if (!good())
		return;
	const auto LockIter = m_Blocks[0].begin() + LockPos;
	std::copy((std::uint8_t*)&lock.Value, (std::uint8_t*)&lock.Value + sizeof(lock.Value), LockIter);
}

std::optional<tBlock> tCardUL::GetBlock(std::size_t blockIdx) const
{
	if (!good() || blockIdx >= m_Blocks.size())
		return {};
	return m_Blocks[blockIdx];
}

bool tCardUL::SetBlock(std::size_t blockIdx, const tBlock& block)
{
	if (blockIdx >= m_Blocks.size())
		return false;
	m_Blocks[blockIdx] = block;
	return true;
}

bool tCardUL::SetBlock(std::size_t blockIdx, const tBlockCRC& block)
{
	if (blockIdx >= m_Blocks.size()) // [TBD] Check CRC
		return false;
	std::copy_n(block.begin(), std::min(block.size(), m_Blocks[blockIdx].size()), m_Blocks[blockIdx].begin());
	return true;
}

std::vector<std::uint8_t> tCardUL::GetUserMemory() const
{
	if (!good())
		return {};
	std::vector<std::uint8_t> Payload;
	for (std::size_t i = 1; i < m_Blocks.size(); ++i) // Block_0_System is not a part of the payload
		Payload.insert(Payload.end(), m_Blocks[i].begin(), m_Blocks[i].end());
	return Payload;
}

void tCardUL::SetUserMemory(const std::vector<std::uint8_t>& data)
{
	if (!good() || data.size() != UserMemorySize)
		return;
	for (std::size_t blockIdx = 1, dataIdx = 0; blockIdx < m_Blocks.size() && dataIdx < data.size(); ++blockIdx)
	{
		std::size_t Size = std::min(data.size() - dataIdx, m_Blocks[blockIdx].size());
		std::copy_n(data.begin() + dataIdx, Size, m_Blocks[blockIdx].begin());
		dataIdx += Size;
	}
}

std::size_t tCardUL::GetUserMemoryUnlockedSize() const
{
	if (!good())
		return 0;
	std::size_t PageUnlocked = 0;
	tLock Lock = GetLock();
	for (int i = 4; i < 16; ++i) // 14 pages are for User Memory
	{
		if (IsPageAvailable(Lock, i))
			++PageUnlocked;
	}
	return PageUnlocked * PageSize;
}

std::vector<std::uint8_t> tCardUL::ReadUserMemoryUnlocked()
{
	std::size_t Size = GetUserMemoryUnlockedSize();
	if (!Size)
		return {};
	std::vector<std::uint8_t> Data(Size, 0);
	tLock Lock = GetLock();
	for (int i = 4, dataIdx = 0; i < 16 && dataIdx < Size; ++i) // 12 pages are for User Memory
	{
		if (!IsPageAvailable(Lock, i))
			continue;
		std::size_t BlockIdx = GetBlockIdxForPage(i);
		std::size_t BlockPos = (i - (BlockIdx * PageSize)) * PageSize;
		std::copy_n(m_Blocks[BlockIdx].begin() + BlockPos, PageSize, Data.begin() + dataIdx);
		dataIdx += PageSize;
	}
	return Data;
}

void tCardUL::WriteUserMemoryUnlocked(std::vector<std::uint8_t> data)
{
	std::size_t Size = GetUserMemoryUnlockedSize();
	if (!Size)
		return;
	data.resize(Size, 0);
	tLock Lock = GetLock();
	for (int i = 4, dataIdx = 0; i < 16 && dataIdx < Size; ++i) // 12 pages are for User Memory
	{
		if (!IsPageAvailable(Lock, i))
			continue;
		std::size_t BlockIdx = GetBlockIdxForPage(i);
		std::size_t BlockPos = (i - (BlockIdx * PageSize)) * PageSize;
		std::copy_n(data.begin() + dataIdx, PageSize, m_Blocks[BlockIdx].begin() + BlockPos);
		dataIdx += PageSize;
	}
}

std::string tCardUL::ToJSON() const
{
	std::stringstream SStr;
	SStr << "{";
	SStr << "\"type\":\"" << to_string(GetType()) << "\",";
	if (!m_ID.empty())
		SStr << "\"id\":\"" << ToStringHEX(m_ID) << "\",";
	std::optional<tUID> UID = GetUID();
	if (UID.has_value())
		SStr << "\"uid\":\"" << ToStringHEX(*UID) << "\",";
	SStr << "\"system\":\"" << ToStringHEX(m_Blocks[0]) << "\",";
	SStr << "\"payload\":\"" << ToStringHEX(GetUserMemory()) << "\",";
	SStr << "\"available\":\"" << std::dec << GetUserMemoryUnlockedSize() << "\",";
	SStr << "\"status\":\"" << to_string(m_Status) << '\"';
	SStr << '}';
	return SStr.str();
}

std::size_t tCardUL::GetBlockIdxForPage(int pageIdx)
{
	if (pageIdx < 4)
		return 0;
	if (pageIdx < 8)
		return 1;
	if (pageIdx < 12)
		return 2;
	if (pageIdx < 16)
		return 3;
	return 3; // [TBD] an exception can be thrown, this value is invalid
}

}
}
