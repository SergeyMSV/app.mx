#include <devDataSetConfig.h>

#include <fstream>
#include <string>

void MakeFile_muttrc(const std::string& a_path, const std::string& a_hostname, const dev::config::tEmail& a_email)
{
	std::fstream File(a_path, std::ios::out);
	if (!File.good())
		return;

	File << "set sendmail=\"/usr/bin/msmtp\"\n";
	File << "set use_from=yes\n";
	File << "set realname=" << a_hostname << '\n';
	File << "set from=" << a_email.From << '\n';
	File << "set envelope_from=yes\n";
	File << "set copy=no\n";//copies of letters are not saved in file ~/sent
	File << "set content_type=text/html\n";//default is "text/plain"

	File.close();
}
