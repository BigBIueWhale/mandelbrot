#ifndef MYFRAMEMAIN_HPP
#define MYFRAMEMAIN_HPP

#include "mna.h"
#include <mutex>
#include <future>

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <wx/rawbmp.h>

using fl = long double; //boost::multiprecision::cpp_bin_float_100;

class MyFrameMain :
    public FrameMain
{
    wxBitmap m_bitmap;
    bool in_on_timer = false;
    void m_timerUpdateScreenOnTimer(wxTimerEvent& evnt) override;
    void FrameMainOnPaint(wxPaintEvent& evnt) override;
    std::mutex drawing_lock;
    std::future<void> main_loop_thread;
    volatile bool stop_now = false;
    volatile bool changed_dimensions = false;
    static void separate_thread_to_compute_rows(wxNativePixelData& data, wxNativePixelData::Iterator pixel, const unsigned y_start, const unsigned rows_to_compute, MyFrameMain& this_ref, wxSize frame_size);
    static void main_loop(MyFrameMain& this_ref);
    void FrameMainOnLeftDClick(wxMouseEvent& evnt) override;
    boost::multiprecision::cpp_bin_float_100 m_magnification = 1;
    fl m_zoom = 0.5;
    fl m_start_x_mandel = -2;
    fl m_start_y_mandel = -1;
    fl m_len_x_mandel = 3;
    fl m_len_y_mandel = 2;
public:
    void start_main_loop();
    MyFrameMain(wxWindow *const parent, const wxWindowID id, const wxString& title);
    ~MyFrameMain() override;
};

#endif
