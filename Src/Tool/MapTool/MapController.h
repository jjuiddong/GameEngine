#pragma once


class cMapController  : public common::cObservable2
									,public common::cSingleton<cMapController>
{
public:
	cMapController(void);
	~cMapController(void);

	bool CreateDefaultTerrain();
	bool LoadTRNFile(const string &fileName);
	bool LoadHeightMap(const string &fileName);
	bool LoadHeightMapTexture(const string &fileName);
	bool SaveTRNFile(const string &fileName);

	graphic::cTerrainEditor& GetTerrain();
	graphic::cTerrainCursor& GetTerrainCursor();
	const string& GetHeightMapFileName();
	const string& GetTextureFileName();

	void BrushTerrain(CPoint point, const float elapseT);
	void BrushTexture(CPoint point);
	void UpdateBrush();
	void UpdateSplatLayer();
	void UpdateHeightFactor(const float heightFactor);
	void UpdatePlaceModel();
	void SendNotifyMessage(const NOTIFY_TYPE::TYPE type);

	void ChangeEditMode(EDIT_MODE::TYPE mode);
	EDIT_MODE::TYPE GetEditMode() const;


private:
	graphic::cTerrainEditor m_terrain;
	graphic::cTerrainCursor m_cursor;
	string m_textFileName;
	EDIT_MODE::TYPE m_editMode;
};


inline graphic::cTerrainEditor& cMapController::GetTerrain() { return m_terrain; }
inline graphic::cTerrainCursor& cMapController::GetTerrainCursor() { return m_cursor; }
inline const string& cMapController::GetHeightMapFileName() { return m_terrain.GetHeightMapFileName(); }
inline const string& cMapController::GetTextureFileName() { return m_textFileName; }
inline EDIT_MODE::TYPE cMapController::GetEditMode() const { return m_editMode; }
