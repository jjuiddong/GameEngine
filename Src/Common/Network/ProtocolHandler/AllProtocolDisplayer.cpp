
#include "stdafx.h"
#include "AllProtocolDisplayer.h"

using namespace network;

AllProtocolDisplayer::AllProtocolDisplayer(int displayType) : 
	m_DisplayType(displayType)
{
}


/**
 @brief recv
 */
void AllProtocolDisplayer::recv(netid senderId, network::CPacket &packet)
{
	protocols::DisplayPacket("Recv =", packet);
	//const int protocolID = packet.GetProtocolId();
	//const int packetID = packet.GetPacketId();
	//sProtocol *protocol = protocols::GetProtocol(packetID);
	//std::stringstream ss;
	//ss << "Recv = ";
	//ss << network::Packet2String(packet, protocol);
	//clog::Log( clog::LOG_F_N_O, clog::LOG_PACKET, packetID, ss.str() );
}
