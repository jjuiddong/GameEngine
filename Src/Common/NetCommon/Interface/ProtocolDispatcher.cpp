
#include "stdafx.h"
#include "ProtocolDispatcher.h"

using namespace network;


/**
	@brief m_pCurrentDispatchPacket 패킷 출력
	*/
void IProtocolDispatcher::PrintThisPacket(int logType/*common::log::LOG_LEVEL*/, const std::string &msg )
{
	RET(!m_pCurrentDispatchPacket);

	const int protocolID = m_pCurrentDispatchPacket->GetProtocolId();
	const int packetID = m_pCurrentDispatchPacket->GetPacketId();
	sProtocol *protocol = protocols::GetProtocol(packetID);
	std::stringstream ss;
	ss << msg << network::Packet2String(*m_pCurrentDispatchPacket, protocol);
	common::log::Log( (common::log::LOG_TYPE)logType, ss.str() );
}
