
#include "stdafx.h"
#include "TestScene.h"

using namespace graphic;


//DECLARE_TYPE_NAME(cViewer)
class cViewer : public framework::cGameMain
						//, public memmonitor::Monitor<cViewer, TYPE_NAME(cViewer)>
{
public:
	cViewer();
	virtual ~cViewer();

	virtual bool OnInit() override;
	virtual void OnUpdate(const float elapseT) override;
	virtual void OnRender(const float elapseT) override;
	virtual void OnShutdown() override;
	virtual void OnMessageProc( UINT message, WPARAM wParam, LPARAM lParam) override;


protected:
	void MakeShadowMap();


private:
	LPD3DXSPRITE m_sprite;

	graphic::cMaterial m_mtrl;
	graphic::cTexture m_texture;
	graphic::cCharacter m_character;
	vector<graphic::cCharacter> m_chars;
	graphic::cGrid m_grid;

	graphic::cSprite *m_image;
	graphic::cTerrain m_terrain;
	graphic::cCube m_cube;
	graphic::cSphere m_sphere;
	cShadow1 m_shadow;

	Vector3 m_pos;

	cTestScene *m_scene;

	string m_filePath;

	POINT m_curPos;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
	Matrix44 m_rotateTm;

	Vector3 m_boxPos;
};

INIT_FRAMEWORK(cViewer);


cViewer::cViewer() :
	m_sprite(NULL)
,	m_image(NULL)
,	m_scene(NULL)
,	m_character(1000)
{
	m_windowName = L"Viewer";
	const RECT r = {0, 0, 1024, 768};
	m_windowRect = r;
	m_LButtonDown = false;
	m_RButtonDown = false;
	m_MButtonDown = false;
}

cViewer::~cViewer()
{
	SAFE_DELETE(m_image);
	SAFE_DELETE(m_scene);
	SAFE_RELEASE(m_sprite);
	graphic::ReleaseRenderer();
}


