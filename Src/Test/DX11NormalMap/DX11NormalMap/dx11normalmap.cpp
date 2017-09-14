//
// DX11 NormalMap
//

#include "stdafx.h"
#include "dx11normalmap.h"
#include "dockview.h"


using namespace graphic;
using namespace framework;

cRenderTarget g_shadowMap;
cCamera g_lightCamera;
cTexture g_unBindTex;

INIT_FRAMEWORK3(cViewer);
cViewer *g_viewer = NULL;


cViewer::cViewer()
{
	m_windowName = L"DX11 NormalMap";
	//const RECT r = { 0, 0, 1024, 768 };
	const RECT r = { 0, 0, (int)(1280 * 1.5f), (int)(1024 * 1.5f) };
	m_windowRect = r;
}

cViewer::~cViewer()
{
}


bool cViewer::OnInit()
{
	g_viewer = this;

	DragAcceptFiles(m_hWnd, TRUE);

	cResourceManager::Get()->SetMediaDirectory("../media/");

	m_gui.SetContext();

	cDockView1 *view1 = new cDockView1("Model View1");
	view1->Create(eDockState::DOCK, eDockSlot::TAB, this, NULL);
	view1->Init();

	cDockView1 *view2 = new cDockView1("Light View");
	view2->Create(eDockState::DOCK, eDockSlot::BOTTOM, this, view1);
	view2->m_isLightRender = true;
	view2->Init();

	cDockView1 *view3 = new cDockView1("DockView3");
	view3->Create(eDockState::DOCK, eDockSlot::RIGHT, this, view2);
	view3->Init();

	const int cx = GetSystemMetrics(SM_CXSCREEN);
	const int cy = GetSystemMetrics(SM_CYSCREEN);

	m_gui.SetContext();

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

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	const Vector3 lightPos(-20, 23, -20);
	const Vector3 lightLookat(0, 2, 0);
	GetMainLight().SetPosition(lightPos);
	GetMainLight().SetDirection((lightLookat - lightPos).Normal());

	g_lightCamera.Init(&m_renderer);
	g_lightCamera.SetCamera(lightPos, lightLookat, Vector3(0, 1, 0));
	g_lightCamera.SetProjectionOrthogonal(5, 5, 0.1f, 100.f);

	m_mtrl.InitWhite();

	cBoundingBox bbox2(Vector3(0, 0, 0), Vector3(10, 10, 10), Quaternion());
	m_axis.Create(m_renderer);
	m_axis.SetAxis(bbox2, false);

	m_model.Create(m_renderer, common::GenerateId(), "../media/boxlifter.x", "", "Unlit", true);
	//m_model2.Create(m_renderer, common::GenerateId(), "../media/Stormtrooper/Stormtrooper.dae", "", "Unlit", true);
	//m_model2.Create(m_renderer, common::GenerateId(), "../media/Lara_Croft/Lara_Croft.dae", "", "Unlit", true);
	m_ground.Create(m_renderer, 10, 10, 1, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE);

	Quaternion q;
	q.SetRotationX(MATH_PI / 2.f);
	//m_model2.m_transform.scale = Vector3(1, 1, 1)*0.4f;
	m_model2.m_transform.rot = q;

	cViewport svp = m_renderer.m_viewPort;
	svp.m_vp.MinDepth = 0.f;
	svp.m_vp.MaxDepth = 1.f;
	svp.m_vp.Width = 1024;
	svp.m_vp.Height = 1024;
	g_shadowMap.Create(m_renderer, svp, DXGI_FORMAT_R32_FLOAT);
	g_unBindTex.Create(m_renderer, "../media/whitetex.dds");

	// Create Cube
	cAssimpModel *assimpModel = new cAssimpModel();
	cMesh2 *mesh = new cMesh2();
	assimpModel->m_meshes.push_back(mesh);

	using namespace std;
	ifstream ifs("../media/cube/cube.txt");
	assert(ifs.is_open());

	char input;
	ifs.get(input);
	while (input != ':')
		ifs.get(input);
	string tmp1, tmp2;
	int vertexCount = 0;
	ifs >> vertexCount;

	ifs.get(input);
	while (input != ':')
		ifs.get(input);
	ifs.get(input);
	ifs.get(input);

	sRawMesh2 rawMesh;
	rawMesh.vertices.resize(vertexCount);
	rawMesh.normals.resize(vertexCount);
	rawMesh.tex.resize(vertexCount);
	rawMesh.indices.resize(vertexCount);
	for (int i = 0; i < vertexCount; ++i)
	{
		ifs >> rawMesh.vertices[i].x >> rawMesh.vertices[i].y >> rawMesh.vertices[i].z;
		ifs >> rawMesh.tex[i].x >> rawMesh.tex[i].y;
		ifs >> rawMesh.normals[i].x >> rawMesh.normals[i].y >> rawMesh.normals[i].z;
		rawMesh.indices[i] = i;
	}
	ifs.close();

	rawMesh.mtrl.ambient = Vector4(0.2f, 0.2f, 0.2f, 1);
	rawMesh.mtrl.diffuse = Vector4(0.8f, 0.8f, 0.8f, 1);
	rawMesh.mtrl.power = 20;
	rawMesh.mtrl.texture = "../media/cube/stone01.dds";
	rawMesh.mtrl.bumpMap = "../media/cube/bump01.dds";

	mesh->Create(m_renderer, rawMesh, NULL, true);
	m_model2.m_model = assimpModel;
	m_model2.m_transform.pos.y = 2.f;

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	m_model.Update(m_renderer, deltaSeconds);
	m_model2.Update(m_renderer, deltaSeconds);
	__super::OnUpdate(deltaSeconds);
}


void cViewer::OnRender(const float deltaSeconds)
{
	cRenderer &renderer = m_renderer;
	cAutoCam cam(&g_lightCamera);

	g_unBindTex.Bind(renderer, 1);// Unbind ShaderResource
	g_shadowMap.SetRenderTarget(renderer);
	renderer.ClearScene(false, Vector4(1, 1, 1, 1));

	renderer.BeginScene();

	GetMainCamera().Bind(renderer);
	GetMainLight().Set(g_lightCamera);
	GetMainLight().Bind(renderer);

	Matrix44 view, proj, tt;
	g_lightCamera.GetShadowMatrix(view, proj, tt);
	renderer.m_cbPerFrame.m_v->mLightView[0] = XMMatrixTranspose(view.GetMatrixXM());
	renderer.m_cbPerFrame.m_v->mLightProj[0] = XMMatrixTranspose(proj.GetMatrixXM());
	renderer.m_cbPerFrame.m_v->mLightTT = XMMatrixTranspose(tt.GetMatrixXM());

	cShader11 *shader = renderer.m_shaderMgr.FindShader(eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE);

	renderer.UnbindTextureAll();
	m_ground.m_techniqueName = "BuildShadowMap";
	m_model2.m_techniqueName = "BuildShadowMap";
	m_ground.Render(renderer);
	m_model2.Render(renderer);
	renderer.EndScene();

	g_shadowMap.RecoveryRenderTarget(renderer);

	__super::OnRender(deltaSeconds);
}


void cViewer::OnEventProc(const sf::Event &evt)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (evt.type)
	{
	case sf::Event::KeyPressed:
		switch (evt.key.code)
		{
		case sf::Keyboard::Escape: close(); break;
		}
		break;
	}
}
