#pragma once

#include "../../../../Common/Graphic/character/character.h"
#include "../../../../Common/Graphic/character/archebladecharacter.h"
#include "../../../../Common/Graphic/character/teracharacter.h"
#include "../../../../Common/Graphic/character/sc2character.h"
#include "../../../../Common/Graphic/character/characteranalyzer.h"


class CViewer2Dlg;

class cController : public common::cObservable2
							, public common::cSingleton<cController>
{
public:
	cController();
	virtual ~cController();

	bool LoadFile( const string &fileName );
	graphic::cCharacter* GetCharacter();
	graphic::cCharacterAnalyzer *GetCharacterAnalyzer();
	const string& GetCurrentMeshFileName();
	const string& GetCurrentAnimationFileName();

	void Render();
	//void RenderShader(graphic::cShader &shader);
	void Update(const float elapseT);
	void SetAnimationPlay(const bool isPlay);
	bool IsAnimationPlay() const;
	void SetCurrentAnimationFrame(const int curFrame);

	void ChangePanel(const int panelIdx);
	EDIT_MODE::TYPE GetCurrentPanel() const;
	void SendUpdate(const int type=0);

	void SetViewerDlg(CViewer2Dlg *dlg);
	CViewer2Dlg* GetViewerDlg();


public:
	graphic::cRenderer m_renderer;

	graphic::cCharacterAnalyzer *m_analyzer;
	graphic::cArchebladeCharacter m_archeCharacter; // 아키블래이드 캐릭터.
	graphic::cTeraCharacter m_teraCharacter; // 테라 캐릭터.
	graphic::cSc2Character m_sc2Character; // 스타크래프트2 캐릭터.
	graphic::cText m_modelName;
	graphic::cText m_animationName;
	graphic::cText m_shaderName;

	string m_currentMeshFileName;
	string m_currentAnimationFileName;
	bool m_isPlay;

	EDIT_MODE::TYPE m_currentMode;
	CViewer2Dlg *m_viewerDlg;
};


inline graphic::cCharacter* cController::GetCharacter() { return m_analyzer->GetCharacter(); }
inline graphic::cCharacterAnalyzer* cController::GetCharacterAnalyzer() { return m_analyzer; }
inline const string& cController::GetCurrentMeshFileName() { return m_currentMeshFileName; }
inline const string& cController::GetCurrentAnimationFileName() { return m_currentAnimationFileName; }
inline void cController::SetAnimationPlay(const bool isPlay) { m_isPlay = isPlay; }
inline bool cController::IsAnimationPlay() const { return m_isPlay; }
inline void cController::SetViewerDlg(CViewer2Dlg *dlg) { m_viewerDlg = dlg; }
inline CViewer2Dlg* cController::GetViewerDlg() { return m_viewerDlg; }
inline EDIT_MODE::TYPE cController::GetCurrentPanel() const { return m_currentMode; }
