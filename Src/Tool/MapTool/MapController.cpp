#include "StdAfx.h"
#include "MapController.h"

using namespace graphic;


cMapController::cMapController(void) :
	m_editMode(EDIT_MODE::MODE_HEIGHTMAP)
{
	cMainCamera::Get()->SetCamera(Vector3(100,100,-500), Vector3(0,0,0), Vector3(0,1,0));
	cMainCamera::Get()->SetProjection( D3DX_PI / 4.f, (float)VIEW_WIDTH / (float) VIEW_HEIGHT, 1.f, 10000.0f);

}


cMapController::~cMapController(void)
{
}


// ���̸� ������ �о ������ �����Ѵ�.
bool cMapController::LoadHeightMap(const string &fileName)
{
	const bool result = m_terrain.CreateFromHeightMap(fileName, "empty" );

	NotifyObserver();
	return result;
}


// ���� ��ü�� ���� ���� �ؽ��ĸ� �ε��Ѵ�.
bool cMapController::LoadHeightMapTexture(const string &fileName)
{
	m_textFileName = fileName;
	const bool result = m_terrain.CreateTerrainTexture(fileName);

	NotifyObserver();
	return result;
}


// ���� ���� ����. (*.TRN ������ �д´�.)
bool cMapController::LoadTRNFile(const string &fileName)
{
	if (!m_terrain.CreateFromTRNFile(fileName))
	{
		return false;
	}

	NotifyObserver();
	return true;
}


// ���� ������ ���Ͽ� �����Ѵ�.
// TRN �������� ����ȴ�.
bool cMapController::SaveTRNFile(const string &fileName)
{
	const bool result = m_terrain.WriteTRNFile(fileName);
	if (result)
	{
		AfxMessageBox( L"���� ����!\n\n��� ���� ��δ� media ������ ����ּҷ� ����˴ϴ�. \nmedia���� �ȿ� ���� ���� ������ ������ �� �� ������,\n�ٽ� �ѹ� Ȯ�����ּ���." );
	}
	else
	{
		AfxMessageBox( L"���� ���� ����" );
	}

	return result;
}


// ���� ����.
bool cMapController::CreateDefaultTerrain()
{
	m_terrain.CreateTerrain(64, 64, 50.f, 8.f);
	m_terrain.CreateTerrainTexture( "./media/terrain/��1.jpg" );

	NotifyObserver();
	return true;
}


// �� ���� ��� ����.
void cMapController::ChangeEditMode(EDIT_MODE::TYPE mode)
{
	m_editMode = mode;
}


void cMapController::BrushTerrain(CPoint point, const float elapseT)
{
	const Ray ray(point.x, point.y, VIEW_WIDTH, VIEW_HEIGHT, 
		cMainCamera::Get()->GetProjectionMatrix(), cMainCamera::Get()->GetViewMatrix() );

	Vector3 pickPos;
	m_terrain.Pick( ray.orig, ray.dir, pickPos );
	m_cursor.UpdateCursor( m_terrain, pickPos );
	m_terrain.BrushTerrain( m_cursor, elapseT );
}


// ���÷��� �ؽ��� ��忡��, ���콺�� �귯���� �� �� ȣ���Ѵ�.
void cMapController::BrushTexture(CPoint point)
{
	const Ray ray(point.x, point.y, VIEW_WIDTH, VIEW_HEIGHT, 
		cMainCamera::Get()->GetProjectionMatrix(), cMainCamera::Get()->GetViewMatrix() );

	Vector3 pickPos;
	m_terrain.Pick( ray.orig, ray.dir, pickPos );
	m_cursor.UpdateCursor( m_terrain, pickPos );

	const int oldLayerCount = m_terrain.GetLayerCount();
	m_terrain.BrushTexture( m_cursor );

	if (m_terrain.GetLayerCount() != oldLayerCount)
	{
		NotifyObserver(NOTIFY_TYPE::NOTIFY_ADD_LAYER);
	}
}


// �귯�� ������Ʈ.
void cMapController::UpdateBrush()
{
	CPoint pos(VIEW_WIDTH/2, VIEW_HEIGHT/2);
	Ray ray(pos.x, pos.y, VIEW_WIDTH, VIEW_HEIGHT, 
		cMainCamera::Get()->GetProjectionMatrix(), cMainCamera::Get()->GetViewMatrix() );

	Vector3 pickPos;
	m_terrain.Pick( ray.orig, ray.dir, pickPos);
	m_cursor.UpdateCursor(m_terrain, pickPos );

	NotifyObserver( NOTIFY_TYPE::NOTIFY_CHANGE_CURSOR );
}


// ���÷��� ���� ���̾� ������Ʈ.
void cMapController::UpdateSplatLayer()
{
	NotifyObserver( NOTIFY_TYPE::NOTIFY_CHANGE_SPLATLAYER );
}


// HeightFactor �� ������Ʈ �� �� ȣ�� �Ѵ�.
void cMapController::UpdateHeightFactor(const float heightFactor)
{
	m_terrain.SetHeightFactor(heightFactor);
}


// �������� ���� �߰��ǰų� ���� �� �� ȣ���Ѵ�.
void cMapController::UpdatePlaceModel()
{
	NotifyObserver( NOTIFY_TYPE::NOTIFY_ADD_PLACE_MODEL );
}


// �ܺο��� ������Ʈ ������ �˸��� ����Ѵ�.
void cMapController::SendNotifyMessage(const NOTIFY_TYPE::TYPE type)
{
	NotifyObserver( type );
}
