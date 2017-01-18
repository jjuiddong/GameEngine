//------------------------------------------------------------------------
// Name:    Room.h
// Author:  jjuiddong
// Date:    12/21/2012
// 
// 방정보를 정의한다.
//------------------------------------------------------------------------
#pragma once

namespace network
{
	class CRoom
	{
	public:
		CRoom();
		virtual ~CRoom();

		netid 			GetId() const;
		const std::string& GetName() const;
		void				SetNumber(int number);
		void				SetName(const std::string &name );
		void				SetId(netid id);

		bool				AddGroup( CGroup *pGroup );
		bool				RemoveGroup( netid groupId );
		const CGroup& GetRootGroup() const;
		GroupPtr		GetGroup(netid groupId );

		bool				AddUser(netid groupId, netid userId);
		bool				RemoveUser(netid groupId, netid userId);
		bool				IsUserExist(netid userId);
		void				Clear();

	protected:
		netid 			m_Id;
		int				m_Number;	// 방번호
		std::string	m_Name;
		CGroup			m_RootGroup;
	};

	inline void	CRoom::SetNumber(int number) { m_Number = number; }
	inline void	CRoom::SetName(const std::string &name ) { m_Name = name; }
	inline void	CRoom::SetId(netid id) { m_Id = id; }
	inline netid CRoom::GetId() const { return m_Id; }
	inline const std::string& CRoom::GetName() const { return m_Name; }
	inline const CGroup& CRoom::GetRootGroup() const { return m_RootGroup; }

}
