//
// 2017-03-28, jjuiddong
// multi window directx device creation
//
#include "stdafx.h"
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "sfml/Window.hpp"
#include "sfml/Graphics/RenderWindow.hpp"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "sfml-system-d.lib")
#pragma comment(lib, "sfml-window-d.lib")
#pragma comment(lib, "sfml-graphics-d.lib")


struct sWnd
{
	string name;
	sf::Window *window;
	cImGui gui;
	LPDIRECT3DDEVICE9 dev;
};

vector<sWnd *> m_views;

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void init_0(sf::Window *window, UINT Adapter, LPDIRECT3DDEVICE9 *pdevice);
void render(const int idx, const string &title, sf::Window *window, LPDIRECT3DDEVICE9 pdevice, cImGui &imGui);
void ResetDevice(HWND hWnd, LPDIRECT3DDEVICE9 pdevice);


void NewWindow()
{
	static int idx = 1;

	sWnd *wnd1 = new sWnd;
	wnd1->name = format("test#%d", idx++);
	wnd1->window = new sf::Window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), wnd1->name.c_str());
	m_views.push_back(wnd1);
	init_0(wnd1->window, D3DADAPTER_DEFAULT, &wnd1->dev);

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
	wnd1->dev->SetTransform(D3DTS_PROJECTION, &matProj);
	wnd1->dev->SetRenderState(D3DRS_ZENABLE, TRUE);
	wnd1->dev->SetRenderState(D3DRS_LIGHTING, FALSE);
	wnd1->dev->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	wnd1->gui.Init(wnd1->window->getSystemHandle(), wnd1->dev);
	wnd1->gui.SetContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("extra_fonts/Roboto-Medium.ttf", 24);
}


void SFEvent2WinMsg(const sf::Event &evt, cImGui &gui)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (evt.type)
	{
	case sf::Event::KeyPressed:
		if (evt.key.code < 256)
			io.KeysDown[evt.key.code] = 1;
		break;

	case sf::Event::MouseMoved:
		io.MousePos.x = (float)evt.mouseMove.x;
		io.MousePos.y = (float)evt.mouseMove.y;
		break;

	case sf::Event::MouseButtonPressed:
		switch (evt.mouseButton.button)
		{
		case sf::Mouse::Left: io.MouseDown[0] = true; break;
		case sf::Mouse::Right: io.MouseDown[1] = true; break;
		case sf::Mouse::Middle: io.MouseDown[2] = true; break;
		}
		break;

	case sf::Event::MouseButtonReleased:
		switch (evt.mouseButton.button)
		{
		case sf::Mouse::Left: io.MouseDown[0] = false; break;
		case sf::Mouse::Right: io.MouseDown[1] = false; break;
		case sf::Mouse::Middle: io.MouseDown[2] = false; break;
		}
		break;

	//case sf::Event::MouseWheel:
	//	break;
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	NewWindow();
	NewWindow();

	sWnd *wnd1 = m_views[0];
	ResetDevice(wnd1->window->getSystemHandle(), wnd1->dev);

	float col_main_hue = 0.0f / 255.0f;
	float col_main_sat = 0.0f / 255.0f;
	float col_main_val = 80.0f / 255.0f;

	float col_area_hue = 0.0f / 255.0f;
	float col_area_sat = 0.0f / 255.0f;
	float col_area_val = 50.0f / 255.0f;

	float col_back_hue = 0.0f / 255.0f;
	float col_back_sat = 0.0f / 255.0f;
	float col_back_val = 35.0f / 255.0f;

	float col_text_hue = 0.0f / 255.0f;
	float col_text_sat = 0.0f / 255.0f;
	float col_text_val = 255.0f / 255.0f;
	float frameRounding = 0.0f;

	ImVec4 col_text = ImColor::HSV(col_text_hue, col_text_sat, col_text_val);
	ImVec4 col_main = ImColor::HSV(col_main_hue, col_main_sat, col_main_val);
	ImVec4 col_area = ImColor::HSV(col_area_hue, col_area_sat, col_area_val);
	ImVec4 col_back = ImColor::HSV(col_back_hue, col_back_sat, col_back_val);
	float rounding = frameRounding;

	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = rounding;
	style.WindowRounding = rounding;
	style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(col_text.x, col_text.y, col_text.z, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.54f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.10f, 0.10f, 0.10f, 0.55f);

	bool loop = true;
	while (loop)
	{
		loop = false;
		bool addWnd = false;
		sWnd *rmWnd = NULL;
		for (auto &v : m_views)
		{
			v->gui.SetContext();

			sf::Event event;
			while (v->window->isOpen() && v->window->pollEvent(event))
			{
				SFEvent2WinMsg(event, v->gui);

				if (event.type == sf::Event::Closed)
				{
					v->window->close();
					rmWnd = v;
				}

				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::Escape)
					{
						v->window->close();
						rmWnd = v;
					}
					if (event.key.code == sf::Keyboard::Return)
					{
						addWnd = true;
					}
				}
			}

			v->gui.NewFrame();
			render(0, v->name, v->window, v->dev, v->gui);

			if (v->window->isOpen())
				loop = true;
		}

		if (rmWnd)
		{
			rmWnd->gui.SetContext();
			rmWnd->gui.Shutdown();
			SAFE_RELEASE(rmWnd->dev);
			delete rmWnd->window;
			delete rmWnd;
			popvector2(m_views, rmWnd);
		}

		if (addWnd)
			NewWindow();
	}


	for (auto &v : m_views)
	{
		v->gui.Shutdown();
		SAFE_RELEASE(v->dev);
		delete v->window;
		delete v;
	}
	m_views.clear();

	return 0;
}


