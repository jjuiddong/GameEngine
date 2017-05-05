
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


void cRoot::Init(graphic::cRenderer &renderer)
{
	const float size = 100;
	m_ground.SetCube(renderer, Vector3(-size, -2, -size), Vector3(size, -1, size), 30.f);
	m_ground.SetShader(cResourceManager::Get()->LoadShader(renderer, "shader/cube3.fx"));
	m_ground.m_tex = cResourceManager::Get()->LoadTexture(renderer, "terrain/바닥.jpg");
	if (m_ground.m_shader)
		m_ground.m_shader->SetTechnique("Scene");

	Matrix44 waterPos;
	waterPos.SetPosition(Vector3(0, 0, 0));
	m_water.m_initInfo.toWorld = waterPos;
	m_water.m_initInfo.uvFactor = 200.f;
	m_water.Create(renderer);

	m_skybox.Create(renderer, cResourceManager::Get()->FindFile("grassenvmap1024.dds"), 10000);

	// ImGui Setting
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
	style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 0.80f);
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

	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("../Media/extra_fonts/Roboto-Medium.ttf", 16);
	}
}


void cRoot::Update(graphic::cRenderer &renderer, const float deltaSeconds)
{
	m_model.Update(renderer, deltaSeconds);

	if (m_dispControl.m_isWater)
		m_water.Update(deltaSeconds);

	if (m_model.IsLoadFinish())
	{
		m_model.SetAnimation(m_model.m_animationName);

		const float len = m_model.m_boundingBox.Length();
		if (len > 0)
		{
			const float scale = 10.f / len;
			Matrix44 S;
			S.SetScale(Vector3(1, 1, 1)*scale);
			m_model.m_tm = S;

			cBoundingBox bbox = m_model.m_boundingBox;
			bbox.m_max *= S;
			bbox.m_min *= S;
			GetMainCamera()->Move(bbox);
		}
	}

	m_dispControl.Update(deltaSeconds);
	m_modelControl.Update(deltaSeconds);
	m_skeletonControl.Update(deltaSeconds);
	m_aniControl.Update(deltaSeconds);
}


