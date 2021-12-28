#include "MyFrameMain.hpp"
#include <wx/rawbmp.h>
#include <wx/msgdlg.h>
#include <sstream>
#include <wx/dcclient.h> // For wxPaintDC
#include <wx/dcbuffer.h> // For wxBufferedPaintDC
#include <random>
#include <algorithm>
#include <array>
#include <thread>
#include <cmath>

#include <functional> // For std::ref

//#include <boost/multiprecision/cpp_complex.hpp>
#include <complex>

MyFrameMain::MyFrameMain(wxWindow* const parent, const wxWindowID id, const wxString& title) :
	FrameMain(parent, id, title, wxDefaultPosition, wxSize(1222, 512)), m_bitmap(1 /*Width*/, 1 /*Height*/, 24 /*Depth*/)
{
	// To avoid flickering
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	wxNativePixelData data(this->m_bitmap);
	if (!data)
	{
		wxMessageBox("Couldn\'t get access to native pixel data", "Error drawing");
		return;
	}
	else
	{
		const int data_width = data.GetWidth();
		const int data_height = data.GetHeight();

		// Zero out the buffer
		{
			wxNativePixelData::Iterator pixel(data);
			for (int column_index = 0; column_index < data_height; ++column_index, pixel.OffsetY(data, 1))
			{
				const wxNativePixelData::Iterator row_start = pixel;
				for (int row_index = 0; row_index < data_width; ++row_index, ++pixel)
				{
					pixel.Red() = 0;
					pixel.Green() = 0;
					pixel.Blue() = 0;
				}
				pixel = row_start;
			}
		}
	}
}

void MyFrameMain::start_main_loop()
{
	// Can't do this from the constructor in case the created thread needs to call a virtual function of this.
	// That's because accessing virtual functions before the object has finished constructing is undefined
	// behavior.
	this->stop_now = false;
	this->main_loop_thread = std::async(std::launch::async, this->main_loop, std::ref(*this));
}

void MyFrameMain::m_timerUpdateScreenOnTimer(wxTimerEvent& evnt)
{
	if (this->in_on_timer)
		return;
	this->in_on_timer = true;
	this->Refresh();
	//this->Update();
	this->in_on_timer = false;
}

void MyFrameMain::FrameMainOnPaint(wxPaintEvent& evnt)
{
	std::lock_guard<std::mutex> lckr(this->drawing_lock);
	const wxSize new_size = this->GetClientSize();
	if (new_size.x == 0 || new_size.y == 0)
		return;
	if (this->m_bitmap.GetSize() != new_size)
	{
		this->changed_dimensions = true;
	}
	//wxPaintDC dc(this);
	//wxBufferedPaintDC dc(this);
	// The difference between wxBufferedPaintDCand this class is that this class won't
	// double-buffer on platforms which have native double-buffering already,
	// avoiding any unnecessary buffering to avoid flicker.
	wxAutoBufferedPaintDC dc(this);
	dc.DrawBitmap
	(
		this->m_bitmap,
		dc.DeviceToLogicalX(0),
		dc.DeviceToLogicalY(0),
		true /* use mask */
	);
	{
		std::ostringstream text_to_show;
		text_to_show << "Magnification: " << this->m_magnification << "X";
		wxPen pen(*wxWHITE);
		dc.SetPen(pen);
		dc.DrawRectangle(3, 5, 170, 17);
		dc.SetTextForeground(wxColour(0, 0, 0));
		dc.DrawText(text_to_show.str(), wxPoint(5, 5));
	}
	evnt.Skip();
	//wxPen pen(*wxRED, 1); // red pen of width 1
	//dc.SetPen(pen);
	//dc.DrawPoint(wxPoint(100, 100));
	//dc.SetPen(wxNullPen);
}

