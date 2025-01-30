#define FULL_WIN
#include "Surface.h"
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>
#include <sstream>

#pragma comment( lib,"gdiplus.lib" )

Surface::Surface(unsigned int width, unsigned int height, unsigned int pitch) noexcept 
{
}

Surface::Surface(unsigned int width, unsigned int height) noexcept : _pBuffer(std::make_unique<Colour[]>(width * height)), _width(width), _height(height)
{
}

Surface::Surface(Surface&& source) noexcept : _pBuffer(std::move(source._pBuffer)), _width(source._width), _height(source._height)
{
}

Surface& Surface::operator=(Surface&& donor) noexcept
{
	_width = donor._width;
	_height = donor._height;
	_pBuffer = std::move(donor._pBuffer);
	donor._pBuffer = nullptr;

	return *this;
}

Surface::~Surface()
{
}

void Surface::Clear(Colour fillvalue) const noexcept
{
	memset(_pBuffer.get(), fillvalue.dWord, _width * _height * sizeof(Colour));
}

void Surface::PutPixel(unsigned int x, unsigned int y, Colour colour) const noexcept(!IS_DEBUG)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < _width);
	assert(y < _height);

	_pBuffer[y * _width + x] = colour;
}

Surface::Colour Surface::GetPixel(unsigned int x, unsigned int y) const noexcept(!IS_DEBUG)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < _width);
	assert(y < _height);

	return _pBuffer[y * _width + x];
}

unsigned int Surface::GetWidth() const noexcept
{
	return _width;
}

unsigned int Surface::GetHeight() const noexcept
{
	return _height;
}

Surface::Colour* Surface::GetBufferPtr() const noexcept
{
	return _pBuffer.get();
}

const Surface::Colour* Surface::GetBufferPtrConst() const noexcept
{
	return _pBuffer.get();
}

Surface Surface::FromFile(const std::string& name)
{
	unsigned int width = 0;
	unsigned int height = 0;
	std::unique_ptr<Colour[]> pBuffer;

	{
		// convert filenam to wide string (for Gdiplus)
		wchar_t wideName[512];
		mbstowcs_s(nullptr, wideName, name.c_str(), _TRUNCATE);

		Gdiplus::Bitmap bitmap(wideName);
		if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
		{
			std::stringstream ss;
			ss << "Loading image [" << name << "]: failed to load.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		width = bitmap.GetWidth();
		height = bitmap.GetHeight();
		pBuffer = std::make_unique<Colour[]>(width * height);

		for (unsigned int y = 0; y < height; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				Gdiplus::Color c;
				bitmap.GetPixel(x, y, &c);
				pBuffer[y * width + x] = c.GetValue();
			}
		}
	}

	return Surface(width, height, std::move(pBuffer));
}

void Surface::Save(const std::string& filename) const
{
	auto GetEncoderClsid = [&filename](const WCHAR* format, CLSID* pClsid) -> void
		{
			UINT  num = 0;          // number of image encoders
			UINT  size = 0;         // size of the image encoder array in bytes

			Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

			Gdiplus::GetImageEncodersSize(&num, &size);
			if (size == 0)
			{
				std::stringstream ss;
				ss << "Saving surface to [" << filename << "]: failed to get encoder; size == 0.";
				throw Exception(__LINE__, __FILE__, ss.str());
			}

			pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
			if (pImageCodecInfo == nullptr)
			{
				std::stringstream ss;
				ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to allocate memory.";
				throw Exception(__LINE__, __FILE__, ss.str());
			}

			GetImageEncoders(num, size, pImageCodecInfo);

			for (UINT j = 0; j < num; ++j)
			{
				if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
				{
					*pClsid = pImageCodecInfo[j].Clsid;
					free(pImageCodecInfo);
					return;
				}
			}

			free(pImageCodecInfo);
			std::stringstream ss;
			ss << "Saving surface to [" << filename <<
				"]: failed to get encoder; failed to find matching encoder.";
			throw Exception(__LINE__, __FILE__, ss.str());
		};

	CLSID bmpID;
	GetEncoderClsid(L"image/bmp", &bmpID);


	// convert filenam to wide string (for Gdiplus)
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	Gdiplus::Bitmap bitmap(_width, _height, _width * sizeof(Colour), PixelFormat32bppARGB, (BYTE*)_pBuffer.get());
	if (bitmap.Save(wideName, &bmpID, nullptr) != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: failed to save.";
		throw Exception(__LINE__, __FILE__, ss.str());
	}
}

void Surface::Copy(const Surface& src) noexcept(!IS_DEBUG)
{
	assert(_width == src._width);
	assert(_height == src._height);
	memcpy(_pBuffer.get(), src._pBuffer.get(), _width * _height * sizeof(Colour));
}

Surface::Surface(unsigned int width, unsigned int height, std::unique_ptr<Colour[]> pBuffer) noexcept : _width(width), _height(height), _pBuffer(std::move(pBuffer))
{
}

#pragma region "Exception"
Surface::Exception::Exception(int line, const char* file, std::string info) noexcept : WinException(line, file), _info(info)
{
}

const char* Surface::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << WinException::what() << std::endl
		<< "[Info] " << GetInfo();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Surface::Exception::GetType() const noexcept
{
	return "Graphics Exception";
}

const std::string& Surface::Exception::GetInfo() const noexcept
{
	return _info;
}
#pragma endregion


