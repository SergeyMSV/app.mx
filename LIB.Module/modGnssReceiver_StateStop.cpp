#include "modGnssReceiver.h"

namespace mod
{

tGnssReceiver::tStateStop::tStateStop(tGnssReceiver* obj, const std::string& value)
	:tState(obj, "StateStop")
{
	m_pObj->LogStateStarted("tStateStop: " + value);
}

void tGnssReceiver::tStateStop::OnTaskScriptDone()
{
	m_pObj->LogTaskScriptDone();

	ChangeState(new tStateHalt(m_pObj, "stop"));
	return;
}

void tGnssReceiver::tStateStop::OnTaskScriptFailed(const std::string& msg)
{
	m_pObj->LogTaskScriptFailed(msg);

	ChangeState(new tStateError(m_pObj, "stop"));
	return;
}

}