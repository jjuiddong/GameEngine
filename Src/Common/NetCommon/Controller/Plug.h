/**
Name:   NetConnector.h
Author:  jjuiddong
Date:    12/25/2012

������ Ŭ���̾�Ʈ�� ���� �θ� Ŭ������. ��������� �ϴ� ��ü���� �Ϲ�ȭ
Ŭ������.
*/
#pragma once

namespace network
{
	class CPlug : public CEventHandler, public CSession
	{
	public:
		CPlug(PROCESS_TYPE procType);
		virtual ~CPlug();

		void				SetParent(PlugPtr parent);
		PlugPtr			GetParent() const;
		bool				AddChild( CPlug *pChild );
		bool				RemoveChild( netid childId );
		PlugPtr			GetChild( netid childId, bool isFindChildren=false );
		Plugs_&		GetChildren();

		bool				RegisterProtocol(ProtocolPtr protocol);
		virtual bool	AddProtocolListener(ProtocolListenerPtr pListener, bool isApplyChildren=true);
		virtual bool	RemoveProtocolListener(ProtocolListenerPtr pListener, bool isApplyChildren=true);
		const ProtocolListenerList&	GetProtocolListeners() const;

		void				SetThreadHandle(HANDLE handle);
		HANDLE		GetThreadHandle() const;
		PROCESS_TYPE GetProcessType() const;

		// child implementes
		virtual bool	Send(netid netId, const SEND_FLAG flag, CPacket &packet) = 0;
		virtual bool	SendAll(CPacket &packet) = 0;

	protected:
		// EventHandler Overring
		virtual bool	SearchEventTable( common::CEvent &event ) override;

	private:
		PlugPtr						m_pParent;				// CNetConnector ������
		ProtocolListenerList m_ProtocolListeners;		
		HANDLE					m_hThread;				// �Ҽӵ� ������ �ڵ�, ���ٸ� NULL
		PROCESS_TYPE		m_ProcessType;
		Plugs_						m_Children;

	};


	inline const ProtocolListenerList&	 CPlug::GetProtocolListeners() const { return m_ProtocolListeners; }
	inline void	 CPlug::SetParent(PlugPtr parent) { m_pParent = parent; }
	inline PlugPtr CPlug::GetParent() const { return m_pParent; }
	inline void	 CPlug::SetThreadHandle(HANDLE handle) { m_hThread = handle; }
	inline HANDLE CPlug::GetThreadHandle() const { return m_hThread; }
	inline PROCESS_TYPE CPlug::GetProcessType() const { return m_ProcessType; }
	inline Plugs_&	CPlug::GetChildren() { return m_Children; }

}
