#include "devGNSS.h"
#include "devDataSet.h"
#include "devDataSetConfig.h"

#include <utilsFile.h>
#include <utilsPath.h>

#include <shareUtilsFile.h>

#include <cstdio>
#include <fstream>

namespace dev
{

tGNSS::tModGnssReceiver::tModGnssReceiver(tGNSS* obj)
	:mod::tGnssReceiver(obj->m_pLog), m_pObj(obj), m_Board(this, *obj->m_pIO)
{

}

tGNSS::tModGnssReceiver::~tModGnssReceiver()
{

}

mod::tGnssTaskScript tGNSS::tModGnssReceiver::GetTaskScript(const std::string& id, bool userTaskScript)
{
	return g_Settings.GetTaskScript(id);
}

mod::tGnssSettingsNMEA tGNSS::tModGnssReceiver::GetSettingsNMEA()
{
	return g_Settings.GetSettingsNMEA();
}

void tGNSS::tModGnssReceiver::OnChanged(const mod::tGnssDataSet& value)
{
	config::tOutGNSS OutGNSS = g_Settings.GetOutGNSS();

	std::string DTStr = utils::GetDateTime();
	std::string Path = OutGNSS.Path + "/";
	std::string FileName = OutGNSS.Prefix + DTStr + ".json";
	std::string FileNameFull = Path + FileName;
	std::string FileNameTemp = Path + g_FileNameTempPrefix + FileName + ".tmp";
	std::fstream File = std::fstream(FileNameTemp, std::ios::out);
	if (!File.good())
	{
		m_pObj->m_pLog->WriteLine(true, utils::tLogColour::LightYellow, "Open File Error: " + FileNameTemp);
		m_pObj->m_pLog->WriteLine(true, utils::tLogColour::LightYellow, value.ToJSON());
		return;
	}

	File << value.ToJSON();
	File.close();
	
	std::rename(FileNameTemp.c_str(), FileNameFull.c_str());

	share::RemoveFilesOutdated(OutGNSS);

	//m_pObj->m_pLog->WriteLine(true, utils::tLogColour::LightYellow, value.ToJSON());
}

void tGNSS::tModGnssReceiver::Board_PowerSupply(bool state)
{
	std::stringstream SStream;
	SStream << "Board_PowerSupply: " << state;
	m_pObj->m_pLog->WriteLine(true, utils::tLogColour::LightMagenta, SStream.str());
}

void tGNSS::tModGnssReceiver::Board_Reset(bool state)
{
	std::stringstream SStream;
	SStream << "Board_Reset:       " << state;
	m_pObj->m_pLog->WriteLine(true, utils::tLogColour::LightMagenta, SStream.str());
}

bool tGNSS::tModGnssReceiver::Board_Send(const utils::tVectorUInt8& data)
{
	return m_Board.Send(data);
}

void tGNSS::tModGnssReceiver::OnReceived(utils::tVectorUInt8& data)
{
	Board_OnReceived(data);
}

}
