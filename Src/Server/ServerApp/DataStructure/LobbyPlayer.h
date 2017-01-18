//------------------------------------------------------------------------
// Name:    LobbyPlayer.h
// Author:  jjuiddong
// Date:    12/29/2012
// 
// 로비 서버에 있는 유저를 정의한다.
//------------------------------------------------------------------------
#pragma once

DECLARE_TYPE_NAME(CLobbyPlayer)
class CLobbyPlayer : public network::CPlayer
			, public memmonitor::Monitor<CLobbyPlayer, TYPE_NAME(CLobbyPlayer)>
{
public:
	enum REQ_STATE
	{
		REQ_NONE,
		REQ_CREATE_GROUP,
		REQ_JOIN_GROUP,
	};

	CLobbyPlayer();
	virtual ~CLobbyPlayer();
	void			SetRequestState(REQ_STATE);
	REQ_STATE GetRequestState() const;

protected:

private:
	REQ_STATE		m_RequestState;

};


inline void	CLobbyPlayer::SetRequestState(CLobbyPlayer::REQ_STATE state) { m_RequestState = state; }
inline CLobbyPlayer::REQ_STATE CLobbyPlayer::GetRequestState() const { return m_RequestState; }


/// CLobbyPlayer Factory
class CLobbyPlayerFactory : public network::IPlayerFactory
{
public:
	virtual network::CPlayer* New() { return new CLobbyPlayer(); }
	virtual network::IPlayerFactory* Clone() { return new CLobbyPlayerFactory(); }
};

