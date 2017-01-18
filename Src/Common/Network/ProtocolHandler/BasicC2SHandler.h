/**
Name:   BasicC2SProtocolHandler.h
Author:  jjuiddong
Date:    3/17/2013

basic_protocol.prt Protocol Handler
*/
#pragma once

#include "NetProtocol/Src/basic_ProtocolListener.h"
#include "NetProtocol/Src/basic_Protocol.h"

namespace network
{
	class CServer;
	class CBasicC2SHandler : public basic::c2s_ProtocolListener
	{
	public:
		CBasicC2SHandler( CServerBasic &svr );
		virtual ~CBasicC2SHandler();

		CServerBasic& GetServer();
		basic::s2c_Protocol& GetBasicProtocol();

	protected:
		// Network Protocol Handler
		virtual bool ReqLogIn(basic::ReqLogIn_Packet &packet) override;
		virtual bool ReqLogOut(basic::ReqLogOut_Packet &packet) override;
		virtual bool ReqGroupList(basic::ReqGroupList_Packet &packet) override;
		virtual bool ReqGroupJoin(basic::ReqGroupJoin_Packet &packet) override;
		virtual bool ReqGroupCreate(basic::ReqGroupCreate_Packet &packet) override;
		virtual bool ReqGroupCreateBlank(basic::ReqGroupCreateBlank_Packet &packet) override;
		virtual bool ReqP2PConnect(basic::ReqP2PConnect_Packet &packet) override;


	protected:
		bool			CreateBlankGroup( netid senderId, const netid &parentGroupId, const std::string &groupName, 
			OUT GroupPtr &pParent, OUT GroupPtr &pFrom, OUT GroupPtr &pNew );

	private:
		CServerBasic				&m_Server;		/// CServer Reference 
		basic::s2c_Protocol		m_BasicProtocol;
	};


	inline CServerBasic& CBasicC2SHandler::GetServer() { return m_Server; }
	inline basic::s2c_Protocol& CBasicC2SHandler::GetBasicProtocol() { return m_BasicProtocol; }

}
