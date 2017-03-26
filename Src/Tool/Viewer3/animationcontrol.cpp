
#include "stdafx.h"
#include "animationcontrol.h"

using namespace graphic;

cAnimationControl::cAnimationControl()
{
}

cAnimationControl::~cAnimationControl()
{
}


void cAnimationControl::Update(const float deltaSeconds)
{

}


void cAnimationControl::Render(cRenderer &renderer, const float deltaSeconds)
{
	ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_FirstUseEver);
	if (ImGui::CollapsingHeader("Animation"))
	{
		ImGui::Text("Animation Name : %s", g_root.m_model.m_storedAnimationName.c_str());
		ImGui::Text("Start : %f (seconds)", g_root.m_model.m_animation.m_start);
		ImGui::Text("End : %f (seconds)", g_root.m_model.m_animation.m_end);

		const float t = g_root.m_model.m_animation.m_incTime;
		const float s = g_root.m_model.m_animation.m_start;
		const float e = g_root.m_model.m_animation.m_end;
		const float d = e - s;
		const float progress = t / d;
		float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
		char buf[32];
		sprintf(buf, "%f/%f", progress_saturated * d, d);
		ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);

		if (ImGui::Button("Play"))
		{
			g_root.m_model.m_animation.Play();
		}

		ImGui::SameLine(); 

		if (ImGui::Button("Stop"))
		{
			g_root.m_model.m_animation.Stop();
		}
	}
}


void cAnimationControl::Clear()
{
}
