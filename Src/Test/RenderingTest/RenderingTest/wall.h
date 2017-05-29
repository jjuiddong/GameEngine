//
// 2017-02-21, jjuiddong
//
#pragma once


namespace warehouse
{

	class cWall : public graphic::cModel2
	{
	public:
		cWall();
		virtual ~cWall();

		bool Create(graphic::cRenderer &renderer, const Vector3 &pos1, const Vector3 &pos2, const float height, 
			const float thickness, const bool isPillar=true);
		virtual bool Update(graphic::cRenderer &renderer, const float deltaSeconds) override;
		virtual void RenderShader(graphic::cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity) override;
		void Clear();


	public:
		bool m_isPillar;
		graphic::cCube3 m_wall;
		vector<graphic::cCube3*> m_pillars;
		float m_shadowEpsilon;
	};

}
