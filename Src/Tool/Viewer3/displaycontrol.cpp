
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
		ImGui::Checkbox("WireFrame", &m_isWireFrame); ImGui::SameLine(150);
		ImGui::Checkbox("Backface Culling", &m_isBackfaceCulling); 
		ImGui::Checkbox("Ground ", &m_isGround); ImGui::SameLine(150);
		ImGui::Checkbox("Water", &m_isWater);
		ImGui::Checkbox("Shadow", &m_isShadow); ImGui::SameLine(150);
		ImGui::Checkbox("SkyBox", &m_isSkybox);
		
		//ImGui::SameLine(150);
		ImGui::Spacing(); ImGui::Spacing();
	}
}


void cDisplayControl::Clear()
{

}
