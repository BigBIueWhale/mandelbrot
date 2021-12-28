#include "MyApp.hpp"

// Evil macro that defines main and starts the
// message loop for me. Handles the platform-specific things.
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	this->m_frame_main = new MyFrameMain(nullptr, wxID_ANY, "wxBitmapWindow");
	this->m_frame_main->Show();
	this->m_frame_main->start_main_loop();
	return true;
}
