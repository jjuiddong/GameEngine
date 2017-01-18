/**
Name:   GlobalRemotePlayer.h
Author:  jjuiddong
Date:    5/2/2013

	전체 서버에서 동일하게 관리되는 클라이언트 객체
*/
#pragma once

class CGlobalRemotePlayer : public network::CSession
{
public:
	CGlobalRemotePlayer() {}
	virtual ~CGlobalRemotePlayer() {}
	void		SetLocateSvrType(const std::string &svrType);
	const std::string& GetLocateSvrType() const;
	void		SetLocateSvrId(const netid netId);
	netid	GetLocateSvrId() const;

protected:

private:
	netid				m_LocateSvrId;
	std::string		m_LocateSvrType;

};


inline void	CGlobalRemotePlayer::SetLocateSvrType(const std::string &svrType) { m_LocateSvrType = svrType; }
inline const std::string& CGlobalRemotePlayer::GetLocateSvrType() const { return m_LocateSvrType; }
inline void	CGlobalRemotePlayer::SetLocateSvrId(const netid netId) { m_LocateSvrId = netId; }
inline netid CGlobalRemotePlayer::GetLocateSvrId() const { return m_LocateSvrId; }

