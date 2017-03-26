
#include "stdafx.h"
#include "root.h"

using namespace graphic;

cRoot g_root;

cRoot::cRoot()
{
}

cRoot::~cRoot()
{
}


void cRoot::Init()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("../media/extra_fonts/Roboto-Medium.ttf", 16);

}


void cRoot::Update(const float deltaSeconds)
{
	m_model.Update(deltaSeconds);
	m_dispControl.Update(deltaSeconds);
	m_modelControl.Update(deltaSeconds);
	m_skeletonControl.Update(deltaSeconds);
	m_aniControl.Update(deltaSeconds);
}


void cRoot::Render(cRenderer &renderer, const float deltaSeconds)
{
	ImGui_ImplDX9_NewFrame();

	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Control"))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	m_dispControl.Render(renderer, deltaSeconds);
	m_modelControl.Render(renderer, deltaSeconds);
	m_skeletonControl.Render(renderer, deltaSeconds);
	m_aniControl.Render(renderer, deltaSeconds);
	ImGui::End();

	renderer.SetCullMode(m_dispControl.m_backfaceCulling ? D3DCULL_CCW : D3DCULL_NONE);
	renderer.SetFillMode(m_dispControl.m_wireFrame ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	m_model.Render(renderer);
	renderer.SetFillMode(D3DFILL_SOLID);
	renderer.SetCullMode(D3DCULL_CCW);
}


void cRoot::Clear()
{
	m_dispControl.Clear();
	m_modelControl.Clear();
	m_skeletonControl.Clear();
}
