//
// DX11 FW Font
//

#include "../../../../../Common/Common/common.h"
using namespace common;
#include "../../../../../Common/Graphic11/graphic11.h"
#include "../../../../../Common/Framework11/framework11.h"

#include "FW1FontWrapper.h"
#pragma comment (lib, "FW1FontWrapper.lib")

using namespace graphic;

class cViewer : public framework::cGameMain
{
public:
	cViewer();
	virtual ~cViewer();

	virtual bool OnInit() override;
	virtual void OnUpdate(const float deltaSeconds) override;
	virtual void OnRender(const float deltaSeconds) override;
	virtual void OnLostDevice() override;
	virtual void OnShutdown() override;
	virtual void OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	void ChangeWindowSize();


public:
	cCamera m_terrainCamera;
	cGrid m_ground;
	cDbgLine m_dbgLine;
	cDbgArrow m_dbgArrow;
	cDbgBox m_dbgBox;
	cDbgAxis m_axis;
	cTexture m_texture;
	SpriteFont *m_sprFont;
	SpriteBatch* m_sprBatch;
	IFW1FontWrapper *m_pFontWrapper;
	IDWriteTextLayout *pTextLayout1;
	IDWriteTextLayout *pTextLayout2;
	IDWriteTextLayout *pTextLayout3;

	Transform m_world;
	cMaterial m_mtrl;

	bool m_isFrustumTracking = true;
	sf::Vector2i m_curPos;
	Plane m_groundPlane1, m_groundPlane2;
	float m_moveLen;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
};

INIT_FRAMEWORK(cViewer);



cViewer::cViewer()
	: m_groundPlane1(Vector3(0, 1, 0), 0)
	, m_sprFont(NULL)
	, m_sprBatch(NULL)
{
	m_windowName = L"DX11 FW Font";
	//const RECT r = { 0, 0, 1024, 768 };
	const RECT r = { 0, 0, 1280, 1024 };
	m_windowRect = r;
	m_moveLen = 0;
	m_LButtonDown = false;
	m_RButtonDown = false;
	m_MButtonDown = false;
}

cViewer::~cViewer()
{
	SAFE_DELETE(m_sprFont);
	SAFE_DELETE(m_sprBatch);
	SAFE_RELEASE(m_pFontWrapper);
	SAFE_RELEASE(pTextLayout1);
	SAFE_RELEASE(pTextLayout2);
	SAFE_RELEASE(pTextLayout3);

	graphic::ReleaseRenderer();
}


