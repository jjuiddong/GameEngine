//------------------------------------------------------------------------
// Name:    LobbyRoom.h
// Author:  jjuiddong
// Date:    12/29/2012
// 
// �κ񼭹��� �����ϴ� �������� �����Ѵ�.
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
