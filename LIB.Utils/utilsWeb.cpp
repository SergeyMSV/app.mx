#include "utilsWeb.h"

#include <libConfig.h>

#include "utilsException.h"

#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

//#include <libs/beast/example/common/root_certificates.hpp>

namespace utils
{
	namespace web
	{

struct tURI
{
	std::string Protocol;
	std::string Host;
	std::string Target;

	tURI() = default;
	tURI(const std::string& uri)
	{
		if (uri.empty())
			return;

		size_t ProtocolSize = uri.find("://");
		if (ProtocolSize == std::string::npos)
			return;
		
		Protocol = uri.substr(0, ProtocolSize);
		if (Protocol != "https")
			return;//other protocols are not tested

		size_t HostPos = ProtocolSize + 3;// + size of "://"
		size_t HostEnd = uri.find("/", HostPos);
		if (HostEnd == std::string::npos)
			return;

		if (HostEnd < HostPos)
			return;

		Host = uri.substr(HostPos, HostEnd - HostPos);

		auto TargetEnd = std::find_if(uri.begin() + HostEnd, uri.end(), [](char ch) { return std::isspace(ch); });
		Target = std::string(uri.begin() + HostEnd, TargetEnd);
	}

	bool IsWrong()
	{
		return Host.empty();
	}
};

template<class T>
static T HttpGET(boost::beast::ssl_stream<boost::beast::tcp_stream>& a_TcpStream, int a_RemoteVersion, const std::string& a_host, const std::string& a_target, int& a_recursion)
{
	if (a_recursion == 0)
		return {};
	// Set up an HTTP GET request message
	boost::beast::http::request<boost::beast::http::string_body> req{ boost::beast::http::verb::get, a_target, a_RemoteVersion };
	req.set(boost::beast::http::field::host, a_host);
	req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

	boost::beast::http::write(a_TcpStream, req);//Send the HTTP request to the remote host

	boost::beast::flat_buffer buffer;//This buffer is used for reading and must be persisted

	boost::beast::http::response<boost::beast::http::dynamic_body> res;//Declare a container to hold the response

	boost::beast::http::read(a_TcpStream, buffer, res);//Receive the HTTP response

	switch (static_cast<boost::beast::http::status>(res.result_int()))
	{
	case boost::beast::http::status::ok:
	{
		const auto resData = res.body().data();
		return T(boost::asio::buffers_begin(resData), boost::asio::buffers_end(resData));
	}
	case boost::beast::http::status::found:
	case boost::beast::http::status::see_other:
	{
		boost::beast::string_view Location = res.at("location");
		if (Location.empty())
			break;

		tURI URI(Location.data());

		if (URI.IsWrong())
			break;

		return HttpGET<T>(a_TcpStream, a_RemoteVersion, URI.Host, URI.Target, --a_recursion);
	}
	}

	return {};
}

template<class T>
std::optional<T> HttpsReqSync(const std::string& a_host, const std::string& a_target)
{
	const std::string RemotePort = "443";
	const int RemoteVersion = 11;//"1.0" -> 10; "1.1" -> 11

	T DataReceived{};

	try
	{
		boost::asio::io_context ioc;

		boost::asio::ssl::context ctx{ boost::asio::ssl::context::tlsv12_client };
		ctx.set_verify_mode(boost::asio::ssl::verify_none);//It's nothing to verify actually this sw sends nothing to the site.

		boost::asio::ip::tcp::resolver Resolver(ioc);
		boost::beast::ssl_stream<boost::beast::tcp_stream> TcpStream(ioc, ctx);

		//Set SNI Hostname (many hosts need this to handshake successfully)
		if (!SSL_set_tlsext_host_name(TcpStream.native_handle(), a_host.c_str()))
		{
			boost::beast::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
			throw boost::beast::system_error{ ec };
		}

		boost::system::error_code cerrResolve;
		const auto results = Resolver.resolve(a_host, RemotePort, cerrResolve);//Look up the domain name
		
		boost::beast::get_lowest_layer(TcpStream).connect(results);//Make the connection on the IP address we get from a lookup

		TcpStream.handshake(boost::asio::ssl::stream_base::client);//Perform the SSL handshake

		int PossibleRecursion = LIB_UTILS_WEB_FORWARDING_ADDRESSES;//[#] - not more than so many forwarding addresses
		DataReceived = HttpGET<T>(TcpStream, RemoteVersion, a_host, a_target, PossibleRecursion);

		boost::beast::error_code CerrBeast;
		TcpStream.shutdown(CerrBeast);
		if (CerrBeast == boost::asio::error::eof)
		{
			// Rationale:
			// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
			CerrBeast = {};
		}

		if (CerrBeast == boost::asio::ssl::error::stream_truncated)
		{
			// Rationale:
			// https://github.com/boostorg/beast/issues/824
			CerrBeast = {};
		}

		if (CerrBeast)
		{
			throw boost::beast::system_error{ CerrBeast };
		}
	}
	catch (std::exception const& e)
	{
		std::cerr << utils::log_ex::GetLogMessage(e.what(), __FILE__, __LINE__) << '\n';
		return {};
	}

	return DataReceived;
}

std::optional<std::string> HttpsReqSyncString(const std::string& a_host, const std::string& a_target)
{
	return HttpsReqSync<std::string>(a_host, a_target);
}

std::optional<tVectorUInt8> HttpsReqSyncVector(const std::string& a_host, const std::string& a_target)
{
	return HttpsReqSync<tVectorUInt8>(a_host, a_target);
}

}
}