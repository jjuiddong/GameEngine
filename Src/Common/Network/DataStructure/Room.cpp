
#include "stdafx.h"
#include "Room.h"
#include <boost/bind.hpp>

using namespace network; 


CRoom::CRoom() : 
	m_RootGroup(NULL, "root")
{

}

CRoom::~CRoom()
{
	Clear();
}


//------------------------------------------------------------------------
// 유저 추가
//------------------------------------------------------------------------
bool CRoom::AddUser(netid groupId, netid userId)
{
	const bool result = m_RootGroup.AddPlayer(groupId, userId);

	return result;
}


//------------------------------------------------------------------------
// 유저 제거
//------------------------------------------------------------------------
bool CRoom::RemoveUser(netid groupId, netid userId)
{
	const bool result = m_RootGroup.RemovePlayer(groupId, userId);

	return result;
}


//------------------------------------------------------------------------
// 초기화
//------------------------------------------------------------------------
void CRoom::Clear()
{
	m_RootGroup.Clear();
}


//------------------------------------------------------------------------
// 그룹추가
// 인자로 넘어온 Group 클래스를 그대로 저장한다.
//------------------------------------------------------------------------
bool CRoom::AddGroup( CGroup *pGroup )
{
	RETV(!pGroup, false);
	const bool result = m_RootGroup.AddChild(pGroup);
	//if (!result)
	//{
	//	LogNPrint( "CRoom::AddGroup() Error!! groupName: %s, id: %d", 
	//		pGroup->GetName().c_str(), pGroup->GetNetId());
	//}
	return result;
}


//------------------------------------------------------------------------
// 그룹 제거
// 메모리도 제거된다.
//------------------------------------------------------------------------
bool CRoom::RemoveGroup( netid groupId )
{
	const bool result = m_RootGroup.RemoveChild(groupId);
	//if (!result)
	//{
	//	LogNPrint( "CRoom::RemoveGroup() Error!! id: %d", groupId);			
	//}
	return result;
}


//------------------------------------------------------------------------
// 그룹 얻기
//------------------------------------------------------------------------
GroupPtr CRoom::GetGroup(netid groupId )
{
	GroupPtr ptr = m_RootGroup.GetChild(groupId);
	return ptr;
}


//------------------------------------------------------------------------
// userId를 가진 유저가 Group에 속해 있다면 true를 리턴한다.
//------------------------------------------------------------------------
bool CRoom::IsUserExist(netid userId)
{
	return m_RootGroup.IsExistPlayer(userId);
}
