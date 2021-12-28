#pragma once

#include <wx/wx.h>

#include "MyFrameMain.hpp"

class MyApp :
    public wxApp
{
    MyFrameMain* m_frame_main = nullptr;
public:
    bool OnInit();
};

