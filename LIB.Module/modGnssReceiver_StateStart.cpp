#include "modGnssReceiver.h"

namespace mod
{

tGnssReceiver::tStateStart::tStateStart(tGnssReceiver* obj, const std::string& value)
	:tState(obj, "StateStart")
{
	m_pObj->LogStateStarted("tStateStart: " + value);

	if (m_pObj->IsControlRestart())
		m_pObj->m_Control_Restart = false;
}

void tGnssReceiver::tStateStart::OnTaskScriptDone()
{
	m_pObj->LogTaskScriptDone();

	if (m_NextState_Stop)
	{
		ChangeState(new tStateStop(m_pObj, "start single"));
	}
	else
	{
		ChangeState(new tStateOperation(m_pObj));
	}
}

void tGnssReceiver::tStateStart::OnTaskScriptFailed(const std::string& msg)
{
	m_pObj->LogTaskScriptFailed(msg);

	ChangeState(new tStateError(m_pObj, "start"));
}

}