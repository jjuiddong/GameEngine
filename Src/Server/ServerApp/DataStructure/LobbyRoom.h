//------------------------------------------------------------------------
// Name:    LobbyRoom.h
// Author:  jjuiddong
// Date:    12/29/2012
// 
// 로비서버에 존재하는 방정보를 정의한다.
//------------------------------------------------------------------------
#pragma once

DECLARE_TYPE_NAME(CLobbyRoom)
class CLobbyRoom : public network::CRoom
			, public memmonitor::Monitor<CLobbyRoom, TYPE_NAME(CLobbyRoom)>
{
public:
	CLobbyRoom() {}
	virtual ~CLobbyRoom() {}

protected:

public:

};
