#include <utilsBase.h>

#include <devDataSetConfig.h>

#include <fstream>
#include <iostream>

namespace dev
{

void MakeFile_muttrc(const std::string& a_path, const std::string& a_hostname, const config::tEmail& a_email)
{
	std::fstream File(a_path, std::ios::out);
	if (File.good())
	{
		File << "set sendmail=\"/usr/bin/msmtp\"\n";
		File << "set use_from=yes\n";
		File << "set realname=" << a_hostname << '\n';
		File << "set from=" << a_email.From<<'\n';
		File << "set envelope_from=yes\n";
		File << "set copy=no\n";//doesn't save copies of letter into file ~/sent
		File << "set content_type=text/html\n";//default is "text/plain"

		File.close();
	}
}

}
