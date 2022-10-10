#include "devDataSetConfig.h"

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

namespace config
{

tEmail::tEmail(boost::property_tree::ptree pTree)
{
	To = pTree.get<std::string>("email.to");
	Period = pTree.get<uint32_t>("email.period");
}

bool tEmail::IsWrong() const
{
	return To.empty() || Period == 0; //[TBD] check if email is correct
}

tGNSS::tGNSS(boost::property_tree::ptree pTree)
{
	Path = pTree.get<std::string>("gnss.path");
	Prefix = pTree.get<std::string>("gnss.prefix");
	QtyMax = pTree.get<uint8_t>("gnss.qtyMax");
}

bool tGNSS::IsWrong() const
{
	return Path.empty() || Prefix.empty() || QtyMax == 0;;
}

tPicture::tPicture(boost::property_tree::ptree pTree)
{
	Path = pTree.get<std::string>("picture.path");
	Prefix = pTree.get<std::string>("picture.prefix");
	QtyMax = pTree.get<uint8_t>("picture.qtyMax");
}

bool tPicture::IsWrong() const
{
	return Path.empty() || Prefix.empty() || QtyMax == 0;
}

}

tDataSetConfig::tDataSetConfig(const std::string& fileName)
{
	boost::property_tree::ptree PTree;
	boost::property_tree::json_parser::read_json(fileName, PTree);

	m_Email = config::tEmail(PTree);
	m_GNSS = config::tGNSS(PTree);
	m_Picture = config::tPicture(PTree);
}

}
