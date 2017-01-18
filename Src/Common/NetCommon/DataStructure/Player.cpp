
#include "stdafx.h"
#include "Player.h"
#include "Character.h"

using namespace network;


CPlayer::CPlayer() : 
	m_SelectCharacterId(-1)
,	m_State(USERSTATE_LISTEN)
{

}

CPlayer::~CPlayer() 
{

}


//------------------------------------------------------------------------
// ĳ���� �߰�
//------------------------------------------------------------------------
bool CPlayer::AddCharacter(CharacterPtr pChar)
{
	CharacterItor it = m_CharacterMap.find(pChar->GetId());
	if (m_CharacterMap.end() != it)
		return false;// �̹� �����ϸ� ����

	m_CharacterMap.insert( CharacterMap::value_type(pChar->GetId(), pChar) );
	return true;
}


//------------------------------------------------------------------------
// ĳ���� ����
//------------------------------------------------------------------------
bool CPlayer::RemoveCharacter(int characterId)
{
	CharacterItor it = m_CharacterMap.find(characterId);
	if (m_CharacterMap.end() == it)
		return false;
	delete it->second;
	m_CharacterMap.erase(it);
	return true;
}


//------------------------------------------------------------------------
// ĳ���� ���� ����
//------------------------------------------------------------------------
CharacterPtr CPlayer::GetCharacter(int characterId)
{
	CharacterItor it = m_CharacterMap.find(characterId);
	if (m_CharacterMap.end() == it)
		return NULL;
	return it->second;
}


//------------------------------------------------------------------------
// ���õ� ĳ���� ����
//------------------------------------------------------------------------
CharacterPtr CPlayer::GetSelectChracter()
{
	CharacterItor it = m_CharacterMap.find(m_SelectCharacterId);
	if (m_CharacterMap.end() == it)
		return NULL;
	return it->second;
}
