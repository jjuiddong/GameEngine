
#include "stdafx.h"
#include "Controller.h"
#include "Launcher.h"
#include "ServerBasic.h"
#include "../Service/Server.h"
#include "../Service/Client.h"
#include "CoreClient.h"
#include "../Task/TaskLogic.h"
#include "../task/TaskAccept.h"
#include "../task/TaskWorkClient.h"
#include "../task/TaskWorkServer.h"
#include "../task/TaskWork.h"
#include <boost/bind.hpp>


using namespace network;

CController::CController() :
	m_AcceptThread("AcceptThread")
,	m_Servers(VECTOR_RESERVED_SIZE)
,	m_Clients(VECTOR_RESERVED_SIZE)
,	m_pSeperateServerWorkThread(NULL)
,	m_pSeperateClientWorkThread(NULL)
{

}

CController::~CController() 
{
	Clear();
}


//------------------------------------------------------------------------
// logicThreadCount ������ŭ ���������带 �����Ѵ�.
//------------------------------------------------------------------------
bool CController::Init(int logicThreadCount)
{
	// ���������� ����
	for (int i=0; i < logicThreadCount; ++i)
	{
		common::CThread *pThread = new common::CThread("LogicThread");
		pThread->AddTask( new CTaskLogic() );
		pThread->Start();
		m_LogicThreads.push_back( pThread );
	}

	// Accept ������ ����
	if (logicThreadCount > 0)
	{
  		m_AcceptThread.AddTask( new CTaskAccept() );
  		m_AcceptThread.Start();
	}

	m_UniqueValue = rand();

	return true;
}


