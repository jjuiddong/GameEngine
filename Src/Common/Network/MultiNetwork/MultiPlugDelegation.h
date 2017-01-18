/**
Name:   NetGroupDelegation.h
Author:  jjuiddong
Date:    4/1/2013

network::multinetwork::CNetGroupDelegation 구현
- Network Group 에 접속하는 Controller를 통해 패킷을 전달받고, 이벤트를 핸들링한다.
*/
#pragma once

namespace network { namespace multinetwork {

	/// Network Group 에 접속하는 Controller를 통해 패킷을 전달받고, 이벤트를 핸들링한다.
	class CMultiPlugDelegation : public CPlug //public CPlugLinker
	{
	public:
		CMultiPlugDelegation();
		virtual ~CMultiPlugDelegation();

		virtual bool	Send(netid netId, const SEND_FLAG flag, CPacket &packet) override;
		virtual bool	SendAll(CPacket &packet) override;

		void					SetMultiPlug(MultiPlugPtr ptr);
		MultiPlugPtr		GetMultiPlug() const;

		CServerBasic* GetServer();
		const CoreClients_V& GetClients();
		CoreClientPtr GetPlayer(netid netId);
		CoreClientPtr GetClientFromServerNetId(netid serverNetId);

	protected:
		// Event 
		virtual void	OnConnectMultiPlug() { }

	private:
		std::string	m_SvrType;
		MultiPlugPtr m_pMultiPlug;
	};

}}
