///////////////////////////////////////////////////////////////////////////////////////////////////
// devGNSS
// 2020-03-03
// C++11
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
/*
#include <devConfig.h>
#include <devDataSet.h>
#include <devLog.h>

#include <modGnssReceiver.h>

#include <utilsPortSerial.h>

#include <future>

#include <boost/asio.hpp>

namespace dev
{

class tGNSS
{
///////////////////////////////////////////////////////////////////////////////////////////////////
	class tModGnssReceiver : public mod::tGnssReceiver
	{
///////////////////////////////////////////////////////////////////////////////////////////////////
		using tPortUARTBase = utils::port::serial::tPortSerialAsync<>;
		class tBoard : public tPortUARTBase
		{
			tModGnssReceiver* m_pObj = nullptr;

		public:
			tBoard(tModGnssReceiver* obj, boost::asio::io_context& io);
			virtual ~tBoard();

		protected:
			void OnReceived(const std::vector<std::uint8_t>& data) override;
		};
///////////////////////////////////////////////////////////////////////////////////////////////////
		tGNSS* m_pObj = nullptr;

		tBoard m_Board;

	public:
		explicit tModGnssReceiver(tGNSS* obj);
		virtual ~tModGnssReceiver();

	protected:
		mod::tGnssTaskScript GetTaskScript(const std::string& id, bool userTaskScript) override;
		mod::tGnssSettingsNMEA GetSettingsNMEA() override;

		void OnChanged(const mod::tGnssDataSet& value) override;

		void Board_PowerSupply(bool state) override;
		void Board_Reset(bool state) override;

		bool Board_Send(const utils::tVectorUInt8& data) override;
		void OnReceived(const std::vector<std::uint8_t>& data);
	};
///////////////////////////////////////////////////////////////////////////////////////////////////
	friend class tModGnssReceiver;

	utils::log::tLog& m_Log;

	boost::asio::io_context& m_ioc;

	tModGnssReceiver *m_pMod = nullptr;

	bool m_StartAuto = true;

public:
	tGNSS() = delete;
	tGNSS(utils::log::tLog& log, boost::asio::io_context& ioc);
	tGNSS(const tGNSS&) = delete;
	tGNSS(tGNSS&&) = delete;
	~tGNSS();

	tGNSS& operator=(const tGNSS&) = delete;
	tGNSS& operator=(tGNSS&&) = delete;
	void operator()();

	void Start();
	void Restart();
	void Halt();
	void Exit();

	bool StartUserTaskScript(const std::string& taskScriptID);

	utils::tDevStatus GetStatus() const;
	std::string GetLastErrorMsg() const;

	//tModGnssReceiver* operator->()//[TEST]
	//{
	//	return m_pMod;
	//}
};

}*/
