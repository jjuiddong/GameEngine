
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
		cColladaModel *model = g_root.m_model.m_colladaModel;
		if (!model)
			return;

		ImGui::Text("Animation Name : %s", model->m_storedAnimationName.c_str());
		ImGui::Text("Start : %f (seconds)", model->m_animation.m_start);
		ImGui::Text("End : %f (seconds)", model->m_animation.m_end);

		const float t = model->m_animation.m_incTime;
		const float s = model->m_animation.m_start;
		const float e = model->m_animation.m_end;
		const float d = e - s;
		const float progress = t / d;
		float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
		char buf[32];
		sprintf(buf, "%f/%f", progress_saturated * d, d);
		ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);

		if (ImGui::Button("Play"))
		{
			model->m_animation.Play();
		}

		ImGui::SameLine(); 

		if (ImGui::Button("Stop"))
		{
			model->m_animation.Stop();
		}
	}
}


void cAnimationControl::Clear()
{
}
