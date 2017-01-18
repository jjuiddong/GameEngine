/**
Name:   GroupTraverse.h
Author:  jjuiddong
Date:    2013-03-14

CGroup Tree Traverse algorithm functions
*/
#pragma once

namespace network { namespace group {

	//------------------------------------------------------------------------
	// boost::bind ���� GroupPtr�� ���� �ν����� ���ؼ� �ϴ� CGroup*
	// �� ���ڷ� �ް��ߴ�.
	// ������ ty�� true�̸� pGroup�� �����Ѵ�.
	//------------------------------------------------------------------------
	template<class T>
	CGroup* TraverseUpward( CGroup *pGroup, T &ty )
	{
		RETV(!pGroup, NULL);
		if (ty(pGroup))
			return pGroup;
		return TraverseUpward(pGroup->GetParent(), ty);
	}

	template<class T>
	CGroup* TraverseDownward( CGroup *pGroup, T &ty )
	{
		RETV(!pGroup, NULL);
		if (ty(pGroup))
			return pGroup;

		BOOST_FOREACH(auto &child, pGroup->GetChildren())
		{
			CGroup *ptr = TraverseDownward(child, ty);
			if (ptr)
				return ptr;
		}
		return NULL;
	}



	bool			IsP2PConnection( GroupPtr pGroup );

	netid		GetP2PHostClient( GroupPtr pGroup, ISessionAccess &userAccess);

	GroupPtr	GetP2PGroup( GroupPtr pGroup );

	netid		SelectP2PHostClient( GroupPtr pGroup );
	
}}
