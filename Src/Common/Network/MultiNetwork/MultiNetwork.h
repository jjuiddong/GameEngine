/**
Name:   MultiNetwork.h
Author:  jjuiddong
Date:    4/1/2013

 �׷����� ���� ������ Ŭ���̾�Ʈ��� ��Ʈ��ŷ�ϴ� ���� ����Ѵ�.
 �׷쿡 �����ϴ� ��ü�� CNetGroupController �̰�, �� ��ü�� �����Ѵ�.
*/
#pragma once

#include "MultiNetworkUtility.h"

namespace network { namespace multinetwork {

	class CMultiPlug;
	class CMultiPlugDelegation;
	class CFarmServerPlug;

	DECLARE_TYPE_NAME_SCOPE(network::multinetwork, CMultiNetwork)
	class CMultiNetwork : public CPlug
										, public common::CSingleton<CMultiNetwork>
										,public memmonitor::Monitor<CMultiNetwork, TYPE_NAME(network::multinetwork::CMultiNetwork)>
	{
	public:
		CMultiNetwork();
		virtual ~CMultiNetwork();

		bool		Init( const std::string &configFileName );
		bool		Start();
		bool		Stop();
		bool		Proc();

		bool		AddController( CMultiPlug *ptr );
		bool		RemoveController( const std::string &linkSvrType );
		MultiPlugPtr GetMultiPlug( const std::string &linkSvrType );

		bool		AddDelegation( const std::string &linkSvrType, CMultiPlugDelegation *ptr);
		bool		RemoveDelegation( const std::string &linkSvrType );
		bool		ConnectDelegation( const std::string &linkSvrType, MultiPlugDelegationPtr ptr);
		MultiPlugDelegationPtr GetDelegation( const std::string &linkSvrType );

		virtual bool	Send(netid netId, const SEND_FLAG flag, CPacket &packet) override;
		virtual bool	SendAll(CPacket &packet) override;

	protected:
		MultiPlugPtr	GetControllerFromNetId( netid netId );
		void		Cleanup();

	private:
		typedef common::VectorMap<std::string, MultiPlugPtr> Controllers;
		typedef common::VectorMap<std::string, CMultiPlugDelegation*> Delegations;

		SSvrConfigData m_Config;
		Controllers m_Controllers;
		Delegations m_Delegations;
		CFarmServerPlug *m_pFarmSvrConnector;

	};
}}