bool cViewer::OnInit()
{
	DragAcceptFiles(m_hWnd, TRUE);

	cResourceManager::Get()->SetMediaDirectory("../media/");

	D3DXCreateSprite(m_renderer.GetDevice(), &m_sprite);

	m_shadow.Create(m_renderer, 1024, 1024);

	//m_scene = new cTestScene(m_sprite);
	//m_scene->SetPos(Vector3(100,100,0));


	//m_model.Create( "../media/max script/valle1.dat" );
	//m_model.SetAnimation( "../media/max script/valle_forward.ani" );


	//m_character.Create( "tiac.dat" );
	//m_character.LoadWeapon( "tiac_weapon.dat" );
	//m_character.SetRenderBoundingBox(true);
	//m_character.SetRenderWeaponBoundingBox(true);


	//m_particles.Create( "강소라.jpg", 512 );

	// 1
	//m_particles.Create( "particle.bmp", 512 );
	//m_particles.m_numToRelease = 5;
	//m_particles.m_releaseInterval = 0.05f;
	//m_particles.m_lifeCycle = 4.f;
	//m_particles.m_pointSize = 0.5;
	//m_particles.m_velocity = Vector3(0,5,0);
	//m_particles.m_wind = Vector3(2,0,0);
	//m_particles.m_velocityVar = 1.5f;

	// 2
	//m_particles.Create( "particle.bmp", 2048 );
	//m_particles.m_numToRelease = 10;
	//m_particles.m_releaseInterval = 0.05f;
	//m_particles.m_lifeCycle = 5.f;
	//m_particles.m_pointSize = 0.5;
	//m_particles.m_bAirResistence = false;
	//m_particles.m_gravity = Vector3(0,0,0);
	//m_particles.m_velocityVar = 2.f;

	// 3
	//m_particles.Create( "particle.bmp", 256 );
	//m_particles.m_numToRelease = 10;
	//m_particles.m_releaseInterval = 0.05f;
	//m_particles.m_lifeCycle = 5.f;
	//m_particles.m_pointSize = 1.5f;
	//m_particles.m_gravity = Vector3(0,-9.8f,0);
	//m_particles.m_emitPos = Vector3(0,0.1f,0);
	//m_particles.m_velocityVar = 20.f;
	//m_particles.AddCollisionPlane( Plane(Vector3(0,1,0), Vector3(0,0,0)) );
	//m_snow.Create( Vector3(-5,-5,-5), Vector3(5,5,5), 256);


	// start craft 2
	// zealot
	{
		m_character.Create( m_renderer, "zealot.dat" );
		if (graphic::cMesh* mesh = m_character.GetMesh("Sphere001"))
			mesh->SetRender(false);
		//m_character.SetShader( graphic::cResourceManager::Get()->LoadShader(
		//	"hlsl_skinning_using_texcoord.fx") );
		m_character.SetShader( graphic::cResourceManager::Get()->LoadShader(
			m_renderer, "hlsl_skinning_using_texcoord_sc2.fx") );
		m_character.SetRenderShadow(true);
		//Matrix44 tm;
		//tm.SetPosition(Vector3(100, 0, -100));
		//m_character.SetTransform(tm);

		using namespace graphic;

		vector<sActionData> actions;
		actions.reserve(16);
		actions.push_back( sActionData(CHARACTER_ACTION::NORMAL, "zealot_stand.ani") );
		actions.push_back( sActionData(CHARACTER_ACTION::RUN, "zealot_walk.ani") );
		actions.push_back( sActionData(CHARACTER_ACTION::ATTACK, "zealot_attack.ani") );
		m_character.SetActionData(actions);

		//m_character.Action( CHARACTER_ACTION::RUN );
		
		//m_character.SetAction( new ai::cAction(&m_character, "normal", "zealot_stand.ani") );
		//m_character.StartAction();
	}

	if (1)
	{
		using namespace graphic;
		vector<sActionData> actions;
		actions.reserve(16);
		//actions.push_back( sActionData(CHARACTER_ACTION::NORMAL, "tiac_normal.ani") );
		//actions.push_back( sActionData(CHARACTER_ACTION::RUN, "zealot_walk.ani") );
		//actions.push_back( sActionData(CHARACTER_ACTION::ATTACK, "tiac_attack.ani") );
		//actions.push_back( sActionData(CHARACTER_ACTION::NORMAL, "zergling_stand.ani") );
		//actions.push_back( sActionData(CHARACTER_ACTION::RUN, "zergling_walk.ani") );
		//actions.push_back( sActionData(CHARACTER_ACTION::ATTACK, "zergling_attack.ani") );

		//actions.push_back( sActionData(CHARACTER_ACTION::NORMAL, "zergling_stand.ani") );
		actions.push_back( sActionData(CHARACTER_ACTION::RUN, "zealot_walk.ani") );
		actions.push_back( sActionData(CHARACTER_ACTION::ATTACK, "zealot_attack.ani") );

		int idx = 0;
		m_chars.resize(100);
		BOOST_FOREACH (auto &character, m_chars)
		{
			character.Create(m_renderer, "zealot.dat" );
			if (graphic::cMesh* mesh = character.GetMesh("Sphere001"))
				mesh->SetRender(false);

			character.SetShader( graphic::cResourceManager::Get()->LoadShader(
				m_renderer, "hlsl_skinning_using_texcoord_sc2.fx") );

			character.SetRenderShadow(true);

			character.SetActionData(actions);
			character.Action( CHARACTER_ACTION::RUN );

			Matrix44 matT;
			matT.SetTranslate( Vector3((float)(idx%10 - 5), 0, (float)(idx/10 - 5)) );
			character.SetTransform(matT);
			++idx;
		}
	}

	m_grid.Create(m_renderer, 100, 100, 50);

	//m_billboard.Create(m_renderer, graphic::BILLBOARD_TYPE::Y_AXIS, 100, 100, Vector3(100,0,100), "Pine.png" );


	//using namespace graphic;

	//vector<sActionData> actions;
	//actions.reserve(16);
	//actions.push_back( sActionData(CHARACTER_ACTION::NORMAL, "tiac_normal.ani") );
	//actions.push_back( sActionData(CHARACTER_ACTION::RUN, "tiac_forward.ani") );
	//actions.push_back( sActionData(CHARACTER_ACTION::DASH, "tiac_dash.ani") );
	//actions.push_back( sActionData(CHARACTER_ACTION::GUARD, "tiac_guard.ani") );
	//actions.push_back( sActionData(CHARACTER_ACTION::ATTACK, "tiac_la.ani") );
	//m_character.SetActionData(actions);

	//m_character.Action( CHARACTER_ACTION::NORMAL );


	//m_teraCharacter.Create( "popori_face00.dat", "popori_hair00.dat",
	//	"popori_body00.dat", "popori_hand00.dat", "popori_leg00.dat", "" );
	//m_teraCharacter.Create( "popori_face0.dat", "popori_hair1.dat",
	//	"popori_body0.dat", "popori_hand0.dat", "popori_leg0.dat", "" );
	//m_teraCharacter.Create( "popori_face2.dat", "popori_hair2.dat",
	//	"popori_body3.dat", "popori_hand3.dat", "popori_leg0.dat", "" );

	//m_teraCharacter.SetAnimation( "../media/maxscript/popori_idle3.ani");



	m_terrain.CreateFromTRNFile(m_renderer, "../media/terrain/terrain121.trn" );

	m_cube.SetCube(m_renderer, Vector3(-10,-10,-10), Vector3(10,10,10));
	//m_sphere.Create(100, 20, 20);

	m_mtrl.InitWhite();

	//Vector4 color(1,1,1,1);
	//m_light.Init( graphic::cLight::LIGHT_DIRECTIONAL, 
	//	color * 0.3f, 
	//	color * 0.7f, 
	//	color, 
	//	Vector3(0,-1,0));
	//m_light.Bind(0);

	
	const int WINSIZE_X = 1024;		//초기 윈도우 가로 크기
	const int WINSIZE_Y = 768;	//초기 윈도우 세로 크기
	GetMainCamera()->Init(&m_renderer);
	GetMainCamera()->SetCamera(Vector3(10, 10,-10), Vector3(0,0,0), Vector3(0,1,0));
	GetMainCamera()->SetProjection(D3DX_PI / 4.f, (float)WINSIZE_X / (float) WINSIZE_Y, 1.f, 10000.0f);
	
	GetMainLight().Init( cLight::LIGHT_DIRECTIONAL );
	GetMainLight().SetPosition(Vector3(30,30,30));
	GetMainLight().SetDirection(Vector3(1,-1,1).Normal());

	m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	m_renderer.GetDevice()->LightEnable (0,true);

	return true;
}


