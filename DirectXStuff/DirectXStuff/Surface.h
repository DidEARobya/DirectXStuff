#pragma once
#include "WinInclude.h"
#include "WinException.h"
#include <string>
#include <memory>
#include <assert.h>

class Surface
{
public:
	class Exception : public WinException
	{
	public:
		Exception(int line, const char* file, std::string info) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetInfo() const noexcept;
	private:
		std::string _info;
	};

	class Colour
	{
	public:
		constexpr Colour() noexcept : dWord() {};
		constexpr Colour(const Colour& colour) noexcept : dWord(colour.dWord) {};
		constexpr Colour(unsigned int dword) noexcept : dWord(dword) {};
		constexpr Colour(unsigned char x, unsigned char r, unsigned char g, unsigned char b) : dWord((x << 24u) | (r << 16u) | (g << 8u) | b) {};

	public:
		unsigned int dWord;
	};

	Surface(unsigned int width, unsigned int height, unsigned int pitch) noexcept;
	Surface(unsigned int width, unsigned int height) noexcept;
	Surface(Surface&& source) noexcept;
	Surface(Surface&) = delete;
	Surface& operator = (Surface&& donor) noexcept;
	~Surface();

	void Clear(Colour fillvalue) const noexcept;
	void PutPixel(unsigned int x, unsigned int y, Colour colour) const noexcept(!IS_DEBUG);
	Colour GetPixel(unsigned int x, unsigned int y) const noexcept(!IS_DEBUG);
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	Colour* GetBufferPtr() const noexcept;
	const Colour* GetBufferPtrConst() const noexcept;
	static Surface FromFile(const std::string& name);
	void Save(const std::string& filename) const;
	void Copy(const Surface& src) noexcept(!IS_DEBUG);

private:
	Surface(unsigned int width, unsigned int height, std::unique_ptr<Colour[]> pBuffer) noexcept;

private: 
	std::unique_ptr<Colour[]> _pBuffer;
	unsigned int _width;
	unsigned int _height;
};

