/**
Name:   Player.h
Author:  jjuiddong
Date:    12/28/2012

 유저정보를 저장하는 클래스다.
*/
#pragma once

namespace network
{
	class CCharacter;
	class CPlayer
	{
	public:
		CPlayer();
		virtual ~CPlayer();

		void			SetState(PLAYER_STATE state);
		void			SetNetId(netid netId);
		void			SetCertifyKey(certify_key key);
		void			SetId(int id);
		void			SetName(const std::string &name);

		netid		GetNetId() const;
		certify_key GetCertifyKey() const;
		int			GetId() const;
		const std::string& GetName() const;
		bool			IsAckWait() const;

		bool			AddCharacter(CharacterPtr pChar);
		bool			RemoveCharacter(int characterId);
		CharacterPtr	GetCharacter(int characterId);
		CharacterPtr	GetSelectChracter();

	private:
		PLAYER_STATE	m_State;
		netid				m_NetId;
		certify_key		m_CertifyKey;
		int					m_Id;
		std::string		m_Name;

		typedef std::map<int,CCharacter*> CharacterMap;
		typedef CharacterMap::iterator CharacterItor;
		CharacterMap		m_CharacterMap;
		int					m_SelectCharacterId;

	};


	inline void	CPlayer::SetNetId(netid netId) { m_NetId = netId; }
	inline void	CPlayer::SetId(int id) { m_Id = id; }
	inline void	CPlayer::SetName(const std::string &name) { m_Name = name; }
	inline netid CPlayer::GetNetId() const { return m_NetId; }
	inline int CPlayer::GetId() const { return m_Id; }
	inline const std::string& CPlayer::GetName() const { return m_Name; }
	inline certify_key CPlayer::GetCertifyKey() const { return m_CertifyKey; }
	inline void			CPlayer::SetCertifyKey(certify_key key) { m_CertifyKey = key; }
	inline void CPlayer::SetState( PLAYER_STATE state) { m_State = state; }		 
	inline bool CPlayer::IsAckWait() const { return m_State == USERSTATE_WAIT_ACK; }

}
