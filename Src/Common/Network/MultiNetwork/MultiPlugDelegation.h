/**
Name:   NetGroupDelegation.h
Author:  jjuiddong
Date:    4/1/2013

network::multinetwork::CNetGroupDelegation ����
- Network Group �� �����ϴ� Controller�� ���� ��Ŷ�� ���޹ް�, �̺�Ʈ�� �ڵ鸵�Ѵ�.
*/
#pragma once

namespace network { namespace multinetwork {

	/// Network Group �� �����ϴ� Controller�� ���� ��Ŷ�� ���޹ް�, �̺�Ʈ�� �ڵ鸵�Ѵ�.
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
