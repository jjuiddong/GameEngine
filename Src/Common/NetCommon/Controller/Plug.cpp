
#include "stdafx.h"
#include "Plug.h"

using namespace network;


CPlug::CPlug(PROCESS_TYPE procType) :
	m_ProcessType(procType)
,	m_pParent(NULL)
,	m_hThread(NULL)
{

}

CPlug::~CPlug()
{

}


//------------------------------------------------------------------------
// 프로토콜 추가
//------------------------------------------------------------------------
bool CPlug::RegisterProtocol(ProtocolPtr protocol)
{
	protocol->SetNetConnector(this);
	return true;
}


//------------------------------------------------------------------------
// 리스너 등록
//------------------------------------------------------------------------
bool CPlug::AddProtocolListener(ProtocolListenerPtr pListener, bool isApplyChildren) // isApplyChildren = true
{
	ProtocolListenerItor it = find(m_ProtocolListeners.begin(), m_ProtocolListeners.end(), pListener);
	if (m_ProtocolListeners.end() != it)
		return false; // 이미 존재한다면 실패
	m_ProtocolListeners.push_back( pListener);

	if (isApplyChildren)
	{
		BOOST_FOREACH(auto child, m_Children.m_Seq)
		{
			if (child)
				child->AddProtocolListener(pListener, isApplyChildren);
		}
	}
	return true;
}


//------------------------------------------------------------------------
// 리스너 제거 (메모리는 외부에서 소거해야한다.)
//------------------------------------------------------------------------
bool CPlug::RemoveProtocolListener(ProtocolListenerPtr pListener, bool isApplyChildren) // isApplyChildren=true
{
	ProtocolListenerItor it = find(m_ProtocolListeners.begin(), m_ProtocolListeners.end(), pListener);
	if (m_ProtocolListeners.end() == it)
		return false; // 없다면 실패
	m_ProtocolListeners.erase(it);

	if (isApplyChildren)
	{
		BOOST_FOREACH(auto child, m_Children.m_Seq)
		{
			if (child)
				child->RemoveProtocolListener(pListener, isApplyChildren);
		}
	}
	return true;
}


/**
 @brief Add child
 */
bool	CPlug::AddChild( CPlug *pChild )
{
	RETV(!pChild, false);
	RETV(this == pChild, false);

	auto it = m_Children.find(pChild->GetNetId());
	if (m_Children.end() != it)
		return false; /// Already Exist
	
	pChild->SetParent(this);
	m_Children.insert( Plugs_::value_type(pChild->GetNetId(), pChild) );

	// apply listener
	BOOST_FOREACH(auto &protocol, m_ProtocolListeners)
	{
		pChild->AddProtocolListener(protocol);
	}
	return true;
}


/**
 @brief Remove Child
 */
bool	CPlug::RemoveChild( netid childId )
{
	auto it = m_Children.find(childId);
	if (m_Children.end() == it)
		return false; /// Not Exist

	// delete in EventTable entry
	EventDisconnect(it->second, EVT_NULL);

	it->second->SetParent(NULL);
	m_Children.remove(childId);
	m_Children.apply_removes();
	return true;
}


/**
 @brief GetChild
 */
PlugPtr	CPlug::GetChild( netid childId, bool isFindChildren ) // isFindChildren = false
{
	if (GetNetId() == childId)
		return this;

	auto it = m_Children.find(childId);
	if (m_Children.end() != it)
		return it->second;

	if (isFindChildren)
	{
		BOOST_FOREACH(auto child, m_Children.m_Seq)
		{
			if (!child)
				continue;			
			if (PlugPtr	p = child->GetChild(childId, isFindChildren))
				return p;
		}
	}
	return NULL;
}


/**
 @brief SearchEventTable
 */
bool CPlug::SearchEventTable( common::CEvent &event )
{
	CEventHandler::SearchEventTable(event);
	if (!event.IsSkip() && m_pParent)
		m_pParent->SearchEventTable(event);
	return true;
}
