// Viewer2 에 쓰이는 공용 타입들을 정의 한다.

#pragma once


const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
//const int WINDOW_WIDTH = 800;
//const int WINDOW_HEIGHT = 600;

const int REAL_WINDOW_WIDTH = WINDOW_WIDTH+18;
const int REAL_WINDOW_HEIGHT = WINDOW_HEIGHT+115;



namespace DISP_MODE
{
	enum TYPE
	{
		DISP_800X600_RIGHT,
		DISP_1024X768_RIGHT,
		DISP_800X600_LEFT,
		DISP_1024X768_LEFT,
	};
}

inline void GetViewSize(DISP_MODE::TYPE type, OUT int &width, OUT int &height)
{
	width = height = 0;
	switch (type)
	{
	case DISP_MODE::DISP_800X600_RIGHT: width = 800; height = 600; break; // 800X600 right align
	case DISP_MODE::DISP_1024X768_RIGHT: width = 1024; height = 768; break; // 1024X768 right align
	case DISP_MODE::DISP_800X600_LEFT: width = 800; height = 600; break; // 800X600 left align
	case DISP_MODE::DISP_1024X768_LEFT: width = 1024; height = 768; break; // 1024X768 left align
	default: return;
	}
}



namespace EDIT_MODE
{
	enum TYPE
	{
		FILE,
		MODEL,
		ANIMATION,
		RENDERER,
		LIGHT,
		ARCHEBLADE,
		TERA,
		SC2, // starcraft2
	};
}


namespace NOTIFY_MSG
{
	enum TYPE
	{
		UPDATE_MODEL, // 모델 파일이 교첼 될 때 발생.
		CHANGE_PANEL, // 패널이 이동 될 때 발생.
		UPDATE_LIGHT_DIRECTION, // light panel 에서 조명 방향 조정을 체크하고, 마우스로 방향을 지정할 때 발생함.
		UPDATE_TEXTURE, // model panel 에서 매터리얼 정보의 텍스쳐를 선택할 때 발생.
		UPDATE_SHADER, // renderer panel에서 셰이더를 선택했을 때 발생.
	};
}


