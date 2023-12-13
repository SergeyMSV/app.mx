#include "utilsCardMIFARE.h"

#include <algorithm>
#include <iomanip>

namespace utils
{
namespace card_MIFARE
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

namespace classic
{

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

bool tAccess::IsR_Data(tKeyID keyID, tBlockID blockID) const
{
	if (blockID == tBlockID::Block_3_Trailer)
		return false;
	auto Check = [&keyID](tAccessBlock a)
	{
		bool AccKeyA = keyID == tKeyID::A && (!a.C3 || !a.C1 && !a.C2 && a.C3);
		bool AccKeyB = keyID == tKeyID::B && (!a.C3 || a.C1 != a.C2 && a.C3 || !a.C1 && !a.C2 && a.C3);
		return AccKeyA || AccKeyB;
	};
	return Check(GetAccessBlock(blockID));
}

bool tAccess::IsW_Data(tKeyID keyID, tBlockID blockID) const
{
	if (blockID == tBlockID::Block_3_Trailer)
		return false;
	auto Check = [&keyID](tAccessBlock a)
	{
		bool AccKeyA = keyID == tKeyID::A && !a.C1 && !a.C2 && !a.C3;
		bool AccKeyB = keyID == tKeyID::B && (!a.C1 && a.C2 == a.C3 || a.C1 && !a.C3);
		return AccKeyA || AccKeyB;
	};
	return Check(GetAccessBlock(blockID));
}

bool tAccess::IsIncr_Data(tKeyID keyID, tBlockID blockID) const
{
	if (blockID == tBlockID::Block_3_Trailer)
		return false;
	auto Check = [&keyID](tAccessBlock a)
	{
		bool AccKeyA = keyID == tKeyID::A && !a.C1 && !a.C2 && !a.C3;
		bool AccKeyB = keyID == tKeyID::B && !a.C3 && a.C1 == a.C2;
		return AccKeyA || AccKeyB;
	};
	return Check(GetAccessBlock(blockID));
}

bool tAccess::IsDecr_Data(tKeyID keyID, tBlockID blockID) const
{
	if (blockID == tBlockID::Block_3_Trailer)
		return false;
	auto Check = [&keyID](tAccessBlock a)
	{
		return !a.C3 && a.C1 == a.C2 || !a.C1 && !a.C2 && a.C3;
	};
	return Check(GetAccessBlock(blockID));
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

tAccess::tAccessBlock tAccess::GetAccessBlock(tBlockID blockID) const
{
	switch (blockID)
	{
	case tBlockID::Block_0: return { !!m_Access.Field.C1_0, !!m_Access.Field.C2_0, !!m_Access.Field.C3_0 };
	case tBlockID::Block_1: return { !!m_Access.Field.C1_1, !!m_Access.Field.C2_1, !!m_Access.Field.C3_1 };
	case tBlockID::Block_2: return { !!m_Access.Field.C1_2, !!m_Access.Field.C2_2, !!m_Access.Field.C3_2 };
	case tBlockID::Block_3_Trailer: return { !!m_Access.Field.C1_3, !!m_Access.Field.C2_3, !!m_Access.Field.C3_3 };
	}
	return {};
}

tKey::tKey(std::uint64_t value) :m_Value(value)
{
	if (m_Value > ValueMAX)
		m_Value = ValueMAX;
}

tKey::tKey(std::vector<std::uint8_t>::const_iterator itBegin, std::vector<std::uint8_t>::const_iterator itEnd)
{
	if (std::distance(itBegin, itEnd) != KeySize)
		m_Value = ValueMAX;
	m_Value = 0;
	std::copy_n(itBegin, std::min(KeySize, sizeof(m_Value)), (char*)&m_Value);
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
	ToStringHEX(std::vector<std::uint8_t>(ValueArray), true);
	return SStr.str();
}

tSector::tSector(tKeyID keyID, const tKey& key, const std::vector<std::uint8_t>& sector)
	:m_KeyID(keyID), m_Key(key)
{
	push(sector);
}

void tSector::push(std::vector<std::uint8_t> sector)
{
	sector.resize(SectorSize);
	m_Payload = sector;
}

void tSector::push_back_block(std::vector<std::uint8_t> block)
{
	block.resize(BlockSize);
	m_Payload.insert(m_Payload.end(), block.begin(), block.end());
}

void tSector::push_back_block(tBlock block)
{
	m_Payload.insert(m_Payload.end(), block.begin(), block.end());
}

tAccess tSector::GetAccess() const
{
	if (m_Payload.size() != SectorSize)
		return {};
	std::uint32_t Value = 0;
	std::copy_n(m_Payload.begin() + AccessPos, sizeof(Value), (char*)&Value);
	return tAccess(Value);
}

std::optional<tKey> tSector::GetKeyB() const
{
	tAccess Access = GetAccess();
	if (!Access.good() || !Access.IsR_KeyB(m_KeyID))
		return {};
	return GetKey(KeyBPos);
}

std::optional<tBlock> tSector::GetBlock(tBlockID blockID) const
{
	if (!good())
		return {};
	std::size_t BlockPos = static_cast<std::size_t>(blockID) * BlockSize;
	tBlock Block{};
	std::copy_n(m_Payload.begin() + BlockPos, Block.size(), Block.begin());
	return Block;
}

std::string tSector::ToJSON() const
{
	std::stringstream SStr;
	SStr << "{";
	SStr << "\"payload\":\"" << ToStringHEX(m_Payload) << '\"';
	if (!m_Status.empty())
		SStr << ",\"status\":\"" << m_Status << '\"';
	SStr << '}';
	return SStr.str();
}

std::string tSector::ToString() const
{
	std::stringstream SStr;
	auto BlockToSStr = [&](tBlockID blockID)
	{
		std::optional<tBlock> Block = GetBlock(blockID);
		if (!Block.has_value())
			return;
		SStr << ToStringHEX(*Block, true);
	};

	BlockToSStr(tBlockID::Block_0);
	SStr << "   ";
	BlockToSStr(tBlockID::Block_1);
	SStr << '\n';
	BlockToSStr(tBlockID::Block_2);
	SStr << "   ";
	BlockToSStr(tBlockID::Block_3_Trailer);
	SStr << '\n';

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
	SStr << '\n';
	SStr << " Key B: ";
	KeyToSStr(GetKeyB());
	SStr << '\n';
	tAccess Access = GetAccess();
	SStr << " Access: " << Access.ToString() << '\n';
	SStr << " Status: " << m_Status << '\n';
	return SStr.str();
}

tKey tSector::GetKey(int pos) const
{
	if (!good())
		return {};
	auto Begin = m_Payload.begin() + pos;
	return tKey(Begin, Begin + tKey::size());
}

std::optional<tNUID> tSector0::GetNUID() const
{
	std::optional<tBlock> Block = GetBlock(tBlockID::Block_0);
	if (!Block.has_value())
		return {};
	tNUID NUID{};
	std::copy_n(Block->begin(), NUID.size(), NUID.begin());
	return NUID;
}

std::optional<tUID> tSector0::GetUID() const
{
	std::optional<tBlock> Block = GetBlock(tBlockID::Block_0);
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
		SStr << card.GetSector(i).ToJSON() << (i == card.GetSectorQty() - 1 ? "" : ",") << '\n';
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
		SStr << card.GetSector(i).ToString() << '\n';
	}
	auto ID = card.GetID();
	if (!ID.empty())
		SStr << "\n ID:  " << ToStringHEX(ID, true) << '\n';
	return SStr.str();
}

std::string tCardMini::ToJSON() const
{
	return CardToJSON(*this);
}

std::string tCardMini::ToString() const
{
	return CardToString(*this);
}

std::string tCard1K::ToJSON() const
{
	return CardToJSON(*this);
}

std::string tCard1K::ToString() const
{
	return CardToString(*this);
}

std::string tCard4K::ToJSON() const
{
	return CardToJSON(*this);
}

std::string tCard4K::ToString() const
{
	return CardToString(*this);
}

}

namespace ultralight
{

void tCard::push(std::vector<std::uint8_t> sector)
{
	sector.resize(SectorSize);
	m_Payload = sector;
}

void tCard::push_back_block(std::vector<std::uint8_t> block)
{
	block.resize(BlockSize);
	m_Payload.insert(m_Payload.end(), block.begin(), block.end());
}

void tCard::push_back_block(tBlock block)
{
	m_Payload.insert(m_Payload.end(), block.begin(), block.end());
}

std::optional<tUID> tCard::GetUID() const
{
	if (!good())
		return {};
	tUID UID{};
	tUID::iterator UIDBegin = UID.begin();
	for (std::size_t i = 0; UIDBegin != UID.end(); ++i)
	{
		if (i == 3 || i == 8) // UID check bytes
			continue;
		*UIDBegin++ = m_Payload[i];
	}
	return UID;
}

tLock tCard::GetLock() const
{
	if (!good())
		return {};
	const auto LockIter = m_Payload.begin() + LockPos;
	tLock Lock;
	std::copy(LockIter, LockIter + 2, reinterpret_cast<std::uint8_t*>(&Lock.Value));
	return Lock;
}

void tCard::SetLock(tLock lock)
{
	if (!good())
		return;
	const auto LockIter = m_Payload.begin() + LockPos;
	std::copy((std::uint8_t*)&lock.Value, (std::uint8_t*)&lock.Value + sizeof(lock.Value), LockIter);
}

std::optional<tBlock> tCard::GetBlock(tBlockID blockID) const
{
	if (!good())
		return {};
	std::size_t BlockPos = static_cast<std::size_t>(blockID) * BlockSize;
	tBlock Block{};
	std::copy_n(m_Payload.begin() + BlockPos, Block.size(), Block.begin());
	return Block;
}

std::vector<std::uint8_t> tCard::GetUserMemory() const
{
	if (!good())
		return {};
	return std::vector<std::uint8_t>(m_Payload.begin() + BlockSize, m_Payload.end());
}

void tCard::SetUserMemory(const std::vector<std::uint8_t>& data)
{
	if (!good() || data.size() != UserMemorySize)
		return;
	m_Payload.resize(SystemMemorySize);
	m_Payload.insert(m_Payload.end(), data.begin(), data.end());
}

std::size_t tCard::GetUserMemoryUnlockedSize() const
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

std::vector<std::uint8_t> tCard::ReadUserMemoryUnlocked()
{
	std::size_t Size = GetUserMemoryUnlockedSize();
	if (!Size)
		return {};
	std::vector<std::uint8_t> Data(Size, 0);
	tLock Lock = GetLock();
	for (int i = 0, dataIndex = 0; i < 16 && dataIndex < Size; ++i) // 14 pages are for User Memory
	{
		if (!IsPageAvailable(Lock, i))
			continue;

		for (int j = 0; j < PageSize; ++j)
			Data[dataIndex++] = m_Payload[i * PageSize + j];
	}
	return Data;
}

void tCard::WriteUserMemoryUnlocked(std::vector<std::uint8_t> data)
{
	std::size_t Size = GetUserMemoryUnlockedSize();
	if (!Size)
		return;
	data.resize(Size, 0);
	tLock Lock = GetLock();
	for (int i = 0, dataIndex = 0; i < 16 && dataIndex < Size; ++i) // 14 pages are for User Memory
	{
		if (!IsPageAvailable(Lock, i))
			continue;

		for (int j = 0; j < PageSize; ++j)
			m_Payload[i * PageSize + j] = data[dataIndex++];
	}
}

std::string tCard::ToJSON() const
{
	std::stringstream SStr;
	SStr << "{";
	SStr << "\"type\":\"" << to_string(GetType()) << "\",";
	if (!m_ID.empty())
		SStr << "\"id\":\"" << ToStringHEX(m_ID) << "\",";
	std::optional<tUID> UID = GetUID();
	if (UID.has_value())
		SStr << "\"uid\":\"" << ToStringHEX(*UID) << "\",";
	SStr << "\"payload\":\"" << ToStringHEX(m_Payload) << "\",";
	SStr << "\"available\":\"" << std::dec << GetUserMemoryUnlockedSize() << "\"";
	if (!m_Status.empty())
		SStr << ",\"status\":\"" << m_Status << '\"';
	SStr << '}';
	return SStr.str();
}

}
}
}
