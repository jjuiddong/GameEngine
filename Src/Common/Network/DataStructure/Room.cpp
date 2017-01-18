
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
// ���� �߰�
//------------------------------------------------------------------------
bool CRoom::AddUser(netid groupId, netid userId)
{
	const bool result = m_RootGroup.AddPlayer(groupId, userId);

	return result;
}


//------------------------------------------------------------------------
// ���� ����
//------------------------------------------------------------------------
bool CRoom::RemoveUser(netid groupId, netid userId)
{
	const bool result = m_RootGroup.RemovePlayer(groupId, userId);

	return result;
}


//------------------------------------------------------------------------
// �ʱ�ȭ
//------------------------------------------------------------------------
void CRoom::Clear()
{
	m_RootGroup.Clear();
}


//------------------------------------------------------------------------
// �׷��߰�
// ���ڷ� �Ѿ�� Group Ŭ������ �״�� �����Ѵ�.
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
// �׷� ����
// �޸𸮵� ���ŵȴ�.
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
// �׷� ���
//------------------------------------------------------------------------
GroupPtr CRoom::GetGroup(netid groupId )
{
	GroupPtr ptr = m_RootGroup.GetChild(groupId);
	return ptr;
}


//------------------------------------------------------------------------
// userId�� ���� ������ Group�� ���� �ִٸ� true�� �����Ѵ�.
//------------------------------------------------------------------------
bool CRoom::IsUserExist(netid userId)
{
	return m_RootGroup.IsExistPlayer(userId);
}
