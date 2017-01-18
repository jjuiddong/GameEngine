//------------------------------------------------------------------------
// Name:    ProtocolDispatcher.h
// Author:  jjuiddong
// Date:    1/2/2013
// 
// 네트워크로부터 받은 패킷을 해당하는 프로토콜에 맞게 분석해서 Listener의 
// 함수를 호출한다.
// Dispatch 함수에서 개별 프로토콜 함수들을 호출하게 된다.
// 네트워크 프로토콜 생성 컴파일러가 Dispatch()를 소스파일로 만들어낸다.
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
		int m_Id; // 대응하는 protocol ID 와 동일한 값이다.
		CPacket *m_pCurrentDispatchPacket;
	};

	inline int IProtocolDispatcher::GetId() const { return m_Id; }
	inline void IProtocolDispatcher::SetCurrentDispatchPacket( CPacket *pPacket ) { m_pCurrentDispatchPacket = pPacket; }

}
