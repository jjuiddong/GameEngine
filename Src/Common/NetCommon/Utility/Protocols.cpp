
#include "stdafx.h"
#include "Protocols.h"

namespace network { namespace protocols {

	void InsertRmi( sRmi *pRmi );
	void InsertProtocol( sRmi *pRmi, sProtocol *pProtocol );

	typedef std::map<int,sProtocol*> Protocols;		// key = packetID
	typedef Protocols::iterator ProtocolItor;

	Protocols g_Protocols;
}}

using namespace network;
using namespace protocols;


/**
 @brief Insert Rmi
 */
void protocols::InsertRmi( sRmi *pRmi )
{
	RET(!pRmi);
	InsertProtocol( pRmi, pRmi->protocol );
	InsertRmi( pRmi->next );
}


/**
 @brief Insert  Protocol
 */
void protocols::InsertProtocol( sRmi *pRmi, sProtocol *pProtocol )
{
	RET(!pProtocol);
	const int packetID = network::GetPacketID(pRmi, pProtocol);
	g_Protocols.insert( Protocols::value_type(packetID, pProtocol) );
	InsertProtocol( pRmi, pProtocol->next );
}


/**
 @brief Init
 */
void	protocols::Init()
{
	std::string protocolDir = "../Src/Common/NetProtocol/*.prt";
	std::list<std::string> fileList = common::FindFileList( protocolDir );
	BOOST_FOREACH(std::string &str, fileList)
	{
		CProtocolParser parser;
		parser.SetAutoRemove(FALSE);

		sRmi *rmiList = parser.Parse( str.c_str() );
		InsertRmi( rmiList );
		ReleaseRmiOnly( rmiList );
	}
}


/**
 @brief Cleanup
 */
void	protocols::Cleanup()
{
	BOOST_FOREACH(auto prt, g_Protocols)
	{
		ReleaseCurrentProtocol(prt.second);
	}
	g_Protocols.clear();
}


/**
 @brief GetProtocol
 */
sProtocol* protocols::GetProtocol(int packetId)
{
	auto it = g_Protocols.find(packetId);
	if (g_Protocols.end() == it)
		return NULL;
	return it->second;
}


/**
@brief  DisplayPacket
*/
void	protocols::DisplayPacket( const std::string &firstStr, const CPacket &packet )
{
	const int protocolID = packet.GetProtocolId();
	const int packetID = packet.GetPacketId();
	sProtocol *protocol = GetProtocol(packetID);
	std::stringstream ss;
	ss << firstStr;
	ss << network::Packet2String(packet, protocol);
	clog::Log( clog::LOG_F_N_O, clog::LOG_PACKET, packetID, ss.str() );
}