void cViewer::OnUpdate(const float elapseT)
{
	m_terrain.Move(elapseT);
	//m_model.Move(elapseT);
	m_character.Update(elapseT);
	//m_teraCharacter.Move(elapseT);

	//m_particles.Move(elapseT);
	//m_snow.Move(elapseT);

	BOOST_FOREACH (auto &character, m_chars)
		character.Update(elapseT);


	//collisionMgr.UpdateCollisionBox();
	//collisionMgr.CollisionTest(1);

	//if (GetAsyncKeyState(VK_LEFT)
	//	|| GetAsyncKeyState(VK_RIGHT)
	//	|| GetAsyncKeyState(VK_UP)
	//	|| GetAsyncKeyState(VK_DOWN))
	//{
	//	if (!GetAsyncKeyState(VK_SPACE))
	//		m_character.Action(graphic::CHARACTER_ACTION::RUN);
	//}
	//else
	//{
	//	if (!GetAsyncKeyState(VK_SPACE))
	//		m_character.Action(graphic::CHARACTER_ACTION::NORMAL);
	//}

	//if (GetAsyncKeyState(VK_SPACE))
	//{
	//	m_character.Action(graphic::CHARACTER_ACTION::ATTACK);
	//}
}


void cViewer::MakeShadowMap()
{
	m_terrain.PreRender(m_renderer);
	//m_terrain.RenderModelShadow(m_renderer, m_character);

	const Vector3 pos(0,0,0);
	// 전역 광원으로 부터 그림자 생성에 필요한 정보를 얻어온다.
	Vector3 lightPos;
	Matrix44 view, proj, tt;
	cLightManager::Get()->GetMainLight().GetShadowMatrix(
		pos, lightPos, view, proj, tt);

	m_shadow.m_surface.Begin();

	m_renderer.GetDevice()->Clear(0, NULL
		, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
		, 0x00000000, 1.0f, 0L);

	BOOST_FOREACH(auto &character, m_chars)
		character.RenderShadow(m_renderer, view*proj, lightPos, Vector3(0, -1, 0), Matrix44::Identity);

	m_shadow.m_surface.End();

	m_terrain.m_shader->SetTexture("ShadowMap", m_shadow.GetTexture());
	m_terrain.m_shader->SetMatrix("mWVPT", view * proj * tt);
}


