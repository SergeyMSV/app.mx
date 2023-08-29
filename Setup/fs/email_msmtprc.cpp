#include <devDataSetConfig.h>

#include <fstream>
#include <string>

void MakeFile_msmtprc(const std::string& a_path, const dev::config::tEmail& a_email)
{
	std::fstream File(a_path, std::ios::out);
	if (!File.good())
		return;

	File << "# Set default values for all following accounts.\n";
	File << "defaults\n";
	if (a_email.TLS)
	{
		File << "tls              on\n";
		File << "tls_trust_file   /etc/ssl/certs/ca-certificates.crt\n";
	}
	File << "logfile          ~/.msmtp.log\n";

	std::string Account1 = "account_" + a_email.User;

	File << "\n# Account 1\n";
	File << "account          " << Account1 << '\n';
	File << "host             " << a_email.Host << '\n';
	File << "port             " << a_email.Port << '\n';
	if (a_email.Auth)
		File << "auth             on\n";
	File << "from             " << a_email.From << '\n';
	File << "user             " << a_email.User << '\n';
	File << "password         " << a_email.Password << '\n';

	File << "\n# Set a default account\n";
	File << "account default : " << Account1 << '\n';

	File.close();
}

/*
.msmtprc
 
# Set default values for all following accounts.
defaults
tls            on
tls_trust_file /etc/ssl/certs/ca-certificates.crt
logfile        ~/.msmtp.log

# test3@example.org
account        test3_example
host           smtp.example.org
port           587
auth           on
from           test3@example.org
user           test3
password       ...

# test4@gmail.com
account        test4_gmail
host           smtp.gmail.com
port           587
auth           on
from           test4@gmail.com
user           test4
password       ...

# Set a default account
#account default: test3_example
account default: test4_gmail
*/