//static unsigned compute_mandel_pixel_color(const fl& x0, const fl& y0, const fl& len_x, const fl& len_y)
//{
//	fl x2 = 0;
//	fl y2 = 0;
//	fl w = 0;
//	const fl max = len_x * len_y;
//	unsigned iteration = 0;
//	constexpr unsigned max_iteration = 10000;
//	for (; x2 + y2 <= max && iteration < max_iteration; ++iteration)
//	{
//		const fl x = x2 - y2 + x0;
//		const fl y = w - x2 - y2 + y0;
//		x2 = x * x;
//		y2 = y * y;
//		w = (x + y) * (x + y);
//	}
//	return static_cast<unsigned>(static_cast<fl>(iteration) * (static_cast<fl>(0xffffff) / static_cast<fl>(max_iteration)));
//}
constexpr unsigned rgb(const unsigned r, const unsigned g, const unsigned b)
{
	return (r << 16) + (g << 8) + b;
}
constexpr unsigned pallet[] = {
rgb(66, 30, 15),// # brown 3
rgb(25,7,26),// # dark violett
rgb(9,1,47),// # darkest blue
rgb(4,4,73),// # blue 5
rgb(0   ,7,100),// # blue 4
rgb(12 , 44 ,138),// # blue 3
rgb(24 , 82, 177),// # blue 2
rgb(57 ,125 ,209),// # blue 1
rgb(134 ,181, 229),// # blue 0
rgb(211 ,236, 248),// # lightest blue
rgb(241, 233, 191),// # lightest yellow
rgb(248, 201,  95),// # light yellow
rgb(255, 170 , 0),// # dirty yellow
rgb(204, 128  , 0),// # brown 0
rgb(153,  87  , 0),// # brown 1
rgb(106,  52  , 3),// # brown 2
};
//constexpr unsigned pallet[] = {
//0x800000U,//maroon
//0x8B0000U,//dark red
//0xA52A2AU,//brown
//0xB22222U,//firebrick
//0xDC143CU,//crimson
//0xFF0000U,//red
//0xFF6347U,//tomato
//0xFF7F50U,//coral
//0xCD5C5CU,//indian red
//0xF08080U,//light coral
//0xE9967AU,//dark salmon
//0xFA8072U,//salmon
//0xFFA07AU,//light salmon
//0xFF4500U,//orange red
//0xFF8C00U,//dark orange
//0xFFA500U,//orange
//0xFFD700U,//gold
//0xB8860BU,//dark golden rod
//0xDAA520U,//golden rod
//0xEEE8AAU,//pale golden rod
//0xBDB76BU,//dark khaki
//0xF0E68CU,//khaki
//0x808000U,//olive
//0xFFFF00U,//yellow
//0x9ACD32U,//yellow green
//0x556B2FU,//dark olive green
//0x6B8E23U,//olive drab
//0x7CFC00U,//lawn green
//0x7FFF00U,//chart reuse
//0xADFF2FU,//green yellow
//0x006400U,//dark green
//0x008000U,//green
//0x228B22U,//forest green
//0x00FF00U,//lime
//0x32CD32U,//lime green
//0x90EE90U,//light green
//0x98FB98U,//pale green
//0x8FBC8FU,//dark sea green
//0x00FA9AU,//medium spring green
//0x00FF7FU,//spring green
//0x2E8B57U,//sea green
//0x66CDAAU,//medium aqua marine
//0x3CB371U,//medium sea green
//0x20B2AAU,//light sea green
//0x2F4F4FU,//dark slate gray
//0x008080U,//teal
//0x008B8BU,//dark cyan
//0x00FFFFU,//aqua
//0x00FFFFU,//cyan
//0xE0FFFFU,//light cyan
//0x00CED1U,//dark turquoise
//0x40E0D0U,//turquoise
//0x48D1CCU,//medium turquoise
//0xAFEEEEU,//pale turquoise
//0x7FFFD4U,//aqua marine
//0xB0E0E6U,//powder blue
//0x5F9EA0U,//cadet blue
//0x4682B4U,//steel blue
//0x6495EDU,//corn flower blue
//0x00BFFFU,//deep sky blue
//0x1E90FFU,//dodger blue
//0xADD8E6U,//light blue
//0x87CEEBU,//sky blue
//0x87CEFAU,//light sky blue
//0x191970U,//midnight blue
//0x000080U,//navy
//0x00008BU,//dark blue
//0x0000CDU,//medium blue
//0x0000FFU,//blue
//0x4169E1U,//royal blue
//0x8A2BE2U,//blue violet
//0x4B0082U,//indigo
//0x483D8BU,//dark slate blue
//0x6A5ACDU,//slate blue
//0x7B68EEU,//medium slate blue
//0x9370DBU,//medium purple
//0x8B008BU,//dark magenta
//0x9400D3U,//dark violet
//0x9932CCU,//dark orchid
//0xBA55D3U,//medium orchid
//0x800080U,//purple
//0xD8BFD8U,//thistle
//0xDDA0DDU,//plum
//0xEE82EEU,//violet
//0xFF00FFU,//magenta / fuchsia
//0xDA70D6U,//orchid
//0xC71585U,//medium violet red
//0xDB7093U,//pale violet red
//0xFF1493U,//deep pink
//0xFF69B4U,//hot pink
//0xFFB6C1U,//light pink
//0xFFC0CBU,//pink
//0xFAEBD7U,//antique white
//0xF5F5DCU,//beige
//0xFFE4C4U,//bisque
//0xFFEBCDU,//blanched almond
//0xF5DEB3U,//wheat
//0xFFF8DCU,//corn silk
//0xFFFACDU,//lemon chiffon
//0xFAFAD2U,//light golden rod yellow
//0xFFFFE0U,//light yellow
//0x8B4513U,//saddle brown
//0xA0522DU,//sienna
//0xD2691EU,//chocolate
//0xCD853FU,//peru
//0xF4A460U,//sandy brown
//0xDEB887U,//burly wood
//0xD2B48CU,//tan
//0xBC8F8FU,//rosy brown
//0xFFE4B5U,//moccasin
//0xFFDEADU,//navajo white
//0xFFDAB9U,//peach puff
//0xFFE4E1U,//misty rose
//0xFFF0F5U,//lavender blush
//0xFAF0E6U,//linen
//0xFDF5E6U,//old lace
//0xFFEFD5U,//papaya whip
//0xFFF5EEU,//sea shell
//0xF5FFFAU,//mint cream
//0x708090U,//slate gray
//0x778899U,//light slate gray
//0xB0C4DEU,//light steel blue
//0xE6E6FAU,//lavender
//0xFFFAF0U,//floral white
//0xF0F8FFU,//alice blue
//0xF8F8FFU,//ghost white
//0xF0FFF0U,//honeydew
//0xFFFFF0U,//ivory
//0xF0FFFFU,//azure
//0xFFFAFAU,//snow
//0x000000U,//black
//0x696969U,//dim gray / dim grey
//0x808080U,//gray / grey
//0xA9A9A9U,//dark gray / dark grey
//0xC0C0C0U,//silver
//0xD3D3D3U,//light gray / light grey
//0xDCDCDCU,//gainsboro
//0xF5F5F5U,//white smoke
//0xFFFFFFU//white
//};
//static unsigned compute_mandel_pixel_color(const fl& x0, const fl& y0)
//{
//	fl x = 0;
//	fl y = 0;
//	unsigned iteration = 0;
//	constexpr unsigned max_iteration = 2000;
//	while (x * x + y * y <= 4 && iteration < max_iteration)
//	{
//		const fl xtemp = x * x - y * y + x0;
//		y = 2 * x * y + y0;
//		x = xtemp;
//		iteration = iteration + 1;
//	}
//	return pallet[iteration % (sizeof(pallet) / sizeof(pallet[0]))];
//}
static unsigned compute_mandel_pixel_color(const fl& x0, const fl& y0)
{
	// boost::multiprecision::cpp_complex_100
	std::complex<double> c(static_cast<double>(x0), static_cast<double>(y0));
	std::complex<double> z = 0;
	unsigned iteration = 0;
	constexpr unsigned max_iteration = 2000;
	// boost::multiprecision::abs
	for ( ; std::abs(z - c) < 2 && iteration < max_iteration; ++iteration)
	{
		z = z * z + c;
	}
	return pallet[iteration % (sizeof(pallet) / sizeof(pallet[0]))];
}
void MyFrameMain::separate_thread_to_compute_rows(wxNativePixelData& data, wxNativePixelData::Iterator pixel, const unsigned y_start, const unsigned rows_to_compute, MyFrameMain& this_ref, wxSize frame_size)
{
	const int y_index_until_not_including = y_start + rows_to_compute;
	const fl len_x_frame = frame_size.x;
	const fl len_y_frame = frame_size.y;
	if (len_y_frame < y_index_until_not_including)
	{
		throw std::invalid_argument("Trying to draw more rows than exist");
	}
	auto frame_x_to_mandel = [len_x_frame, &this_ref](const fl& x_frame) -> fl
	{
		return this_ref.m_start_x_mandel + this_ref.m_len_x_mandel * (x_frame / len_x_frame);
	};
	auto frame_y_to_mandel = [len_y_frame, &this_ref](const fl& y_frame) -> fl
	{
		return this_ref.m_start_y_mandel + this_ref.m_len_y_mandel * (y_frame / len_y_frame);
	};
	while (!this_ref.changed_dimensions)
	{
		const wxNativePixelData::Iterator pixel_start = pixel;
		for (int y = y_start; y < y_index_until_not_including && !this_ref.changed_dimensions; ++y, pixel.OffsetY(data, 1))
		{
			const fl current_y_mandel = frame_y_to_mandel(frame_size.y - y);
			const wxNativePixelData::Iterator row_start = pixel;
			for (int x = 0; x < frame_size.x && !this_ref.changed_dimensions; ++x, ++pixel)
			{
				const fl current_x_mandel = frame_x_to_mandel(x);
				const unsigned pixel_color = compute_mandel_pixel_color(current_x_mandel, current_y_mandel);
				//pixel_color *= pixel_color;
				//pixel_color /= 2;
				//pixel_color = std::min(pixel_color, 0xffffffU);
				pixel.Red() = (pixel_color & 0xff0000) >> 16;
				pixel.Green() = (pixel_color & 0x00ff00) >> 8;
				pixel.Blue() = (pixel_color & 0x0000ff) >> 0;
			}
			pixel = row_start;
		}
		pixel = pixel_start;
	}
}
void MyFrameMain::main_loop(MyFrameMain& this_ref)
{
	std::random_device truly_random;
	std::default_random_engine rand_algo(truly_random());
	std::uniform_int_distribution<int> range(0, 255);
	while (!this_ref.stop_now)
	{
		wxSize frame_size;
		{
			std::lock_guard<std::mutex> lckr(this_ref.drawing_lock);
			frame_size = this_ref.GetClientSize();
			if (this_ref.m_bitmap.GetSize() != frame_size && frame_size.x > 0 && frame_size.y > 0)
			{
				this_ref.m_bitmap = wxBitmap(frame_size.x, frame_size.y, 24 /* Depth */);
			}
			this_ref.changed_dimensions = false;
		}
		wxNativePixelData data(this_ref.m_bitmap);
		if (!data)
		{
			wxMessageBox("Couldn\'t get access to native pixel data", "Error drawing");
		}
		else
		{
			const unsigned num_cores = std::max<unsigned>(std::thread::hardware_concurrency() / 2U, 1U);
			std::vector<std::future<void>> threads;
			threads.reserve(num_cores);
			wxNativePixelData::Iterator pixel(data);
			const unsigned num_rows_for_each_thread = frame_size.y / num_cores;
			const unsigned num_rows_for_last_thread = frame_size.y - num_rows_for_each_thread * (num_cores - 1);
			for (unsigned core_index = 0; core_index < num_cores; ++core_index)
			{
				const unsigned num_rows_for_current_thread = ((core_index == num_cores - 1) ? num_rows_for_last_thread : num_rows_for_each_thread);
				threads.push_back(std::async(std::launch::async, separate_thread_to_compute_rows, std::ref(data), pixel, core_index * num_rows_for_each_thread, num_rows_for_current_thread, std::ref(this_ref), frame_size));
				pixel.OffsetY(data, num_rows_for_current_thread);
			}
		}
	}
}