void cViewer::OnRender(const float elapseT)
{
	MakeShadowMap();

	if (SUCCEEDED(m_renderer.GetDevice()->Clear(
		0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_XRGB(150, 150, 150), 1.0f, 0)))
	{
		m_renderer.GetDevice()->BeginScene();

		//if (m_scene)
		//	m_scene->Render(m_renderer, Matrix44::Identity);

		//m_grid.RenderLinelist();
		m_terrain.Render(m_renderer);
		m_renderer.RenderGrid();
		m_renderer.RenderAxis();

		//m_particles.Render();

		//m_cube.Render(m_renderer, Matrix44::Identity);
		//m_character.SetTM(m_cube.GetTransform());
		//m_character.Render(m_renderer, Matrix44::Identity);
		//m_teraCharacter.Render(Matrix44::Identity);

		m_shadow.RenderShadowMap(m_renderer);

		BOOST_FOREACH (auto &character, m_chars)
			character.Render(m_renderer, Matrix44::Identity);

		//m_snow.Render();
		//m_billboard.Render(m_renderer);
		//m_character.GetShadow().RenderShadowMap();

		//m_cube.Render(matIdentity);
		m_renderer.RenderFPS();

		m_renderer.GetDevice()->EndScene();
		m_renderer.GetDevice()->Present( NULL, NULL, NULL, NULL );
	}
}


void cViewer::OnShutdown()
{

}


