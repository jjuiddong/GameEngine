
#include "stdafx.h"
#include "Controller.h"

using namespace graphic;


cController::cController() : 
	m_analyzer(NULL)
,	m_isPlay(true)
,	m_archeCharacter(common::GenerateId())
,	m_sc2Character(common::GenerateId())
,	m_viewerDlg(NULL)
,	m_modelName("modelName: ", 10, 27)
,	m_animationName("animationName: ", 10, 44)
,	m_shaderName("shaderName: ", 10, 61)
,	m_currentMode(EDIT_MODE::FILE)
{
	m_analyzer = new cCharacterAnalyzer();
	m_analyzer->SetCharacter(&m_archeCharacter);
}

cController::~cController()
{
	SAFE_DELETE(m_analyzer);
}


bool cController::LoadFile( const string &fileName )
{
	cCharacter *character = GetCharacter();
	RETV(!character, false);

	ShowLoadingDialog();

	bool result = false;
	const RESOURCE_TYPE::TYPE type = cResourceManager::Get()->GetFileKind(fileName);
	switch (type)
	{
	case RESOURCE_TYPE::MESH:
		m_currentMeshFileName = fileName;
		if (result = character->Create(fileName))
		{
			m_modelName.SetText("model: " + common::GetFileName(fileName));
			m_animationName.SetText("animation: ");
			if (character->GetShader())
				m_shaderName.SetText("shader: " + common::GetFileName(character->GetShader()->GetFileName()));

			// 모델 크기에 따라 조명의 위치를 조절한다.
			// 그림자 크기를 조정하기 위해서.
			const Vector3 lightPos = Vector3(1,1,-1) * character->GetCollisionBox()->Length() * 4;
			cLightManager::Get()->GetMainLight().SetPosition(lightPos);
		}
		else
		{
			goto error;
		}
		break;

	case RESOURCE_TYPE::ANIMATION:
		m_currentAnimationFileName = fileName;
		if (result = character->SetAnimation(fileName, 0, true))
		{
			m_animationName.SetText("animation: " + common::GetFileName(fileName));
		}
		else
		{
			goto error;
		}
		break;

	default:
		break;
	}

	NotifyObserver(NOTIFY_MSG::UPDATE_MODEL);

	HideLoadingDialog();
	return result;

error:
	HideLoadingDialog();
	return false;
}


void cController::Render()
{
	RET(!m_analyzer);
	m_analyzer->Render(Matrix44::Identity);
	m_modelName.Render();
	m_animationName.Render();
	m_shaderName.Render();
}


//void cController::RenderShader(graphic::cShader &shader)
//{
//	RET(!m_character);
//	m_character->RenderShader(shader);
//}


void cController::Update(const float elapseT)
{
	RET(!m_analyzer);

	if (m_isPlay)
		m_analyzer->Move(elapseT);
	else
		m_analyzer->Move(0);
}


void cController::SetCurrentAnimationFrame(const int curFrame)
{
	cCharacter *character = GetCharacter();
	RET(!character);

	cBoneMgr *boneMgr = character->GetBoneMgr();
	RET(!boneMgr);
	boneMgr->SetCurrentAnimationFrame(curFrame);
}


// 패널이 전환 될 때 호출된다.
void cController::ChangePanel(const int panelIdx)
{
	EDIT_MODE::TYPE type = (EDIT_MODE::TYPE)panelIdx;
	m_currentMode = type;

	switch (type)
	{
	case EDIT_MODE::FILE: // file panel
		m_analyzer->SetCharacter(&m_archeCharacter);
		break;

	case EDIT_MODE::MODEL: // model panel
		break;

	case EDIT_MODE::ANIMATION: // animation panel
		break;

	case EDIT_MODE::LIGHT:
		break;

	case EDIT_MODE::ARCHEBLADE: // archeblade panel
		m_analyzer->SetCharacter(&m_archeCharacter);
		break;

	case EDIT_MODE::TERA: // tera panel
		m_analyzer->SetCharacter(&m_teraCharacter);
		break;

	case EDIT_MODE::SC2:
		m_analyzer->SetCharacter(&m_sc2Character);
		break;
	}

	NotifyObserver(NOTIFY_MSG::CHANGE_PANEL);
}


// observer 에게 업데이트 메세지를 보낸다.
void cController::SendUpdate(const int type) //type=0
{
	if (NOTIFY_MSG::UPDATE_SHADER == type)
	{
		if (cCharacter *character = GetCharacter())
			if (character->GetShader())
				m_shaderName.SetText("shader: " + common::GetFileName(character->GetShader()->GetFileName()));
	}

	NotifyObserver(type);
}
