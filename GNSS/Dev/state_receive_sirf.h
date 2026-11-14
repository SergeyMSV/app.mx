#pragma once

#include "state_receive.h"

namespace dev
{
namespace state
{

template <typename TPolicy>
class tContentSetSiRF : public tContentSet<TPolicy>
{
	int m_ResetSequence = 0;

protected:
	tContentType HandleContentP(const typename TPolicy::tContentP& a_content) override
	{
		if constexpr (!std::is_same_v<typename TPolicy::tContentP, tContentNotSupported>)
		{
			//if (a_content.Value.size() != 3)
			//	return tContentType::None;
#ifdef GNSS_TEST
			std::cout << a_content.Value.size() << '\n';
#endif // GNSS_TEST
			// These reset sequence of messages can't be detected because of absence of CRC. The driver can't find them in the input stream of data.

			/*
			MakeMsgNoCRC("PSRFTXT,NMEA: ID 101 Ack Input"));
            MakeMsgNoCRC("PSRFTXT,GSU-7x : Position Co.,Ltd.2009"));
            MakeMsgNoCRC("PSRFTXT,Firmware Checksum: 1ac8 "));
            MakeMsgNoCRC("PSRFTXT,TOW:  0"));
            MakeMsgNoCRC("PSRFTXT,WK:   1234"));
            MakeMsgNoCRC("PSRFTXT,POS:  1234567 0 0"));
            MakeMsgNoCRC("PSRFTXT,CLK:  12345"));
            MakeMsgNoCRC("PSRFTXT,CHNL: 12"));
            MakeMsgNoCRC("PSRFTXT,Baud rate: " + baudrate.ToString()));
			*/

			/*switch (m_ResetSequence)
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
			}*/

			if (m_ResetSequence == 4)
				return tContentType::Restart;
		}
		return tContentType::None;
	}
};

}
}
