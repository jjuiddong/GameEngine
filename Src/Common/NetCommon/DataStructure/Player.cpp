
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
// 캐릭터 추가
//------------------------------------------------------------------------
bool CPlayer::AddCharacter(CharacterPtr pChar)
{
	CharacterItor it = m_CharacterMap.find(pChar->GetId());
	if (m_CharacterMap.end() != it)
		return false;// 이미 존재하면 실패

	m_CharacterMap.insert( CharacterMap::value_type(pChar->GetId(), pChar) );
	return true;
}


//------------------------------------------------------------------------
// 캐릭터 제거
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
// 캐릭터 정보 얻음
//------------------------------------------------------------------------
CharacterPtr CPlayer::GetCharacter(int characterId)
{
	CharacterItor it = m_CharacterMap.find(characterId);
	if (m_CharacterMap.end() == it)
		return NULL;
	return it->second;
}


//------------------------------------------------------------------------
// 선택된 캐릭터 리턴
//------------------------------------------------------------------------
CharacterPtr CPlayer::GetSelectChracter()
{
	CharacterItor it = m_CharacterMap.find(m_SelectCharacterId);
	if (m_CharacterMap.end() == it)
		return NULL;
	return it->second;
}
