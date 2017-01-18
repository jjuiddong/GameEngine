/**
Name:   NetConnectorLinker.h
Author:  jjuiddong
Date:    4/3/2013

	network::CNetConnectorLinker ����

	- CNetConnectorLinker �� CNetConnector Ŭ������ �̾��ִ� ������ �Ѵ�.
	- CNetConnector Wraping Ŭ����

	- CNetConnector ��ü�� �ƴϸ鼭, CNetConnector ��ü�� ��Ŷ�� �ְ� �޴� ��ü�� �� Ŭ������
	��ӹ޾� �����ϸ� �����ϴ�.
*/
#pragma once

#include "Plug.h"
/*
namespace network
{

	/// NetConnector Wrapper class, CEventHandler �� protected ����� �޾Ƽ� Event�� ���õ�
	/// �Լ��� �������� ���ϵ��� �Ѵ�. �⺻������ CNetConnectorLinker�� Event �� GetConnector()
	/// �Լ��� ���� ��ϵǾ�� �Ѵ�.
	class CPlugLinker : protected CEventHandler
	{
	public:
		CPlugLinker();
		CPlugLinker(PlugPtr ptr);
		virtual ~CPlugLinker() { }

		void				SetPlug( PlugPtr ptr );
		PlugPtr			GetPlug() const;
		bool				RegisterProtocol(ProtocolPtr protocol);
		virtual bool	AddProtocolListener(ProtocolListenerPtr pListener);
		virtual bool	RemoveProtocolListener(ProtocolListenerPtr pListener);
		const ProtocolListenerList&	GetProtocolListeners() const;
		void				ClearConnection();

		using CEventHandler::EventConnect;
		bool				EventConnect(CEventHandler *handler, EventType type, EventFunction fn);

		netid			GetNetId() const;
		SOCKET		GetSocket() const;
		void				SetSocket(SOCKET sock);
		void				SetParent(PlugPtr parent);
		PlugPtr GetParent() const;
		void				SetThreadHandle(HANDLE handle);
		HANDLE		GetThreadHandle() const;
		PROCESS_TYPE GetProcessType() const;

		// child implementes
		virtual bool	Send(netid netId, const SEND_FLAG flag, const CPacket &packet);
		virtual bool	SendAll(const CPacket &packet);

	private:
		PlugPtr m_pNetCon;
		static ProtocolListenerList s_emptyListeners;
	};


	ProtocolListenerList CPlugLinker::s_emptyListeners;
	inline CPlugLinker::CPlugLinker() : m_pNetCon(NULL) {}
	inline CPlugLinker::CPlugLinker(PlugPtr ptr) : m_pNetCon(ptr) {}
	inline void	CPlugLinker::SetPlug( PlugPtr ptr ) { m_pNetCon = ptr; }
	inline PlugPtr CPlugLinker::GetPlug() const { return m_pNetCon; }
	inline void	 CPlugLinker::SetSocket(SOCKET sock) { if (m_pNetCon) m_pNetCon->SetSocket(sock); }

	inline bool CPlugLinker::RegisterProtocol(ProtocolPtr protocol) { if (m_pNetCon) return m_pNetCon->RegisterProtocol(protocol); return false; }
	inline bool CPlugLinker::AddProtocolListener(ProtocolListenerPtr pListener) { if (m_pNetCon) return m_pNetCon->AddProtocolListener(pListener); return false; }
	inline bool CPlugLinker::RemoveProtocolListener(ProtocolListenerPtr pListener) { if (m_pNetCon) return m_pNetCon->RemoveProtocolListener(pListener); return false;}

	inline const ProtocolListenerList&	 CPlugLinker::GetProtocolListeners() const { return (m_pNetCon!=NULL? m_pNetCon->GetProtocolListeners() : s_emptyListeners); }
	inline netid CPlugLinker::GetNetId() const { return (m_pNetCon!=NULL? m_pNetCon->GetNetId() : INVALID_NETID); }
	inline SOCKET CPlugLinker::GetSocket() const { return (m_pNetCon!=NULL? m_pNetCon->GetSocket() : 0); }
	inline void	 CPlugLinker::SetParent(PlugPtr parent) { if (m_pNetCon!=NULL) m_pNetCon->SetParent(parent); }
	inline PlugPtr CPlugLinker::GetParent() const { return (m_pNetCon!=NULL? m_pNetCon->GetParent() : NULL); }
	inline void	 CPlugLinker::SetThreadHandle(HANDLE handle) { if (m_pNetCon!=NULL) m_pNetCon->SetThreadHandle(handle); }
	inline HANDLE CPlugLinker::GetThreadHandle() const { return (m_pNetCon!=NULL? m_pNetCon->GetThreadHandle() : NULL); }
	inline PROCESS_TYPE CPlugLinker::GetProcessType() const { return (m_pNetCon!=NULL? m_pNetCon->GetProcessType() : USER_LOOP); }
	inline bool CPlugLinker::EventConnect(CEventHandler *handler, EventType type, EventFunction fn) {
		if (m_pNetCon)
			return m_pNetCon->EventConnect(handler, type, fn);
		return false;
	}
	inline bool CPlugLinker::Send(netid netId, const SEND_FLAG flag, const CPacket &packet) {
		if (m_pNetCon)
			return m_pNetCon->Send(netId, flag, packet);
		return false;
	}
	inline bool CPlugLinker::SendAll(const CPacket &packet) {
		if (m_pNetCon)
			return m_pNetCon->SendAll(packet);
		return false;
	}

}
/**/