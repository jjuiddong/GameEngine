/**
Name:   Group.h
Author:  jjuiddong
Date:    12/29/2012

 그룹을 관리하는 그룹, 여러명의 유저를 관리한다.
 소멸자에서 자식으로 가지는 그룹들을 모두 제거한다.
*/
#pragma once

namespace network { 
	class CPacket;
	class CGroup;
	namespace marshalling {
		CPacket& operator<<(CPacket& packet, const CGroup &rhs);
		CPacket& operator>>(CPacket& packet, CGroup &rhs);
}}


namespace network
{
	class IGroupFactory;

	class CGroup
	{
		friend CPacket& (marshalling::operator<<(CPacket& packet, const CGroup &rhs));
		friend CPacket& (marshalling::operator>>(CPacket& packet, CGroup &rhs));

	public:
		enum NET_STATE
		{
			NET_STATE_P2P,
			NET_STATE_SERVERCLIENT,
		};

		CGroup(GroupPtr parent=NULL, const std::string &name="");
		CGroup(const CGroup &rhs);
		virtual ~CGroup();

		// Group
		bool				AddChild( CGroup *pGroup );
		CGroup*		AddChild( IGroupFactory *pFactory );
		bool				RemoveChild( netid groupId );
		GroupPtr		GetChild( netid groupId );
		GroupPtr		GetChildandThis( netid groupId );
		GroupPtr		GetChildFromPlayer( netid playerId );
		const Groups::VectorType&	GetChildren() const;
		bool				IsTerminal();

		// playerId
		bool				AddPlayer(netid groupId, netid playerId);
		bool				RemovePlayer(netid groupId, netid playerId);
		bool				IsExistPlayer(netid groupId, netid playerId);
		bool				IsExistPlayer(netid playerId);
		const NetIdes& GetPlayers() const;
		void				Clear();

		// Viewer
		bool				AddViewer(netid id);
		bool				RemoveViewer(netid id);
		const NetIdes& GetViewers() const;

		bool operator==(const CGroup &rhs) const;
		CGroup& operator=(const CGroup &rhs);

		void							SetNetId(netid groupId);
		netid						GetNetId() const;
		const std::string&	GetName() const;
		void							SetName(const std::string &name);		
		DWORD					GetTag() const;
		void							SetTag(DWORD tag);
		CGroup*					GetParent() const;
		void							SetParent(GroupPtr parent);
		netid						GetParentId() const;
		NET_STATE				GetNetState() const;
		void							SetNetState(NET_STATE state);

	protected:
		bool				AddPlayer(netid playerId);
		bool				AddPlayerNApplyParent(GroupPtr pGroup, netid playerId);
		bool				RemovePlayer(netid playerId);
		bool				RemovePlayerNApplyParent(GroupPtr pGroup, netid playerId);

	private:
		netid			m_Id;
		netid			m_ParentId;
		std::string	m_Name;
		DWORD		m_Tag;
		NET_STATE	m_NetState;
		CGroup			*m_pParent;
		NetIdes		m_Players;
		NetIdes		m_Viewers;
		Groups			m_Children;
	};

	inline void	CGroup::SetNetId(netid groupId) { m_Id = groupId; }
	inline netid CGroup::GetNetId() const { return m_Id; }
	inline const std::string& CGroup::GetName() const { return m_Name; }
	inline void CGroup::SetName(const std::string &name) { m_Name = name; }
	inline CGroup* CGroup::GetParent() const { return m_pParent; }
	inline netid CGroup::GetParentId() const { return m_ParentId; }
	inline void CGroup::SetParent(GroupPtr parent) { m_pParent = parent; if(parent) {m_ParentId = parent->GetNetId();} }
	inline DWORD CGroup::GetTag() const { return m_Tag; }
	inline void CGroup::SetTag(DWORD tag) { m_Tag = tag; }
	inline CGroup::NET_STATE CGroup::GetNetState() const { return m_NetState; }
	inline void CGroup::SetNetState(NET_STATE state) { m_NetState = state; }
	inline const Groups::VectorType& CGroup::GetChildren() const { return m_Children.m_Seq; }
	inline const NetIdes& CGroup::GetPlayers() const { return m_Players; }
	inline const NetIdes& CGroup::GetViewers() const { return m_Viewers; }
	inline bool CGroup::operator==(const CGroup &rhs) const { return m_Id==rhs.GetNetId(); }
}
