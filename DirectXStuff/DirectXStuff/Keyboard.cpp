#include "Keyboard.h"
#include "Mouse.h"

void Keyboard::ClearAllQueues() noexcept
{
	ClearKeyQueue();
	ClearCharQueue();
}

#pragma region "Key Events"
bool Keyboard::IsKeyPressed(unsigned char keycode) const noexcept
{
	return _keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (_keybuffer.size() > 0u)
	{
		Keyboard::Event event = _keybuffer.front();
		_keybuffer.pop();

		return event;
	}

	return Keyboard::Event();
}

bool Keyboard::IsKeyEmpty() const noexcept
{
	return _keybuffer.empty();
}

void Keyboard::ClearKeyQueue() noexcept
{
	_keybuffer = std::queue<Event>();
}
#pragma endregion

#pragma region "Char Events"
char Keyboard::ReadChar() noexcept
{
	if (_charbuffer.size() > 0u)
	{
		unsigned char charcode = _charbuffer.front();
		_charbuffer.pop();

		return charcode;
	}

	return 0;
}

bool Keyboard::IsCharEmpty() const noexcept
{
	return _charbuffer.empty();
}

void Keyboard::ClearCharQueue() noexcept
{
	_charbuffer = std::queue<char>();
}
#pragma endregion

#pragma region "AutoRepeat Control"
void Keyboard::EnableAutoRepeat() noexcept
{
	_isAutoRepeatEnabled = true;
}

void Keyboard::DisableAutoRepeat() noexcept
{
	_isAutoRepeatEnabled = false;
}

bool Keyboard::IsAutoRepeateEnabled() const noexcept
{
	return _isAutoRepeatEnabled;
}
#pragma endregion;

#pragma region "Messages"
void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	_keystates[keycode] = true;
	_keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Pressed, keycode));
	TrimBuffer(_keybuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	_keystates[keycode] = true;
	_keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Released, keycode));
	TrimBuffer(_keybuffer);
}

void Keyboard::OnChar(char character) noexcept
{
	_charbuffer.push(character);
	TrimBuffer(_charbuffer);
}

template<typename T>
inline void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > _bufferSize)
	{
		buffer.pop();
	}
}
#pragma endregion

void Keyboard::ClearKeyStates() noexcept
{
	_keystates.reset();
}

#pragma region "Event"
Keyboard::Event::Event() noexcept : _type(Type::Invalid), _code(0u)
{
}

Keyboard::Event::Event(Type type, unsigned char code) noexcept : _type(type), _code(code)
{
}

bool Keyboard::Event::IsPressed() const noexcept
{
	return _type == Type::Pressed;
}

bool Keyboard::Event::IsReleased() const noexcept
{
	return _type == Type::Released;
}
bool Keyboard::Event::IsValid() const noexcept
{
	return _type != Type::Invalid;
}
unsigned char Keyboard::Event::GetCode() const noexcept
{
	return _code;
}
#pragma endregion