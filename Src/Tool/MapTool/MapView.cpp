// MapView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "MapView.h"
#include "LightPanel.h"


using namespace graphic;

CMapView *g_mapView = NULL;


// CMapView
CMapView::CMapView() :
	m_dxInit(false)
,	m_RButtonDown(false)
,	m_LButtonDown(false)
,	m_MButtonDown(false)
{
	g_mapView = this;
}

CMapView::~CMapView()
{
	
}

BEGIN_MESSAGE_MAP(CMapView, CView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CMapView 그리기입니다.

void CMapView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// CMapView 진단입니다.

#ifdef _DEBUG
void CMapView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CMapView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMapView 메시지 처리기입니다.

bool CMapView::Init()
{
	graphic::GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	graphic::GetDevice()->LightEnable (
		0, // 활성화/ 비활성화 하려는 광원 리스트 내의 요소
		true); // true = 활성화 ， false = 비활성화


	//m_grid.Create(64,64,50.f);
	//m_cube.SetCube(Vector3(-10,-10,-10), Vector3(10,10,10));
	//m_cube.SetColor( 0xFF0000FF );
	m_dxInit = true;

	m_lightLine.GetMaterial().InitBlack();
	m_lightLine.GetMaterial().GetMtrl().Emissive = *(D3DXCOLOR*)&Vector4(1,1,0,1);

	m_lightSphere.Create(5, 10, 10);
	m_lightSphere.GetMaterial().InitBlack();
	m_lightSphere.GetMaterial().GetMtrl().Emissive = *(D3DCOLORVALUE*)&Vector4(1,1,0,1);

	return true;
}


void CMapView::Render()
{
	if (!m_dxInit)
		return;

	cMapController::Get()->GetTerrain().PreRender();

	//화면 청소
	if (SUCCEEDED(graphic::GetDevice()->Clear( 
		0,			//청소할 영역의 D3DRECT 배열 갯수		( 전체 클리어 0 )
		NULL,		//청소할 영역의 D3DRECT 배열 포인터		( 전체 클리어 NULL )
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,	//청소될 버퍼 플레그 ( D3DCLEAR_TARGET 컬러버퍼, D3DCLEAR_ZBUFFER 깊이버퍼, D3DCLEAR_STENCIL 스텐실버퍼
		D3DCOLOR_XRGB(150, 150, 150),			//컬러버퍼를 청소하고 채워질 색상( 0xAARRGGBB )
		1.0f,				//깊이버퍼를 청소할값 ( 0 ~ 1 0 이 카메라에서 제일가까운 1 이 카메라에서 제일 먼 )
		0					//스텐실 버퍼를 채울값
		)))
	{
		//화면 청소가 성공적으로 이루어 졌다면... 랜더링 시작
		graphic::GetDevice()->BeginScene();
		//graphic::GetRenderer()->RenderFPS();
		//graphic::GetRenderer()->RenderGrid();

		cMapController::Get()->GetTerrain().Render();

		switch (cMapController::Get()->GetEditMode())
		{
		case EDIT_MODE::MODE_TERRAIN:
			cMapController::Get()->GetTerrainCursor().RenderTerrainBrush();
			break;
		case EDIT_MODE::MODE_BRUSH:
			cMapController::Get()->GetTerrainCursor().RenderBrush();
			break;
		case EDIT_MODE::MODE_MODEL:
			cMapController::Get()->GetTerrainCursor().RenderModel();
			break;
		}

		// 조명 방향 선 출력.
		if (cMapController::Get()->GetEditMode() == EDIT_MODE::MODE_LIGHT)
		{
			// 광원 위치 조정 후 출력.
			const Vector3 lightPos = cLightManager::Get()->GetMainLight().GetPosition();
			Matrix44 lightTm;
			lightTm.SetTranslate( lightPos );
			m_lightSphere.SetTransform(lightTm);
			m_lightSphere.Render(Matrix44::Identity);

			if (m_LButtonDown 
				&& g_lightPanel->m_EditDirection)
			{
				m_lightLine.Render();
			}
		}

		graphic::GetRenderer()->RenderAxis();
		graphic::GetRenderer()->RenderFPS();

		//랜더링 끝
		graphic::GetDevice()->EndScene();

		//랜더링이 끝났으면 랜더링된 내용 화면으로 전송
		graphic::GetDevice()->Present( NULL, NULL, NULL, NULL );
	}
}


void CMapView::Update(float elapseT)
{
	graphic::GetRenderer()->Update(elapseT);
	cMapController::Get()->GetTerrain().Move(elapseT);

	// 지형 높낮이 편집.
	if (EDIT_MODE::MODE_TERRAIN == cMapController::Get()->GetEditMode())
	{
		if (m_LButtonDown)
		{
			cMapController::Get()->BrushTerrain(m_curPos, elapseT);
		}
	}
}


void CMapView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	SetCapture();
	m_LButtonDown = true;
	m_curPos = point;
	CView::OnLButtonDown(nFlags, point);
}


