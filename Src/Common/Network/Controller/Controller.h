/**
Name:   Controller.h
Author:  jjuiddong
Date:    12/24/2012


네트워크에 필요한 쓰레드와 서버,클라이언트 객체를 관리한다.
Dipatcher 를 관리한다.

*/
#pragma once

namespace network
{
	DECLARE_TYPE_NAME_SCOPE(network, CController)
	class CController : public common::CSingleton<CController>
								, public memmonitor::Monitor<CController, TYPE_NAME(network::CController)>
	{
	public:
		CController();
		virtual ~CController();
		friend class CTaskLogic;
		friend class CTaskAccept;
		friend class CTaskWorkClient;
		friend class CTaskWorkServer;

		typedef std::map<int,IProtocolDispatcher*> DispatcherMap;

		typedef DispatcherMap::iterator DispatcherItor;
		typedef std::list<common::CThread*> ThreadList;
		typedef ThreadList::iterator ThreadItor;

		enum { VECTOR_RESERVED_SIZE = 128, };

		bool		Init(int logicThreadCount);
		void		Proc();
		void		Clear();

		// Server
		bool		StartServer(int port, ServerBasicPtr pSvr);
		bool		StopServer(ServerBasicPtr pSvr);
		bool		RemoveServer(ServerBasicPtr pSvr);
		ServerBasicPtr	GetServer(netid netId);

		// Client
		bool		StartClient(const std::string &ip, int port, ClientBasicPtr pClt);
		bool		StopClient(ClientBasicPtr pClt);
		bool		RemoveClient(ClientBasicPtr pClt);
		ClientBasicPtr	GetClient(netid netId);

		// CoreClient
		bool		StartCoreClient(const std::string &ip, int port, CoreClientPtr pClt);
		bool		StopCoreClient(CoreClientPtr  pClt);
		bool		RemoveCoreClient(CoreClientPtr  pClt);
		CoreClientPtr	GetCoreClient(netid netId);

		// protocol
		void		AddDispatcher(IProtocolDispatcher *pDispatcher);
		IProtocolDispatcher* GetDispatcher(int protocolID);

		int		GetUniqueValue() const;

		// debug
		std::string ToString();

	protected:
		ThreadPtr AllocWorkThread(SERVICE_TYPE serviceType, PlugPtr pConnector);
		ThreadPtr GetThread( const ThreadList &threads, HANDLE hThreadHandle );
		void		MakeServersFDSET( SFd_Set *pfdset);
		void		MakeCoreClientsFDSET( PROCESS_TYPE procType, SFd_Set *pfdset);
		void		DisconnectServer(ServerBasicPtr pSvr);
		void		DisconnectClient(ClientBasicPtr pClt);
		void		DisconnectCoreClient(CoreClientPtr pClt);
		void		MainLoop();
		void		RemoveProcess();

	private:
		Servers							m_Servers;
		Clients							m_Clients;
		CoreClients					m_CoreClients;

		DispatcherMap				m_Dispatchers;
		common::CThread		m_AcceptThread;

		/// CServer가 SERVICE_THREAD 타입일때 추가된다.
		/// Thread 인스턴스는 m_WorkThreads 에 저장된다.
		ThreadPtr						m_pSeperateServerWorkThread;

		/// CCoreClient가 SERVICE_THREAD 타입일때 추가된다.
		/// Thread 인스턴스는 m_WorkThreads 에 저장된다.
		ThreadPtr						m_pSeperateClientWorkThread;	

		ThreadList					m_WorkThreads;
		ThreadList					m_LogicThreads;
		common::CriticalSection  m_CS;
		int								m_UniqueValue;

	};


	inline int	CController::GetUniqueValue() const { return m_UniqueValue; }

}
