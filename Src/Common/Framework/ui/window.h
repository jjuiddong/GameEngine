#pragma once


namespace framework
{

	class cWindow : public framework::cEventHandler
							, public graphic::cSprite
	{
	public:
		cWindow(LPD3DXSPRITE sprite, const int id, const string &name="window");
		virtual ~cWindow();

		virtual bool MessageProc( UINT message, WPARAM wParam, LPARAM lParam);
		

	protected:
		bool DispatchEvent( cEvent &event );

		bool m_isVisible;
	};
}