bool cViewer::OnInit()
{
	DragAcceptFiles(m_hWnd, TRUE);

	//cResourceManager::Get()->SetMediaDirectory("../media/");

	const int WINSIZE_X = m_windowRect.right - m_windowRect.left;
	const int WINSIZE_Y = m_windowRect.bottom - m_windowRect.top;
	GetMainCamera().Init(&m_renderer);
	GetMainCamera().SetCamera(Vector3(30, 30, -30), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GetMainCamera().SetProjection(MATH_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 0.1f, 10000.0f);
	GetMainCamera().SetViewPort(WINSIZE_X, WINSIZE_Y);

	m_terrainCamera.Init(&m_renderer);
	m_terrainCamera.SetCamera(Vector3(-3, 10, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	m_terrainCamera.SetProjection(MATH_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 1.0f, 10000.f);
	m_terrainCamera.SetViewPort(WINSIZE_X, WINSIZE_Y);

	m_ground.Create(m_renderer, 10, 10, 1, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE | eVertexType::TEXTURE);
	m_ground.m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	m_dbgLine.Create(m_renderer, Vector3(2, 0, 0), Vector3(10, 0, 0), 1.f, cColor::RED);
	m_dbgBox.Create(m_renderer);
	cBoundingBox bbox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion());
	m_dbgBox.SetBox(bbox);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	const Vector3 lightPos(-300, 300, -300);
	const Vector3 lightLookat(0, 0, 0);
	GetMainLight().SetPosition(lightPos);
	GetMainLight().SetDirection((lightLookat - lightPos).Normal());

	m_mtrl.InitWhite();

	cBoundingBox bbox2(Vector3(0, 0, 0), Vector3(10, 10, 10), Quaternion());
	m_axis.Create(m_renderer);
	m_axis.SetAxis(bbox2, false);

	m_sprBatch = new DirectX::SpriteBatch(m_renderer.GetDevContext());
	m_sprFont = new DirectX::SpriteFont(m_renderer.GetDevice(), L"Calibri.spritefont");

	// Create the font-wrapper
	IFW1Factory *pFW1Factory;
	HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	if (FAILED(hResult)) {
		MessageBox(NULL, TEXT("FW1CreateFactory"), TEXT("Error"), MB_OK);
		return 0;
	}
	hResult = pFW1Factory->CreateFontWrapper(m_renderer.GetDevice()
		, L"Arial", &m_pFontWrapper);
	if (FAILED(hResult)) {
		MessageBox(NULL, TEXT("CreateFontWrapper"), TEXT("Error"), MB_OK);
		return 0;
	}

	// Get the DirectWrite factory used by the font-wrapper
	IDWriteFactory *pDWriteFactory;
	hResult = m_pFontWrapper->GetDWriteFactory(&pDWriteFactory);

	// Create the default DirectWrite text format to base layouts on
	IDWriteTextFormat *pTextFormat;
	hResult = pDWriteFactory->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		16.0f,
		L"",
		&pTextFormat
	);
	if (FAILED(hResult)) {
		MessageBox(NULL, TEXT("CreateTextFormat"), TEXT("Error"), MB_OK);
		return 0;
	}

	// Create a text layout for a string
	//IDWriteTextLayout *pTextLayout1;
	const WCHAR str1[] = L"ANCIENT GREEK MYTHOLOGY";
	pDWriteFactory->CreateTextLayout(
		str1,
		sizeof(str1) / sizeof(str1[0]),
		pTextFormat,
		0.0f,
		0.0f,
		&pTextLayout1
	);

	// No word wrapping
	pTextLayout1->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	pTextLayout1->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	// Set up typography features
	IDWriteTypography *pTypography;
	pDWriteFactory->CreateTypography(&pTypography);
	DWRITE_FONT_FEATURE fontFeature;
	fontFeature.nameTag = DWRITE_FONT_FEATURE_TAG_STYLISTIC_ALTERNATES;
	fontFeature.parameter = 1;
	pTypography->AddFontFeature(fontFeature);

	// Apply the typography features to select ranges in the string
	DWRITE_TEXT_RANGE textRange;

	textRange.startPosition = 0;
	textRange.length = 1;
	pTextLayout1->SetTypography(pTypography, textRange);
	textRange.startPosition = 10;
	textRange.length = 2;
	pTextLayout1->SetTypography(pTypography, textRange);
	textRange.startPosition = 18;
	textRange.length = 1;
	pTextLayout1->SetTypography(pTypography, textRange);

	pTypography->Release();

	// Set the font and size
	textRange.startPosition = 0;
	textRange.length = sizeof(str1) / sizeof(str1[0]);
	pTextLayout1->SetFontSize(64.0f, textRange);
	pTextLayout1->SetFontFamilyName(L"Pericles", textRange);


	const WCHAR str2[] = L"A B C D E F G H I J K L M N\nA B C D E F G H I J K L M N";
	pDWriteFactory->CreateTextLayout(
		str2,
		sizeof(str2) / sizeof(str2[0]),
		pTextFormat,
		0.0f,
		0.0f,
		&pTextLayout2
	);
	pTextLayout2->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	pTextLayout2->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	textRange.startPosition = 0;
	textRange.length = sizeof(str2) / sizeof(str2[0]);
	pTextLayout2->SetFontSize(48.0f, textRange);
	pTextLayout2->SetFontFamilyName(L"Pescadero", textRange);

	// Add font feature
	fontFeature.nameTag = DWRITE_FONT_FEATURE_TAG_SWASH;
	fontFeature.parameter = 1;
	pDWriteFactory->CreateTypography(&pTypography);
	pTypography->AddFontFeature(fontFeature);
	textRange.startPosition = 28;
	textRange.length = sizeof(str2) / sizeof(str2[0]) - 28;
	pTextLayout2->SetTypography(pTypography, textRange);
	pTypography->Release();

	// Set color overrides for select ranges
	IFW1ColorRGBA *pColorRed;
	pFW1Factory->CreateColor(0xff0000ff, &pColorRed);
	IFW1ColorRGBA *pColorGreen;
	pFW1Factory->CreateColor(0xff00ff00, &pColorGreen);

	textRange.startPosition = 2;
	textRange.length = 8;
	pTextLayout2->SetDrawingEffect(pColorRed, textRange);

	textRange.startPosition = 22;
	textRange.length = 15;
	pTextLayout2->SetDrawingEffect(pColorGreen, textRange);

	pColorRed->Release();
	pColorGreen->Release();



	const WCHAR codeStr[] = L"#include <iostream>\n\nint main() {\n    int num;\n    std::cin >> num;\n\n    for(int i=0; i < num; ++i)\n        std::cout << \"Hello World!\\n\";\n\n    return 0;\n}\n";
	pDWriteFactory->CreateTextLayout(
		codeStr,
		sizeof(codeStr) / sizeof(codeStr[0]),
		pTextFormat,
		0,
		0.0f,
		&pTextLayout3
	);
	pTextLayout3->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

	textRange.startPosition = 0;
	textRange.length = sizeof(codeStr) / sizeof(codeStr[0]);
	pTextLayout3->SetFontSize(13.333333333f, textRange);
	pTextLayout3->SetFontFamilyName(L"Courier New", textRange);



	pFW1Factory->Release();


	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	cAutoCam cam(&m_terrainCamera);

	GetMainCamera().Update(deltaSeconds);
}


void cViewer::OnRender(const float deltaSeconds)
{
	if (m_isFrustumTracking)
		cMainCamera::Get()->PushCamera(&m_terrainCamera);

	//GetMainLight().Bind(m_renderer, 0);

	// Render
	if (m_renderer.ClearScene())
	{
		m_renderer.BeginScene();

		GetMainCamera().Bind(m_renderer);

		static float t = 0;
		t += deltaSeconds;

		XMMATRIX mView = XMLoadFloat4x4((XMFLOAT4X4*)&m_terrainCamera.GetViewMatrix());
		XMMATRIX mProj = XMLoadFloat4x4((XMFLOAT4X4*)&m_terrainCamera.GetProjectionMatrix());
		XMMATRIX mWorld = XMLoadFloat4x4((XMFLOAT4X4*)&m_world.GetMatrix());

		m_renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(mWorld);
		m_renderer.m_cbPerFrame.m_v->mView = XMMatrixTranspose(mView);
		m_renderer.m_cbPerFrame.m_v->mProjection = XMMatrixTranspose(mProj);

		m_renderer.m_cbLight = GetMainLight().GetLight();
		m_renderer.m_cbLight.Update(m_renderer, 1);
		m_renderer.m_cbMaterial = m_mtrl.GetMaterial();
		m_renderer.m_cbMaterial.Update(m_renderer, 2);

		m_ground.Render(m_renderer);
		m_axis.Render(m_renderer);

		m_pFontWrapper->DrawString(
			m_renderer.GetDevContext(),
			//L"Text with D3D11!",// String
			L"한글 테스트!",// String
					   //128.0f,// Font size
			128,
			1280 / 2.0f,// X offset
			1024 / 2.0f,// Y offset
			0xff0099ff,// Text color, 0xAaBbGgRr
			FW1_CENTER | FW1_VCENTER// Flags
		);

		m_pFontWrapper->DrawTextLayout(m_renderer.GetDevContext(), pTextLayout1, 1280 / 2.0f, 250.0f, 0xffffffff, 0);
		m_pFontWrapper->DrawTextLayout(m_renderer.GetDevContext(), pTextLayout2, 1280 / 2.0f, 350.0f, 0xff00aaff, FW1_IMMEDIATECALL);
		m_pFontWrapper->DrawTextLayout(
			m_renderer.GetDevContext(),
			pTextLayout3,
			1280 / 2.0f - 100.0f,
			20.0f,
			0xffffffff,
			FW1_ALIASED | FW1_IMMEDIATECALL
		);

		m_renderer.EndScene();
		m_renderer.Present();
	}

	if (m_isFrustumTracking)
		cMainCamera::Get()->PopCamera();
}


void cViewer::OnLostDevice()
{
	m_renderer.ResetDevice(0, 0, true);
	m_terrainCamera.SetViewPort(m_renderer.m_viewPort.GetWidth(), m_renderer.m_viewPort.GetHeight());
}


void cViewer::OnShutdown()
{
}


void cViewer::ChangeWindowSize()
{
	if (m_renderer.CheckResetDevice())
	{
		m_renderer.ResetDevice();
		m_terrainCamera.SetViewPort(m_renderer.m_viewPort.GetWidth(), m_renderer.m_viewPort.GetHeight());
	}
}


void cViewer::OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	framework::cInputManager::Get()->MouseProc(message, wParam, lParam);

	static bool maximizeWnd = false;
	switch (message)
	{
	case WM_EXITSIZEMOVE:
		ChangeWindowSize();
		break;

	case WM_SIZE:
		if (SIZE_MAXIMIZED == wParam)
		{
			maximizeWnd = true;
			ChangeWindowSize();
		}
		else if (maximizeWnd && (SIZE_RESTORED == wParam))
		{
			maximizeWnd = false;
			ChangeWindowSize();
		}
		break;

	case WM_DROPFILES:
	{
		const HDROP hdrop = (HDROP)wParam;
		char filePath[MAX_PATH];
		const UINT size = DragQueryFileA(hdrop, 0, filePath, MAX_PATH);
		if (size == 0)
			return;// handle error...
	}
	break;

	case WM_MOUSEWHEEL:
	{
		if (m_isFrustumTracking)
			cMainCamera::Get()->PushCamera(&m_terrainCamera);

		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		//dbg::Print("%d %d", fwKeys, zDelta);

		const float len = graphic::GetMainCamera().GetDistance();
		float zoomLen = (len > 100) ? 50 : (len / 4.f);
		if (fwKeys & 0x4)
			zoomLen = zoomLen / 10.f;

		graphic::GetMainCamera().Zoom((zDelta<0) ? -zoomLen : zoomLen);

		if (m_isFrustumTracking)
			cMainCamera::Get()->PopCamera();
	}
	break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_TAB:
		{
			static bool flag = false;
			//m_renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, flag ? D3DCULL_CCW : D3DCULL_NONE);
			//m_renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
			flag = !flag;
		}
		break;

		case VK_RETURN:
			//cResourceManager::Get()->ReloadShader(m_renderer);
			break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
		if (m_isFrustumTracking)
			cMainCamera::Get()->PushCamera(&m_terrainCamera);

		POINT pos = { (short)LOWORD(lParam), (short)HIWORD(lParam) };

		SetCapture(m_hWnd);
		m_LButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);

		Vector3 orig, dir;
		graphic::GetMainCamera().GetRay(pos.x, pos.y, orig, dir);
		Vector3 p1 = m_groundPlane1.Pick(orig, dir);
		m_moveLen = common::clamp(1, 100, (p1 - orig).Length());
		graphic::GetMainCamera().MoveCancel();

		if (m_isFrustumTracking)
			cMainCamera::Get()->PopCamera();
	}
	break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		m_LButtonDown = false;
		break;

	case WM_RBUTTONDOWN:
	{
		SetCapture(m_hWnd);
		m_RButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);

		Ray ray(m_curPos.x, m_curPos.y, 1024, 768,
			GetMainCamera().GetProjectionMatrix(),
			GetMainCamera().GetViewMatrix());
	}
	break;

	case WM_RBUTTONUP:
		m_RButtonDown = false;
		ReleaseCapture();
		break;

	case WM_MBUTTONDOWN:
		SetCapture(m_hWnd);
		m_MButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
		break;

	case WM_MBUTTONUP:
		ReleaseCapture();
		m_MButtonDown = false;
		break;

	case WM_MOUSEMOVE:
	{
		if (m_isFrustumTracking)
			cMainCamera::Get()->PushCamera(&m_terrainCamera);

		sf::Vector2i pos = { (int)LOWORD(lParam), (int)HIWORD(lParam) };

		Vector3 orig, dir;
		graphic::GetMainCamera().GetRay(pos.x, pos.y, orig, dir);
		Vector3 p1 = m_groundPlane1.Pick(orig, dir);
		//m_line.SetLine(orig + Vector3(1, 0, 0), p1, 0.3f);
		m_dbgLine.SetLine(orig + Vector3(-1, 0, 0), p1 + Vector3(-1, 0, 0), 0.3f);

		if (wParam & 0x10) // middle button down
		{
		}

		if (m_LButtonDown)
		{
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;

			if ((abs(x) > 1000) || (abs(y) > 1000))
			{
				if (m_isFrustumTracking)
					cMainCamera::Get()->PopCamera();
				break;
			}

			m_curPos = pos;

			Vector3 dir = graphic::GetMainCamera().GetDirection();
			Vector3 right = graphic::GetMainCamera().GetRight();
			dir.y = 0;
			dir.Normalize();
			right.y = 0;
			right.Normalize();

			graphic::GetMainCamera().MoveRight(-x * m_moveLen * 0.001f);
			graphic::GetMainCamera().MoveFrontHorizontal(y * m_moveLen * 0.001f);
		}
		else if (m_RButtonDown)
		{
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;
			m_curPos = pos;

			graphic::GetMainCamera().Yaw2(x * 0.005f);
			graphic::GetMainCamera().Pitch2(y * 0.005f);

		}
		else if (m_MButtonDown)
		{
			const sf::Vector2i point = { pos.x - m_curPos.x, pos.y - m_curPos.y };
			m_curPos = pos;

			const float len = graphic::GetMainCamera().GetDistance();
			graphic::GetMainCamera().MoveRight(-point.x * len * 0.001f);
			graphic::GetMainCamera().MoveUp(point.y * len * 0.001f);
		}
		else
		{
		}

		if (m_isFrustumTracking)
			cMainCamera::Get()->PopCamera();
	}
	break;
	}
}

