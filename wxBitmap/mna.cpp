///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.0-68-g74b42be3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "mna.h"

///////////////////////////////////////////////////////////////////////////

FrameMain::FrameMain( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	m_timerUpdateScreen.SetOwner( this, wxID_ANY );
	m_timerUpdateScreen.Start( 14 );


	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( FrameMain::FrameMainOnLeftDClick ) );
	this->Connect( wxEVT_PAINT, wxPaintEventHandler( FrameMain::FrameMainOnPaint ) );
	this->Connect( wxID_ANY, wxEVT_TIMER, wxTimerEventHandler( FrameMain::m_timerUpdateScreenOnTimer ) );
}

FrameMain::~FrameMain()
{
	// Disconnect Events
	this->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( FrameMain::FrameMainOnLeftDClick ) );
	this->Disconnect( wxEVT_PAINT, wxPaintEventHandler( FrameMain::FrameMainOnPaint ) );
	this->Disconnect( wxID_ANY, wxEVT_TIMER, wxTimerEventHandler( FrameMain::m_timerUpdateScreenOnTimer ) );

}
