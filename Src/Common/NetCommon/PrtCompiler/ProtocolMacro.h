//------------------------------------------------------------------------
// Name:    ProtocolMacro.h
// Author:  jjuiddong
// Date:    1/4/2013
// 
// �������� �����Ϸ��� �����ϴ� Protocol, ProtocolListener�� ���� ��ũ��
// ���� ��Ƴ��Ҵ�.
//------------------------------------------------------------------------
#pragma once

// Dispatcher::Dispatch ���� ���Ǵ� ��ũ��
#define SEND_LISTENER(listenerType, listeners, func)												\
	BOOST_FOREACH(ProtocolListenerPtr p, listeners)												\
	{																																\
		listenerType *lstr = dynamic_cast<listenerType*>((IProtocolListener*)p);		\
		if (lstr)																													\
			lstr->func;																										\
	}																																\


/**
 @brief ���ø� ���� T Ÿ�԰� ���� ���������� matchListeners �� �־� �����Ѵ�.
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