void MyFrameMain::FrameMainOnLeftDClick(wxMouseEvent& evnt)
{
	wxPoint click_position;
	wxSize window_size;
	{
		std::lock_guard<std::mutex> lckr(this->drawing_lock);
		click_position = evnt.GetPosition();
		window_size = this->m_bitmap.GetSize();
	}
	const fl ratio_x = static_cast<fl>(click_position.x) / static_cast<fl>(window_size.x);
	const fl ratio_y = static_cast<fl>(window_size.y - click_position.y) / static_cast<fl>(window_size.y);
	const fl new_center_x_mandle = this->m_start_x_mandel + this->m_len_x_mandel * ratio_x;
	const fl new_center_y_mandle = this->m_start_y_mandel + this->m_len_y_mandel * ratio_y;

	this->m_len_x_mandel *= this->m_zoom;
	this->m_len_y_mandel *= this->m_zoom;
	this->m_start_x_mandel = new_center_x_mandle - this->m_len_x_mandel / 2;
	this->m_start_y_mandel = new_center_y_mandle - this->m_len_y_mandel / 2;

	this->m_magnification /= static_cast<boost::multiprecision::cpp_bin_float_100>(this->m_zoom);
}

MyFrameMain::~MyFrameMain()
{
	this->stop_now = true;
	this->changed_dimensions = true;
	// Will happen in any case
	//this->main_loop_thread.get();
}
