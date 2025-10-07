#include "modGnssReceiver.h"

namespace mod
{

tGnssReceiver::tStateError::tStateError(tGnssReceiver* obj, const std::string& value)
	:tState(obj, "StateError")
{
	m_pObj->m_LastErrorMsg = "tStateError: " + value;
	m_pObj->LogStateStarted(m_pObj->m_LastErrorMsg);

	m_pObj->m_Control_Operation = false;
}

bool tGnssReceiver::tStateError::OnCmdFailed()
{
	return tState::OnCmdDone();
}

void tGnssReceiver::tStateError::OnTaskScriptDone()
{
	m_pObj->LogTaskScriptDone();

	ChangeState(new tStateHalt(m_pObj, "error", true));
	return;
}

void tGnssReceiver::tStateError::OnTaskScriptFailed(const std::string& msg)
{
	m_pObj->LogTaskScriptFailed(msg);

	ChangeState(new tStateHalt(m_pObj, "error", true));
	return;
}

}
