#include "devGNSS.h"

namespace dev
{

tGNSS::tGNSS(utils::tLog& log, boost::asio::io_context& ioc)
	:m_Log(log), m_ioc(ioc)
{
	m_pMod = new tModGnssReceiver(this);
}

tGNSS::~tGNSS()
{
	delete m_pMod;
}

void tGNSS::operator()()
{
	if (m_StartAuto)
	{
		m_StartAuto = false;

		m_pMod->Start(true);
	}

	(*m_pMod)();
}

void tGNSS::Start()
{
	m_pMod->Start();
}

void tGNSS::Restart()
{
	m_pMod->Restart();
}

void tGNSS::Halt()
{
	m_pMod->Halt();
}

void tGNSS::Exit()
{
	m_pMod->Exit();
}

bool tGNSS::StartUserTaskScript(const std::string& taskScriptID)
{
	return m_pMod->StartUserTaskScript(taskScriptID);
}

utils::tDevStatus tGNSS::GetStatus() const
{
	if (!m_pMod)
		return utils::tDevStatus::Unknown;
	return m_pMod->GetStatus();
}

std::string tGNSS::GetLastErrorMsg() const
{
	if (!m_pMod)
		return {};
	return m_pMod->GetLastErrorMsg();
}

}
