/**
Name:   NetEvent.h
Author:  jjuiddong
Date:    4/4/2013

	Network Event Class
*/
#pragma once

namespace network
{
	using namespace common;
	class CNetEvent;


	// Network Event Handler Function
	typedef void (CEventHandler::*NetEventFunction)(CNetEvent &);

#define NetEventHandler( fn ) \
	(EventFunction)static_cast<NetEventFunction>(&fn)


	// 컴파일 워닝을 없애기위한 매크로
#define NETEVENT_CONNECT_TO( to, handler, eventType, classname, fn )\
	{\
	void (classname::*evt_handler)(CNetEvent &) = &fn;\
	(to)->EventConnect( handler, eventType, (EventFunction)(NetEventFunction)evt_handler); \
	}

#define EVENT_CONNECT_TO( to, handler, eventType, classname, fn )\
	{\
	void (classname::*evt_handler)(CEvent &) = &fn;\
	(to)->EventConnect( handler, eventType, (EventFunction)evt_handler); \
	}


#define NETEVENT_CONNECT( eventType, classname, fn )\
		NETEVENT_CONNECT_TO( this, this, eventType, classname, fn )
	
#define EVENT_CONNECT( eventType, classname, fn )\
	EVENT_CONNECT_TO( this, this, eventType, classname, fn )


	/// Network Event Class
	class CNetEvent : public common::CEvent
	{
	public:
		CNetEvent(EventType type, PlugPtr ptr, netid id=0);
		virtual ~CNetEvent() {}

		netid GetNetId() const;
		void SetNetId(netid netId);
		PlugPtr GetEventObject();
		void SetEventObject(PlugPtr ptr);

	private:
		netid m_netId;
		PlugPtr m_eventObject;
	};

 
	inline CNetEvent::CNetEvent(EventType type, PlugPtr ptr, netid id) : CEvent(type),
		m_netId(id), m_eventObject(ptr) {}
	inline netid CNetEvent::GetNetId() const { return m_netId; }
	inline void CNetEvent::SetNetId(netid netId) { m_netId = netId; }
	inline PlugPtr CNetEvent::GetEventObject() { return m_eventObject; }
	inline void CNetEvent::SetEventObject(PlugPtr ptr) { m_eventObject = ptr; }

}
