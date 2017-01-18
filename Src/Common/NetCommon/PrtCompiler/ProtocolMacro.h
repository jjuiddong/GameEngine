//------------------------------------------------------------------------
// Name:    ProtocolMacro.h
// Author:  jjuiddong
// Date:    1/4/2013
// 
// 프로토콜 컴파일러가 생성하는 Protocol, ProtocolListener에 들어가는 매크로
// 들을 모아놓았다.
//------------------------------------------------------------------------
#pragma once

// Dispatcher::Dispatch 에서 사용되는 매크로
#define SEND_LISTENER(listenerType, listeners, func)												\
	BOOST_FOREACH(ProtocolListenerPtr p, listeners)												\
	{																																\
		listenerType *lstr = dynamic_cast<listenerType*>((IProtocolListener*)p);		\
		if (lstr)																													\
			lstr->func;																										\
	}																																\


/**
 @brief 템플릿 인자 T 타입과 같은 프로토콜을 matchListeners 에 넣어 리턴한다.
 @return if matchListeners is empty return false, or return true
 */
template<class T>
inline bool ListenerMatching(const ProtocolListenerList &listeners, OUT ProtocolListenerList &matchListeners)
{
	BOOST_FOREACH(auto &it, listeners)
	{
		T *lstr = dynamic_cast<T*>((IProtocolListener*)it.Get());
		if (lstr)
			matchListeners.push_back(lstr);
	}
	return !matchListeners.empty();
}
