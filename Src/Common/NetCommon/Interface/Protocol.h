/**
Name:   Protocol.h
Author:  jjuiddong
Date:    12/23/2012

 ������ Ŭ���̾�Ʈ���� �ְ�޴� ���������� �����Ѵ�.
 ������ Ŭ���̾�Ʈ���� ���������� *.prt ���Ͽ� �����ؾ� �Ѵ�.
*/
#pragma once

namespace network
{
	class IProtocol
	{
		friend class CPlug;

	public:
		IProtocol(int id) : m_Id(id) {}
		virtual ~IProtocol() {}
		void SetId(int id) { m_Id = id; }
		int GetId() const { return m_Id; }
		const PlugPtr& GetNetConnector() const;
	protected:
		void SetNetConnector(PlugPtr pServer);
	private:
		int m_Id; // �����ϴ� ProtocolListener ID �� ������ ���̴�.
		PlugPtr m_pNetConnector;
	};


	inline const PlugPtr& IProtocol::GetNetConnector() const { return m_pNetConnector; }
	inline void IProtocol::SetNetConnector(PlugPtr pServer) { m_pNetConnector = pServer; }

}