/**
@brief  User Thread Packet process
*/
void CController::Proc()
{
	// client user loop
	BOOST_FOREACH(ClientBasicPtr &ptr, m_Clients.m_Seq)
	{
		if (!ptr) break;
		if (ptr->GetProcessType() == USER_LOOP)
			ptr->Proc();
	}

	// server user loop
	BOOST_FOREACH(auto &pSvr, m_Servers.m_Seq)
	{
		if (!pSvr) break;
		if (pSvr->GetProcessType() == USER_LOOP)
			pSvr->Proc();
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool CController::StartServer(int port, ServerBasicPtr pSvr)
{
	RETV(!pSvr, false);

	{ /// Sync
		common::AutoCSLock cs(m_CS); 	/// Sync
		Servers::iterator it = m_Servers.find(pSvr->GetNetId());
		if (m_Servers.end() != it)
		{
			clog::Error( clog::ERROR_WARNING, "�̹� ����ǰ� �ִ� ������ �ٽ� ���������\n");
			return false;
		}
	}

	if (pSvr->IsServerOn())
		pSvr->Close();

	if (!launcher::LaunchServer(pSvr, port))
		return false;

	// ���� ���ۿ� ���õ� �ڵ� �߰�
	clog::Log( clog::LOG_F_N_O, "%d Server Start\n", pSvr->GetNetId() );

	{ /// Sync
		common::AutoCSLock cs(m_CS); /// Sync
		m_Servers.insert( Servers::value_type(pSvr->GetNetId(), pSvr) );
		//m_ServerSockets.insert( ServerSockets::value_type(pSvr->GetSocket(), pSvr) );
	}

	// Work ������ ����
	common::CThread *pWorkTread = AllocWorkThread(SERVER, pSvr.Get());
	if (pWorkTread)
	{
		pWorkTread->AddTask( new CTaskWorkServer((int)pSvr->GetSocket(), pSvr->GetNetId()) );
		pWorkTread->Start();
		pSvr->SetThreadHandle(pWorkTread->GetHandle()); // after Call Thread Start Function
	}

	return true;
}


//------------------------------------------------------------------------
// remove server container
// remove thread task
//------------------------------------------------------------------------
bool CController::StopServer(ServerBasicPtr pSvr)
{
	RETV(!pSvr, false);

	DisconnectServer(pSvr);
	return true;
}


/**
 @brief Remove Server
	Call from CServerBasic::Disconnect()
 */
bool	 CController::RemoveServer(ServerBasicPtr pSvr)
{
	RETV(!pSvr, false);

	clog::Log( clog::LOG_F_N_O, clog::LOG_MESSAGE, 0, "RemoveServer netid: %d \n", pSvr->GetNetId());

	CPacketQueue::Get()->RemovePacket(pSvr->GetNetId());

	common::AutoCSLock cs(m_CS); /// sync
	if (!m_Servers.remove(pSvr->GetNetId()))
		return false;
	pSvr->SetState(SESSIONSTATE_LOGOUT_WAIT);

	//ServerItor it = m_ServerSockets.find(pSvr->GetSocket());
	//if (m_ServerSockets.end() != it)
	//	m_ServerSockets.erase(it);

	return true;
}


//------------------------------------------------------------------------
// netId �� �ش��ϴ� ������ �����Ѵ�.
//------------------------------------------------------------------------
ServerBasicPtr CController::GetServer(netid netId)
{
	common::AutoCSLock cs(m_CS); /// sync

	Servers::iterator it = m_Servers.find(netId);
	if (m_Servers.end() == it)
		return NULL;
	return it->second;
}


//------------------------------------------------------------------------
// Ŭ���̾�Ʈ�� ip, port �� ������ ������ �õ��Ѵ�.
//------------------------------------------------------------------------
bool CController::StartClient(const std::string &ip, int port, ClientBasicPtr pClt)
{
	RETV(!pClt, false);

	if (pClt->IsConnect())
		pClt->Close(); // ������ ����

	clog::Log( clog::LOG_F_N_O, "%d Client Start\n", pClt->GetNetId() );
	if (!StartCoreClient(ip, port, pClt->GetConnectSvrClient()))
		return false;

	{ /// Sync
		common::AutoCSLock cs(m_CS); /// Sync
		Clients::iterator it = m_Clients.find( pClt->GetNetId());
		if (m_Clients.end() == it)
		{
			m_Clients.insert( Clients::value_type(pClt->GetNetId(), pClt) );
		}
	}
	return true;
}


//------------------------------------------------------------------------
// Ŭ���̾�Ʈ�� �����Ѵ�.
//------------------------------------------------------------------------
bool CController::StopClient(ClientBasicPtr pClt)
{
	RETV(!pClt, false);

	DisconnectClient(pClt);
	return true;
}


/**
 @brief Remove Client
 Call from CClientBasic::Disconnect
 */
bool	CController::RemoveClient(ClientBasicPtr pClt)
{
	RETV(!pClt, false);

	clog::Log( clog::LOG_F_N_O, clog::LOG_MESSAGE, 0, "RemoveClient netid: %d \n", pClt->GetNetId());

	CPacketQueue::Get()->RemovePacket(pClt->GetNetId());

	common::AutoCSLock cs(m_CS); 	/// Sync
	if (!m_Clients.remove(pClt->GetNetId()))
		clog::Error( clog::ERROR_PROBLEM, "StopClient Error!! netid: %d client\n", pClt->GetNetId());
	pClt->SetState(SESSIONSTATE_LOGOUT_WAIT);

	return true;
}


//------------------------------------------------------------------------
// clientId�� �ش��ϴ� Ŭ���̾�Ʈ�� �����Ѵ�.
//------------------------------------------------------------------------
ClientBasicPtr CController::GetClient(netid netId)
{
	common::AutoCSLock cs(m_CS); 	/// Sync

	Clients::iterator it = m_Clients.find(netId);
	if (m_Clients.end() == it)
		return NULL;
	return it->second;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool CController::StartCoreClient(const std::string &ip, int port, CoreClientPtr pClt)
{
	RETV(!pClt,false);

	if (pClt->IsConnect())
		pClt->Close(); // ������ ����

	// ���� ���ۿ� ���õ� �ڵ� �߰�
	clog::Log( clog::LOG_F_N_O, "%d Client Start\n", pClt->GetNetId() );

 	if (!launcher::LaunchCoreClient(pClt, ip, port))
 	{
 		clog::Error( clog::ERROR_CRITICAL, "StartCoreClient Error!! Launch Fail ip: %s, port: %d\n", ip.c_str(), port);
 		return false;
 	}

	{ /// Sync
		common::AutoCSLock cs(m_CS); 	/// Sync
		CoreClients::iterator it = m_CoreClients.find( pClt->GetNetId());
		if (m_CoreClients.end() == it)
			m_CoreClients.insert( CoreClients::value_type(pClt->GetNetId(), pClt) );
	}

	// CoreClient �Ӽ��� ���� Thread���� ��Ŷ�� ó������, ���� �������� ó������ �����Ѵ�.
	common::CThread *pWorkTread = AllocWorkThread(CLIENT, pClt.Get());
	if (pWorkTread)
	{
		if (SERVICE_EXCLUSIVE_THREAD == pClt->GetProcessType())
			pWorkTread->AddTask( new CTaskWork((int)pClt->GetSocket(), pClt->GetNetId(), pClt->GetSocket()) );
		pWorkTread->Start();
		pClt->SetThreadHandle(pWorkTread->GetHandle());
	}
	return true;
}


//------------------------------------------------------------------------
// remove coreclient list 
// remove task if SERVICE_EXCLUSIVE_THREAD mode coreclient
//------------------------------------------------------------------------
bool CController::StopCoreClient(CoreClientPtr pClt)
{
	RETV(!pClt, false);

	DisconnectCoreClient(pClt);
	return true;
}


/**
 @brief Remove CoreClient
 Call from CCoreClient::Disconnect()
 */
bool	CController::RemoveCoreClient(CoreClientPtr  pClt)
{
	RETV(!pClt, false);

	clog::Log( clog::LOG_F_N_O, clog::LOG_MESSAGE, 0, "RemoveCoreClient netid: %d \n", pClt->GetNetId());

	CPacketQueue::Get()->RemovePacket(pClt->GetNetId());

	{ /// Sync
		common::AutoCSLock cs(m_CS); 	/// Sync
		if (!m_CoreClients.remove(pClt->GetNetId()))
			clog::Error( clog::ERROR_PROBLEM, "StopClient Error!! netid: %d client\n", pClt->GetNetId());
		pClt->SetState(SESSIONSTATE_LOGOUT_WAIT);
	}

	// Stop CoreClient Work Thread
	if (pClt->GetProcessType() == SERVICE_EXCLUSIVE_THREAD)
	{
		ThreadPtr ptr = GetThread( m_WorkThreads, pClt->GetThreadHandle() );
		if (ptr)
			ptr->Send2ThreadMessage( common::threadmsg::TERMINATE_TASK, pClt->GetSocket(), 0 );
	}
	return true;
}


//------------------------------------------------------------------------
// clientId�� �ش��ϴ� Ŭ���̾�Ʈ�� �����Ѵ�.
//------------------------------------------------------------------------
CoreClientPtr CController::GetCoreClient(netid netId)
{
	common::AutoCSLock cs(m_CS); 	/// Sync

	CoreClients::iterator it = m_CoreClients.find(netId);
	if (m_CoreClients.end() == it)
		return NULL;
	return it->second;
}


//------------------------------------------------------------------------
// Protocol Dispatcher �߰�
//------------------------------------------------------------------------
void CController::AddDispatcher(IProtocolDispatcher *pDispatcher)
{
	common::AutoCSLock cs(m_CS); 	/// Sync

	DispatcherItor it = m_Dispatchers.find(pDispatcher->GetId());
	if (m_Dispatchers.end() != it)
	{
		clog::Error( clog::ERROR_WARNING, 
			common::format( "���� ProtocolDispatcher�� �̹� ����߽��ϴ�. DispatcherId: %d\n", pDispatcher->GetId()) );
		return; // �̹� �����Ѵٸ� ����
	}
	m_Dispatchers.insert( DispatcherMap::value_type(pDispatcher->GetId(), pDispatcher) );
}


//------------------------------------------------------------------------
// Protocol Dispatcher ����
//------------------------------------------------------------------------
IProtocolDispatcher* CController::GetDispatcher(int protocolID)
{
	common::AutoCSLock cs(m_CS); 	/// Sync

	DispatcherItor it = m_Dispatchers.find(protocolID);
	if (m_Dispatchers.end() == it)
		return NULL; // ���ٸ� ����
	return it->second;
}


//------------------------------------------------------------------------
// �������� fd_set �� �����ؼ� �����Ѵ�.
//------------------------------------------------------------------------
void CController::MakeServersFDSET( SFd_Set *pfdset )
{
	common::AutoCSLock cs(m_CS);

	FD_ZERO(pfdset);
	BOOST_FOREACH(auto &pSvr, m_Servers.m_Seq)
	{
		if (!pSvr)
			continue;
		if (!pSvr->IsConnect())
			continue;
		FD_SET( pSvr->GetSocket(), pfdset );
		pfdset->netid_array[ pfdset->fd_count-1] = pSvr->GetNetId();
	}
}


//------------------------------------------------------------------------
// CoreClient �߿��� procType �� �ش��ϴ� CoreClient�鸸 fd_set�� �����Ѵ�.
//------------------------------------------------------------------------
void	CController::MakeCoreClientsFDSET( PROCESS_TYPE procType, SFd_Set *pfdset)
{
	common::AutoCSLock cs(m_CS);

	FD_ZERO(pfdset);
	BOOST_FOREACH(CoreClientPtr &ptr, m_CoreClients.m_Seq)
	{
		if (!ptr) continue;
		if (ptr->GetProcessType() == procType)
		{
			FD_SET( ptr->GetSocket(), pfdset );
			pfdset->netid_array[ pfdset->fd_count-1] = ptr->GetNetId();
		}
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CController::Clear()
{
	m_AcceptThread.Terminate();

	BOOST_FOREACH( common::CThread *pThread, m_LogicThreads)
	{
		pThread->Terminate();
		delete pThread;
	}
	m_LogicThreads.clear();

	BOOST_FOREACH( common::CThread *pThread, m_WorkThreads)
	{
		pThread->Terminate();
		delete pThread;
	}
	m_WorkThreads.clear();

	m_pSeperateServerWorkThread = NULL;
	m_pSeperateClientWorkThread = NULL;

	CPacketQueue::Release();

}


//------------------------------------------------------------------------
// ��Ʈ������ ��ȯ, �ַ� ����뿡 ���õ� ������ ��Ʈ������ ��������.
//------------------------------------------------------------------------
std::string CController::ToString()
{
	std::stringstream ss;

	// ������ ����
	ss << "Thread.. " << std::endl;
	ss << m_AcceptThread.GetName() << " state: " << m_AcceptThread.GetState() << std::endl;

	BOOST_FOREACH( common::CThread *pThread, m_LogicThreads)
	{
		ss << pThread->GetName() << " state: " << pThread->GetState() << std::endl;
	}
	BOOST_FOREACH( common::CThread *pThread, m_WorkThreads)
	{
		ss << pThread->GetName() << " state: " << pThread->GetState() << std::endl;
	}	

	// ���� ����
	ss << std::endl;
	ss << "Server Cnt: " << m_Servers.size() << std::endl;

	// �����İ���
	ss << "Dispatcher Cnt: " << m_Dispatchers.size() << std::endl;
	BOOST_FOREACH( DispatcherMap::value_type &kv, m_Dispatchers)
	{
		ss << "id: " << kv.second->GetId() << std::endl;
	}

	return ss.str();
}


//------------------------------------------------------------------------
// �ش�Ǵ� Ÿ���� WorkThread�� �����Ѵ�.
//------------------------------------------------------------------------
ThreadPtr CController::AllocWorkThread(SERVICE_TYPE serviceType, PlugPtr pConnector)
{
	RETV(!pConnector, NULL);

	const PROCESS_TYPE processType = pConnector->GetProcessType();

	switch (serviceType)
	{
	case CLIENT:
		if (SERVICE_SEPERATE_THREAD == processType)
		{
			if (m_pSeperateClientWorkThread)
				return m_pSeperateClientWorkThread;

			common::CThread *pThread = new common::CThread("ClientWorkThread");
			m_pSeperateClientWorkThread = pThread;

			pThread->AddTask( new CTaskWorkClient(0) );
			m_WorkThreads.push_back(pThread);
			return pThread;
		}
		break;

	case SERVER:
		if (SERVICE_SEPERATE_THREAD == processType)
		{
			if (m_pSeperateServerWorkThread)
				return m_pSeperateServerWorkThread;

			common::CThread *pThread = new common::CThread("ServerWorkThread");
			m_pSeperateServerWorkThread = pThread;
			m_WorkThreads.push_back(pThread);
			return pThread;
		}
		break;
	}

	/// etc
	switch (processType)
	{
	case USER_LOOP: return NULL;
	case SERVICE_EXCLUSIVE_THREAD:
		{
			std::string threadName = (serviceType==SERVER)? "ServerWorkThread" : "ClientWorkThread";
			common::CThread *pThread = new common::CThread(threadName);
			m_WorkThreads.push_back(pThread);
			return pThread;
		}
		break;

	case SERVICE_CHILD_THREAD:
		{
			PlugPtr pParent = pConnector->GetParent();
			if (!pParent)
				return NULL;
			ThreadPtr ptr = GetThread( m_WorkThreads, pParent->GetThreadHandle() );
			return ptr;
		}
		break;
	}

	return NULL;
}


//------------------------------------------------------------------------
// find WorkThread of hThreadHandle 
//------------------------------------------------------------------------
ThreadPtr CController::GetThread( const ThreadList &threads, HANDLE hThreadHandle )
{
	common::AutoCSLock cs(m_CS); 	/// Sync

	auto it = std::find_if( threads.begin(), threads.end(), 
		boost::bind( &common::IsSameHandle<common::CThread>, _1, hThreadHandle) );
	if (threads.end() == it)
		return NULL;
	return *it;
}


/**
 @brief Call Disconnect function through the thread message communication
 */
void	CController::DisconnectServer(ServerBasicPtr pSvr)
{
	RET(!pSvr);

	CPacketQueue::Get()->PushPacket( 
		CPacketQueue::SPacketData(pSvr->GetNetId(), 
			DisconnectPacket(pSvr->GetNetId(), GetUniqueValue()) ));

	switch (pSvr->GetProcessType())
	{
	case USER_LOOP: 
	case SERVICE_SEPERATE_THREAD:  
		break;

	case SERVICE_EXCLUSIVE_THREAD:
		{
			ThreadPtr ptr = GetThread( m_WorkThreads, pSvr->GetThreadHandle() );
			if (!ptr) break;
			ptr->Send2ThreadMessage( common::threadmsg::TERMINATE_TASK, pSvr->GetSocket(), 0 );
		}
		break;
	}
}


/**
 @brief Call Disconnect function through the thread message communication
 */
void	CController::DisconnectClient(ClientBasicPtr pClt)
{
	RET(!pClt);

	CPacketQueue::Get()->PushPacket( 
		CPacketQueue::SPacketData(pClt->GetNetId(), 
			DisconnectPacket(pClt->GetNetId(), GetUniqueValue()) ));
}


/**
 @brief Call Disconnect function through the thread message communication
 */
void	CController::DisconnectCoreClient(CoreClientPtr pCoreClt)
{
	RET(!pCoreClt);

	CPacketQueue::Get()->PushPacket( 
		CPacketQueue::SPacketData(pCoreClt->GetNetId(), 
			DisconnectPacket(pCoreClt->GetNetId(), GetUniqueValue()) ));

	switch (pCoreClt->GetProcessType())
	{
	case USER_LOOP: 
	case SERVICE_SEPERATE_THREAD: 
		break;

	// ���� �� case �� ȣ��� ���� ����. core client �� ���� user loop������ �����Ѵ�.
	case SERVICE_EXCLUSIVE_THREAD:
		{
			ThreadPtr ptr = GetThread( m_WorkThreads, pCoreClt->GetThreadHandle() );
			if (!ptr) break;
			ptr->Send2ThreadMessage( common::threadmsg::TERMINATE_TASK, pCoreClt->GetSocket(), 0 );
		}
		break;
	}
}


/**
 @brief ���ŵ� Sever, Client �� ó���Ѵ�.
 */
void	CController::RemoveProcess()
{
	m_Clients.apply_removes();
	m_CoreClients.apply_removes();
	m_Servers.apply_removes();
}


/**
 @brief Main Loop , Logic Thread ���� ȣ���Ѵ�.
 */
void	CController::MainLoop()
{
	// ���ŵ� Sever, Client �� ó���Ѵ�.
	RemoveProcess();
	//

	// Server Main Loop Process
	common::AutoCSLock cs(m_CS); 	/// Sync
	BOOST_FOREACH(auto server, m_Servers.m_Seq)
	{
		if (server)
			server->MainLoop();
	}
	//
}
