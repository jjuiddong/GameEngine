/**
Name:   GroupTraverse.h
Author:  jjuiddong
Date:    2013-03-14

CGroup Tree Traverse algorithm functions
*/
#pragma once

namespace network { namespace group {

	//------------------------------------------------------------------------
	// boost::bind 에서 GroupPtr을 쓰면 인식하지 못해서 일단 CGroup*
	// 를 인자로 받게했다.
	// 서술자 ty가 true이면 pGroup을 리턴한다.
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