void cViewer::OnMessageProc( UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_scene)
		m_scene->MessageProc(message, wParam, lParam);

	switch (message)
	{
	case WM_DROPFILES:
		{
			HDROP hdrop = (HDROP)wParam;
			char filePath[ MAX_PATH];
			const UINT size = DragQueryFileA(hdrop, 0, filePath, MAX_PATH);
			if (size == 0) 
				return;// handle error...

			m_filePath = filePath;

			const graphic::RESOURCE_TYPE::TYPE type = graphic::cResourceManager::Get()->GetFileKind(filePath);
			switch (type)
			{
			case graphic::RESOURCE_TYPE::MESH:
				//m_model.Create(filePath);
				break;

			case graphic::RESOURCE_TYPE::ANIMATION:
				//m_model.SetAnimation(filePath);
				break;
			}
		}
		break;

	case WM_MOUSEWHEEL:
		{
			int fwKeys = GET_KEYSTATE_WPARAM(wParam);
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			dbg::Print( "%d %d", fwKeys, zDelta);

			const float len = graphic::GetMainCamera()->GetDistance();
			float zoomLen = (len > 100)? 50 : (len/4.f);
			if (fwKeys & 0x4)
				zoomLen = zoomLen/10.f;

			graphic::GetMainCamera()->Zoom( (zDelta<0)? -zoomLen : zoomLen );
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F5: // Refresh
			{
				if (m_filePath.empty())
					return;
				//m_model.Create(m_filePath);
			}
			break;
		case VK_BACK:
			// 회전 행렬 초기화.
			//m_rotateTm.SetIdentity();
			//m_model.SetTM(m_rotateTm);
			break;
		case VK_TAB:
			{
				static bool flag = false;
				m_renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, flag? D3DCULL_CCW : D3DCULL_NONE);
				m_renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag? D3DFILL_SOLID : D3DFILL_WIREFRAME);
				flag = !flag;
			}
			break;

		case VK_LEFT: m_boxPos.x -= 10.f; break;
		case VK_RIGHT: m_boxPos.x += 10.f; break;
		case VK_UP: m_boxPos.z += 10.f; break;
		case VK_DOWN: m_boxPos.z -= 10.f; break;
		}
		break;

	case WM_LBUTTONDOWN:
		{
			m_LButtonDown = true;
			m_curPos.x = LOWORD(lParam);
			m_curPos.y = HIWORD(lParam);
		}
		break;

	case WM_LBUTTONUP:
		m_LButtonDown = false;
		break;

	case WM_RBUTTONDOWN:
		{
			m_RButtonDown = true;
			m_curPos.x = LOWORD(lParam);
			m_curPos.y = HIWORD(lParam);
		
			// unit 이동.
			Ray ray(m_curPos.x, m_curPos.y, 1024, 768,
				GetMainCamera()->GetProjectionMatrix(), 
				GetMainCamera()->GetViewMatrix());

			Vector3 pickPos;
			if (m_terrain.Pick(ray.orig, ray.dir, pickPos))
			{
				//m_character.SetAction(new ai::cAction(&m_character, "normal", "zealot_stand.ani") );
				//ai::cMove<cCharacter> *action = new ai::cMove<cCharacter>(&m_character, pickPos);
				//action->Init(&m_character, pickPos);
				//m_character.	.PushAction( action );
				//m_character.StartAction();
			}
		
		}
		break;

	case WM_RBUTTONUP:
		m_RButtonDown = false;
		break;

	case WM_MBUTTONDOWN:
		m_MButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
		break;

	case WM_MBUTTONUP:
		m_MButtonDown = false;
		break;

	case WM_MOUSEMOVE:
		{
			if (wParam & 0x10) // middle button down
			{
				POINT pos = {LOWORD(lParam), HIWORD(lParam)};
				if (m_scene)
					m_scene->SetPos(Vector3((float)pos.x, (float)pos.y,0));
			}

			if (m_LButtonDown)
			{
				POINT pos = {LOWORD(lParam), HIWORD(lParam)};
				const int x = pos.x - m_curPos.x;
				const int y = pos.y - m_curPos.y;
				m_curPos = pos;

				Quaternion q1(graphic::GetMainCamera()->GetRight(), -y * 0.01f);
				Quaternion q2(graphic::GetMainCamera()->GetUpVector(), -x * 0.01f);

				m_rotateTm *= (q2.GetMatrix() * q1.GetMatrix());
			}
			else if (m_RButtonDown)
			{
				POINT pos = {LOWORD(lParam), HIWORD(lParam)};
				const int x = pos.x - m_curPos.x;
				const int y = pos.y - m_curPos.y;
				m_curPos = pos;

				//if (GetAsyncKeyState('C'))
				{
					graphic::GetMainCamera()->Yaw2( x * 0.005f );
					graphic::GetMainCamera()->Pitch2( y * 0.005f );
				}
			}
			else if (m_MButtonDown)
			{
				const POINT point = {LOWORD(lParam), HIWORD(lParam)};
				const POINT pos = {point.x - m_curPos.x, point.y - m_curPos.y};
				m_curPos = point;

				const float len = graphic::GetMainCamera()->GetDistance();
				graphic::GetMainCamera()->MoveRight( -pos.x * len * 0.001f );
				graphic::GetMainCamera()->MoveUp( pos.y * len * 0.001f );
			}
			else
			{
				POINT pos = {LOWORD(lParam), HIWORD(lParam)};

				Vector3 pickPos;
				Ray ray(pos.x, pos.y, 1024, 768, 
					graphic::GetMainCamera()->GetProjectionMatrix(), 
					graphic::GetMainCamera()->GetViewMatrix());
				const float y = m_terrain.GetHeightFromRay(ray.orig, ray.dir, pickPos);

				pickPos.y = y;

				Matrix44 matT;
				matT.SetTranslate(pickPos);
				m_cube.SetTransform(matT);

				Matrix44 matT2;
				matT2.SetTranslate(Vector3(pickPos.x, 150, pickPos.z));
				m_sphere.SetTransform(matT2);
				
				//m_scene->SetPos(Vector3(pos.x, pos.y,0));
				//if (m_image->IsContain(Vector2(pos.x, pos.y)))
				//{
				//	static int count = 0;
				//	++count;
				//	dbg::Print( "IsContain %d", count);
				//}
			}

		}
		break;
	}
}

