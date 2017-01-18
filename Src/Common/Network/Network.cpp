
#include "stdafx.h"
#include "Network.h"
#include "Controller/Controller.h"
#include "MultiNetwork/MultiNetwork.h"
#include <MMSystem.h>


using namespace network;

//------------------------------------------------------------------------
// 네트워크에 관련된 클래스들을 초기화 한다.
//------------------------------------------------------------------------
bool network::Init(int logicThreadCount, const std::string &svrConfigFileName) // svrConfigFileName=""
{
	srand( timeGetTime() );
	clog::Log( clog::LOG_F_N_O, "Network Init\n" );

	common::InitRandNoDuplicate();
	protocols::Init();

	bool result = CController::Get()->Init(logicThreadCount);
	if (!result)
		clog::Error( clog::ERROR_CRITICAL, 0, "CController::Get()->Init() Error!!" );

	if (result)
	{
		result = multinetwork::CMultiNetwork::Get()->Init( svrConfigFileName );
	}

	return result;
}


//------------------------------------------------------------------------
// 메모리 제거
//------------------------------------------------------------------------
void network::Clear()
{
	dbg::Print( "Network Clear" );
	protocols::Cleanup();
	CController::Release();
	multinetwork::CMultiNetwork::Release();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool network::StartServer(int port, ServerBasicPtr pSvr)
{
	clog::Log( clog::LOG_F_N_O, "StartServer port: %d\n", port );
	return CController::Get()->StartServer(port, pSvr);
}

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool network::StopServer(ServerBasicPtr pSvr)
{
	if (!pSvr) return false;
	clog::Log( clog::LOG_F_N_O, "StopServer netid: %d\n", pSvr->GetNetId() );
	return pSvr->Stop();
}

//------------------------------------------------------------------------
// serverid 에 해당하는 서버를 리턴한다.
//------------------------------------------------------------------------
ServerBasicPtr network::GetServer(netid serverId)
{
	return CController::Get()->GetServer(serverId);
}

//------------------------------------------------------------------------
// 클라이언트 실행
//------------------------------------------------------------------------
bool network::StartClient(const std::string &ip, int port, ClientBasicPtr pClt)
{
	clog::Log( clog::LOG_F_N_O, "StartClient %s, %d\n", ip.c_str(), port);
	return CController::Get()->StartClient(ip, port, pClt);
}

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool network::StopClient(ClientBasicPtr pClt)
{
	if (!pClt) return false;
	clog::Log( clog::LOG_F_N_O, "StopClient netid: %d\n", pClt->GetNetId() );
	return pClt->Stop();
}


/**
 @brief Connect Delegation to multinetwork 
 */
bool	network::ConnectDelegation( const std::string &linkSvrType, MultiPlugDelegationPtr ptr)
{
	return multinetwork::CMultiNetwork::Get()->ConnectDelegation( linkSvrType, ptr );
}


/**
 @brief AddDelegation
 */
bool	network::AddDelegation( const std::string &linkSvrType, multinetwork::CMultiPlugDelegation *ptr)
{
	return multinetwork::CMultiNetwork::Get()->AddDelegation(linkSvrType, ptr);
}


/**
 @brief multinetwork Start
 */
bool	network::StartMultiNetwork()
{
	return multinetwork::CMultiNetwork::Get()->Start();
}


//------------------------------------------------------------------------
// clientId에 해당하는 클라이언트를 리턴한다.
//------------------------------------------------------------------------
ClientBasicPtr network::GetClient(netid clientId)
{
	return CController::Get()->GetClient(clientId);
}
CoreClientPtr network::GetCoreClient(netid clientId)
{
	return CController::Get()->GetCoreClient(clientId);
}

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void network::Proc()
{
	CController::Get()->Proc();
}


//------------------------------------------------------------------------
// 스트링변환, 디버그에 필요한 정보를 스트링으로 내보낸다.
//------------------------------------------------------------------------
std::string network::ToString()
{
	return CController::Get()->ToString();
}