void CMapView::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	// 지형위에 모델을 위치 시킨다.
	if (m_LButtonDown &&
		(cMapController::Get()->GetEditMode() == EDIT_MODE::MODE_MODEL))
	{
		// 모델이 선택되어 있는 상태라면, 모델을 지형위에 위치 시킨다.
		if (cMapController::Get()->GetTerrainCursor().IsSelectModel())
		{
			if (const graphic::cModel *model = cMapController::Get()->GetTerrainCursor().GetSelectModel())
			{
				graphic::cModel *cloneModel = cMapController::Get()->GetTerrain().AddRigidModel(*model);

				// 툴에서 쓰이는 변환 정보 초기화.
				sTransform tm;
				tm.pos = cloneModel->GetTransform().GetPosition();
				tm.rot = Vector3(0,0,0);
				tm.scale = Vector3(1,1,1);
				cloneModel->SetToolTransform(tm);

				cMapController::Get()->UpdatePlaceModel();
			}
		}
		else
		{
			// 모델이 선택되어 있지 않다면, 지형위의 모델을 피킹해서 선택한다.
			cCamera &camera = *cMainCamera::Get();
			m_ray.Create(point.x, point.y, VIEW_WIDTH, VIEW_HEIGHT, 
				camera.GetProjectionMatrix(), camera.GetViewMatrix() );

			// 모델 피킹.
			if (graphic::cModel *model = 
				cMapController::Get()->GetTerrain().PickModel(m_ray.orig, m_ray.dir))
			{
				m_focusModel = model;
				cMapController::Get()->SendNotifyMessage(NOTIFY_TYPE::NOTIFY_SELECT_PLACE_MODEL);
				//cMapController::Get()->GetTerrain().RemoveRigidModel(model, false);
				//cMapController::Get()->GetTerrainCursor().SelectModel(model);
				//cMapController::Get()->UpdatePlaceModel();
			}

		}
	}

	m_LButtonDown = false;
	CView::OnLButtonUp(nFlags, point);
}


void CMapView::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	SetCapture();
	m_RButtonDown = true;
	m_curPos = point;
	CView::OnRButtonDown(nFlags, point);
}


void CMapView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_RButtonDown = false;
	CView::OnRButtonUp(nFlags, point);
}


void CMapView::OnMouseMove(UINT nFlags, CPoint point)
{
	cCamera &camera = *cMainCamera::Get();

	if (m_LButtonDown)
	{
		m_curPos = point;

		bool isEditLightDirection = false;
		if (cMapController::Get()->GetEditMode() == EDIT_MODE::MODE_LIGHT)
		{
			isEditLightDirection = g_lightPanel->m_EditDirection? true : false;
		}

		if (isEditLightDirection)
		{
			const int width = 1024;
			const int height = 768;

			Ray ray(point.x, point.y, width, height, GetMainCamera()->GetProjectionMatrix(), 
				GetMainCamera()->GetViewMatrix());
			Vector3 pickPos;
			if (cMapController::Get()->GetTerrain().Pick( ray.orig, ray.dir, pickPos ))
			{
				// 광원 방향 구하기.
				const Vector3 lightPos = cLightManager::Get()->GetMainLight().GetPosition();
				Vector3 dir = pickPos - lightPos;
				dir.Normalize();
				m_lightLine.SetLine(lightPos, pickPos, 1);

				// 광원 위치 조정
				Matrix44 lightTm;
				lightTm.SetTranslate( lightPos );
				m_lightSphere.SetTransform(lightTm);

				// 광원 방향 업데이트
				cLightManager::Get()->GetMainLight().SetDirection(dir);

				// 모든 리스너에게 전달한다.
				cMapController::Get()->SendNotifyMessage(NOTIFY_TYPE::NOTIFY_UPDATE_LIGHT_DIRECTION);
			}
		}
		else if (EDIT_MODE::MODE_BRUSH == cMapController::Get()->GetEditMode())
		{
			cMapController::Get()->BrushTexture(point);
		}
	}
	else if (m_RButtonDown)
	{
		CPoint pos = point - m_curPos;
		m_curPos = point;
		camera.Pitch2(pos.y * 0.005f); 
		camera.Yaw2(pos.x * 0.005f); 
	}
	else if (m_MButtonDown)
	{
		CPoint pos = point - m_curPos;
		m_curPos = point;

		Vector3 dir = camera.GetDirection();
		dir.y = 0;
		dir.Normalize();

		const float len = camera.GetDistance();
		camera.MoveRight( -pos.x * len * 0.001f );
		camera.MoveAxis( dir, pos.y * len * 0.001f );
	}
	else
	{
		m_curPos = point;

		if ((cMapController::Get()->GetEditMode() == EDIT_MODE::MODE_BRUSH) ||
			(cMapController::Get()->GetEditMode() == EDIT_MODE::MODE_MODEL))
		{
			m_ray.Create(m_curPos.x, m_curPos.y, VIEW_WIDTH, VIEW_HEIGHT, 
				camera.GetProjectionMatrix(), camera.GetViewMatrix() );

			Vector3 pickPos;
			cMapController::Get()->GetTerrain().Pick(m_ray.orig, m_ray.dir, pickPos);
			cMapController::Get()->GetTerrainCursor().UpdateCursor(cMapController::Get()->GetTerrain(), pickPos );
		}

	}

	CView::OnMouseMove(nFlags, point);
}


void CMapView::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	SetCapture();
	m_MButtonDown = true;
	CView::OnMButtonDown(nFlags, point);
}


void CMapView::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_MButtonDown = false;
	ReleaseCapture();
	CView::OnMButtonUp(nFlags, point);
}


BOOL CMapView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	cCamera &camera = *cMainCamera::Get();
	
	const float len = camera.GetDistance();
	float zoomLen = (len > 100)? 50 : (len/4.f);
	if (nFlags & 0x4)
		zoomLen = zoomLen/10.f;

	camera.Zoom( (zDelta<0)? -zoomLen : zoomLen );	

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CMapView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_TAB:
		{
			static bool flag = false;
			graphic::GetDevice()->SetRenderState(D3DRS_CULLMODE, flag? D3DCULL_CCW : D3DCULL_NONE);
			graphic::GetDevice()->SetRenderState(D3DRS_FILLMODE, flag? D3DFILL_SOLID : D3DFILL_WIREFRAME);
			flag = !flag;
		}
		break;

	case VK_ESCAPE:
		{
			cMapController::Get()->GetTerrainCursor().CancelSelectModel();
		}
		break;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
