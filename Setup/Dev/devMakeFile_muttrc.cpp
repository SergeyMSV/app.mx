#include <utilsBase.h>

#include <devDataSet.h>

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

		File.close();
	}
}

}

/*
.muttrc

set sendmail="/usr/bin/msmtp"
set use_from=yes
set realname="mx6bull"
set from=navidevices@gmail.com
#set from=test3287@yandex.ru
set envelope_from=yes
*/
