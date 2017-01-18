// Viewer2 �� ���̴� ���� Ÿ�Ե��� ���� �Ѵ�.

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
		UPDATE_MODEL, // �� ������ ��ÿ �� �� �߻�.
		CHANGE_PANEL, // �г��� �̵� �� �� �߻�.
		UPDATE_LIGHT_DIRECTION, // light panel ���� ���� ���� ������ üũ�ϰ�, ���콺�� ������ ������ �� �߻���.
		UPDATE_TEXTURE, // model panel ���� ���͸��� ������ �ؽ��ĸ� ������ �� �߻�.
		UPDATE_SHADER, // renderer panel���� ���̴��� �������� �� �߻�.
	};
}


