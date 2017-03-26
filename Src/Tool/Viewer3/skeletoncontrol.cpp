
#include "stdafx.h"
#include "skeletoncontrol.h"

using namespace graphic;


cSkeletonControl::cSkeletonControl()
{
}

cSkeletonControl::~cSkeletonControl()
{
}


void cSkeletonControl::Update(const float deltaSeconds)
{

}


void cSkeletonControl::Render(cRenderer &renderer, const float deltaSeconds)
{
	ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_FirstUseEver);
	if (ImGui::CollapsingHeader("Skeleton"))
	{

	}
}


void cSkeletonControl::Clear()
{

}