void init_0(sf::Window *window, UINT Adapter, LPDIRECT3DDEVICE9 *pdevice)
{
	LPDIRECT3D9 g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = true;
	d3dpp.hDeviceWindow = (HWND)window->getSystemHandle();
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH; // set the width of the buffer
	d3dpp.BackBufferHeight = SCREEN_HEIGHT; // set the height of the buffer
	d3dpp.BackBufferCount = 1;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	g_pD3D->CreateDevice(Adapter, D3DDEVTYPE_HAL,
		window->getSystemHandle(), 
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, pdevice);

	g_pD3D->Release();
}

void ResetDevice(HWND hWnd, LPDIRECT3DDEVICE9 pdevice)
{
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = true;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH; // set the width of the buffer
	d3dpp.BackBufferHeight = SCREEN_HEIGHT; // set the height of the buffer
	d3dpp.BackBufferCount = 1;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	pdevice->Reset(&d3dpp);
}

bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_col = ImColor(114, 144, 154);

void render(const int idx, const string &title, sf::Window *window, LPDIRECT3DDEVICE9 pdevice, cImGui &imGui)
{
	//ImGui::BeginChild(title.c_str());
	static bool open = true;
	//ImGui::Begin(title.c_str(), &open, ImVec2(800,800));
	//ImVec2 pos = ImGui::GetCursorPos();
	//static float f = 0.0f;
	//f += 0.0001f;
	//ImGui::Text("Hello, world!");
	//if (idx == 0)
	//{
	//	ImGui::SliderFloat("float1", &f, 0.0f, 1.0f);
	//}
	//else
	//{
	//	ImGui::SliderFloat("float2", &f, 0.0f, 1.0f);
	//}
	//ImGui::ColorEdit3("clear color", (float*)&clear_col);
	//if (ImGui::Button("Test Window")) show_test_window ^= 1;
	//if (ImGui::Button("Another Window")) show_another_window ^= 1;
	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	////ImGui::EndChild();
	//ImGui::End();
	std::string idname = title;
	ImVec2 calculated_size(400, 600);

	ImVec2 backup_pos = ImGui::GetCursorPos();

	ImGui::Begin(title.c_str(), &open, ImVec2(800,800));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 3.0f));
	//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 0.0f));
	ImVec4 childBg = ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg];
	ImVec4 button_color = ImVec4(childBg.x - 0.04f, childBg.y - 0.04f, childBg.z - 0.04f, childBg.w);
	ImVec4 buttonColorActive = ImVec4(childBg.x + 0.10f, childBg.y + 0.10f, childBg.z + 0.10f, childBg.w);
	ImVec4 buttonColorHovered = ImVec4(childBg.x + 0.15f, childBg.y + 0.15f, childBg.z + 0.15f, childBg.w);
	ImGui::PushStyleColor(ImGuiCol_Button, childBg);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, childBg);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, childBg);
	//ImGui::PushStyleColor(ImGuiCol_Button, button_color);
	//ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColorActive);
	//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColorHovered);
	if (ImGui::Button(idname.c_str(), ImVec2(0, 32)))
	{
		int a = 0;
		//container->active_dock = dock;
	}
	//ImGui::SameLine();
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();

	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clear_col);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		if (ImGui::Button("Another Window")) show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}


	ImGui::End();

	backup_pos.y += 32;
	ImGui::SetCursorPos(backup_pos);

	//ImGui::BeginChild(idname.c_str(), calculated_size, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
	//{
	//	static float f = 0.0f;
	//	ImGui::Text("Hello, world!");
	//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	//	ImGui::ColorEdit3("clear color", (float*)&clear_col);
	//	if (ImGui::Button("Test Window")) show_test_window ^= 1;
	//	if (ImGui::Button("Another Window")) show_another_window ^= 1;
	//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//}
	//ImGui::EndChild();

	pdevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
	pdevice->BeginScene();

	D3DXMATRIX matView;
	D3DXMATRIX matWorld;
	D3DXMATRIX matRotation;
	D3DXMATRIX matTranslation;
	D3DXMatrixIdentity(&matView);
	pdevice->SetTransform(D3DTS_VIEW, &matView);

	ImGui::Render();
	imGui.Render();

	pdevice->EndScene();
	pdevice->Present(NULL, NULL, NULL, NULL);
}
