
#pragma once


class cGameApp : public framework::cGameMain
{
public:
	cGameApp();
	virtual ~cGameApp();

	virtual bool OnInit() override;
	virtual void OnUpdate(const float elapseT) override;
	virtual void OnRender(const float elapseT) override;
	virtual void OnShutdown() override;
	virtual void MessageProc( UINT message, WPARAM wParam, LPARAM lParam) override;


protected:
	bool ReadModelFile( const string &fileName, 
		graphic::cVertexBuffer &vtxBuff, int &vtxSize,  
		graphic::cIndexBuffer &idxBuff, int &faceSize );

	void ComputeNormals(graphic::cVertexBuffer &vtxBuff, int vtxSize,  
		graphic::cIndexBuffer &idxBuff, int faceSize);


private:
	graphic::cVertexBuffer m_vtxBuff;
	graphic::cIndexBuffer m_idxBuff;
	int m_VtxSize;
	int m_FaceSize;
	graphic::cMaterial m_mtrl;
	graphic::cLight m_light;
};
