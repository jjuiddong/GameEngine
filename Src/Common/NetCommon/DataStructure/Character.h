/**
Name:    Character.h
Author:  jjuiddong
Date:    12/28/2012

 유저가 소유한 캐릭터 정보를 관리한다.
*/
#pragma once

namespace network
{
	class CCharacter
	{
	public:
		CCharacter() {}
		virtual ~CCharacter() {}

	protected:
		int					m_Id;		// Character 고유아이디
		std::string		m_Name;

	public:
		int					GetId() const { return m_Id; }
		const std::string&	GetName() const { return m_Name; }

	};
}