void cRoot::PreRender(graphic::cRenderer &renderer, const float deltaSeconds)
{
	RET(!m_dispControl.m_isWater);

	// Reflection plane in local space.
	Plane waterPlaneL(0, -1, 0, 0);

	// Reflection plane in world space.
	Matrix44 waterWorld;
	waterWorld.SetTranslate(Vector3(0, 0, 0)); // 실제 물의 높이는 10이지만, 컬링을 위해 20으로 높임
	Matrix44 WInvTrans;
	WInvTrans = waterWorld.Inverse();
	WInvTrans.Transpose();
	Plane waterPlaneW = waterPlaneL * WInvTrans;

	// Reflection plane in homogeneous clip space.
	Matrix44 WVPInvTrans = (waterWorld * cMainCamera::Get()->GetViewProjectionMatrix()).Inverse();
	WVPInvTrans.Transpose();
	Plane waterPlaneH = waterPlaneL * WVPInvTrans;

	float f[4] = { waterPlaneH.N.x, waterPlaneH.N.y, waterPlaneH.N.z, waterPlaneH.D };
	renderer.GetDevice()->SetClipPlane(0, (float*)f);
	renderer.GetDevice()->SetRenderState(D3DRS_CLIPPLANEENABLE, 1);

	m_water.BeginRefractScene();
	renderer.ClearScene();
	m_skybox.Render(renderer);
	RenderGeometry(renderer, deltaSeconds, Matrix44::Identity, false);
	m_water.EndRefractScene();

	// Seems like we need to reset these due to a driver bug.  It works
	// correctly without these next two lines in the REF and another 
	//video card, however.
	renderer.GetDevice()->SetClipPlane(0, (float*)f);
	renderer.GetDevice()->SetRenderState(D3DRS_CLIPPLANEENABLE, 1);
	renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	m_water.BeginReflectScene();
	renderer.ClearScene();
	Matrix44 reflectMatrix = waterPlaneW.GetReflectMatrix();
	m_skybox.Render(renderer, reflectMatrix);
	RenderGeometry(renderer, deltaSeconds, reflectMatrix, false);
	m_water.EndReflectScene();

	renderer.GetDevice()->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
	renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


void cRoot::Render(cRenderer &renderer, const float deltaSeconds)
{
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

	renderer.SetCullMode(D3DCULL_CCW);
	renderer.SetFillMode(D3DFILL_SOLID);

	if (m_dispControl.m_isSkybox)
		m_skybox.Render(renderer);

	RenderGeometry(renderer, deltaSeconds);

	if (m_dispControl.m_isWater)
		m_water.Render(renderer);
}


void cRoot::RenderGeometry(graphic::cRenderer &renderer, const float deltaSeconds
	, const Matrix44 &tm //= Matrix44::Identity
	, const bool isShadow //= true;
)
{
	D3DFILLMODE fillMode = renderer.GetFillMode();
	D3DCULL cullMode = renderer.GetCullMode();
	renderer.SetCullMode(m_dispControl.m_isBackfaceCulling ? cullMode : D3DCULL_NONE);
	renderer.SetFillMode(m_dispControl.m_isWireFrame ? D3DFILL_WIREFRAME : fillMode);

	if (isShadow)
	{
		if (m_ground.m_shader)
		{
			GetMainCamera()->Bind(*m_ground.m_shader);
			GetMainLight().Bind(*m_ground.m_shader);
			m_ground.m_shader->SetTechnique("Ambient");
			if (m_dispControl.m_isGround)
				m_ground.RenderShader(renderer, tm);
		}

		if (m_model.m_shader)
		{
			GetMainCamera()->Bind(*m_model.m_shader);
			GetMainLight().Bind(*m_model.m_shader);
			m_model.m_shader->SetTechnique("Ambient");
			m_model.RenderShader(renderer, tm);
		}

		renderer.GetDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0, 170, 170, 170), 1.0f, 0);
		if (m_model.m_shader && m_dispControl.m_isShadow)
		{
			m_model.m_shader->SetTechnique("Shadow");
			m_model.RenderShadow(renderer, tm);
		}

		if (m_model.m_shader)
		{
			m_model.m_shader->SetTechnique("Scene");
			m_model.RenderShader(renderer, tm);
		}

		if (m_ground.m_shader)
		{
			m_ground.m_shader->SetTechnique("Scene");
			if (m_dispControl.m_isGround)
				m_ground.RenderShader(renderer, tm);
		}
	}
	else
	{
		// No Shadow Rendering
		if (m_ground.m_shader)
		{
			GetMainCamera()->Bind(*m_ground.m_shader);
			GetMainLight().Bind(*m_ground.m_shader);
			m_ground.m_shader->SetTechnique("Scene_NoShadow");
			if (m_dispControl.m_isGround)
				m_ground.RenderShader(renderer, tm);
		}

		if (m_model.m_shader)
		{
			GetMainCamera()->Bind(*m_model.m_shader);
			GetMainLight().Bind(*m_model.m_shader);
			m_model.m_shader->SetTechnique("Scene_NoShadow");
			m_model.RenderShader(renderer, tm);
		}
	}

	renderer.SetFillMode(fillMode);
	renderer.SetCullMode(cullMode);
}


void cRoot::Clear()
{
	m_dispControl.Clear();
	m_modelControl.Clear();
	m_skeletonControl.Clear();
}


void cRoot::LostDevice()
{
	m_water.LostDevice();
}


void cRoot::ResetDevice(cRenderer &renderer)
{
	m_water.ResetDevice(renderer);
}


void cRoot::UpdateShader()
{
	m_water.UpdateShader();
}
