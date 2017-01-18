/**
Name:    Character.h
Author:  jjuiddong
Date:    12/28/2012

 ������ ������ ĳ���� ������ �����Ѵ�.
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
		int					m_Id;		// Character �������̵�
		std::string		m_Name;

	public:
		int					GetId() const { return m_Id; }
		const std::string&	GetName() const { return m_Name; }

	};
}
