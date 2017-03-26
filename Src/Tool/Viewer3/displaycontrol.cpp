
#include "stdafx.h"
#include "displaycontrol.h"

using namespace graphic;


cDisplayControl::cDisplayControl()
{
}

cDisplayControl::~cDisplayControl()
{
}


void cDisplayControl::Update(const float deltaSeconds)
{

}


void cDisplayControl::Render(cRenderer &renderer, const float deltaSeconds)
{
	ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_FirstUseEver);
	if (ImGui::CollapsingHeader("Display Option"))
	{
		ImGui::Checkbox("WireFrame", &m_wireFrame); ImGui::SameLine(150);
		ImGui::Checkbox("Backface Culling", &m_backfaceCulling); 
		//ImGui::SameLine(150);
		ImGui::Spacing(); ImGui::Spacing();
	}
}


void cDisplayControl::Clear()
{

}
