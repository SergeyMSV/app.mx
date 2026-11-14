#pragma once

#include "state_receive.h"

namespace dev
{
namespace state
{

template <typename TPolicy>
class tContentSetMTK : public tContentSet<TPolicy>
{
	int m_ResetSequence = 0;

protected:
	tContentType HandleContentP(const typename TPolicy::tContentP& a_content) override
	{
		if constexpr (!std::is_same_v<typename TPolicy::tContentP, tContentNotSupported>)
		{
			if (a_content.Value.size() != 3)
				return tContentType::None;

			switch (m_ResetSequence)
			{
			case 0:
			case 2:
				if (a_content.Value[1] == "011" && a_content.Value[2] == "MTKGPS")
					++m_ResetSequence;
				break;
			case 1:
				if (a_content.Value[1] == "010" && a_content.Value[2] == "001")
					++m_ResetSequence;
				break;
			case 3:
				if (a_content.Value[1] == "010" && a_content.Value[2] == "002")
					++m_ResetSequence;
				break;
			}

			if (m_ResetSequence == 4)
				return tContentType::Restart;
		}
		return tContentType::None;
	}
};

}
}
