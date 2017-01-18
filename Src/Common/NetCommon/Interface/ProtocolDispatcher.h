//------------------------------------------------------------------------
// Name:    ProtocolDispatcher.h
// Author:  jjuiddong
// Date:    1/2/2013
// 
// ��Ʈ��ũ�κ��� ���� ��Ŷ�� �ش��ϴ� �������ݿ� �°� �м��ؼ� Listener�� 
// �Լ��� ȣ���Ѵ�.
// Dispatch �Լ����� ���� �������� �Լ����� ȣ���ϰ� �ȴ�.
// ��Ʈ��ũ �������� ���� �����Ϸ��� Dispatch()�� �ҽ����Ϸ� ������.
//------------------------------------------------------------------------
#pragma once
#include <string>

namespace network
{
	class IProtocolDispatcher
	{
	public:
		IProtocolDispatcher(int id) : m_Id(id), m_pCurrentDispatchPacket(NULL) {}
		virtual ~IProtocolDispatcher() {}
		friend class CTaskLogic;
		friend class CCoreClient;
		friend class CServerBasic;

		int GetId() const;
		void SetCurrentDispatchPacket( CPacket *pPacket );
		virtual void PrintThisPacket(int logType /*common::log::LOG_LEVEL*/, const std::string &msg);

	protected:
		virtual bool Dispatch(CPacket &packet, const ProtocolListenerList &listeners)=0;
		int m_Id; // �����ϴ� protocol ID �� ������ ���̴�.
		CPacket *m_pCurrentDispatchPacket;
	};

	inline int IProtocolDispatcher::GetId() const { return m_Id; }
	inline void IProtocolDispatcher::SetCurrentDispatchPacket( CPacket *pPacket ) { m_pCurrentDispatchPacket = pPacket; }

}
