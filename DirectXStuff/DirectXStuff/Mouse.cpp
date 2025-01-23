#include "Mouse.h"
#include "Windows.h"

#pragma region "Inputs"
bool Mouse::IsLeftPressed() const noexcept
{
	return _isLeftPressed;
}

bool Mouse::IsRightPressed() const noexcept
{
	return _isRightPressed;
}

bool Mouse::IsWheelPressed() const noexcept
{
	return _isWheelPressed;
}
#pragma endregion

#pragma region "Helper"
std::pair<int, int> Mouse::GetPos() const noexcept
{
	return { _x, _y };
}

int Mouse::GetPosX() const noexcept
{
	return _x;
}

int Mouse::GetPosY() const noexcept
{
	return _y;
}

bool Mouse::IsInWindow() const noexcept
{
	return _isInWindow;
}

Mouse::Event Mouse::Read() noexcept
{
	if (_buffer.size() > 0u)
	{
		Mouse::Event event = _buffer.front();
		_buffer.pop();

		return event;
	}

	return Mouse::Event();
}

bool Mouse::IsEmpty() const noexcept
{
	return _buffer.empty();
}

void Mouse::ClearQueue() noexcept
{
}
#pragma endregion

#pragma region "Messages"
void Mouse::OnMouseMove(int x, int y) noexcept
{
	_x = x;
	_y = y;

	_buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	_isInWindow = true;
	_buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	_isInWindow = false;
	_buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
	_isLeftPressed = true;

	_buffer.push(Mouse::Event(Mouse::Event::Type::LPressed, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
	_isLeftPressed = false;

	_buffer.push(Mouse::Event(Mouse::Event::Type::LReleased, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
	_isRightPressed = true;

	_buffer.push(Mouse::Event(Mouse::Event::Type::RPressed, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
	_isRightPressed = false;

	_buffer.push(Mouse::Event(Mouse::Event::Type::RReleased, *this));
	TrimBuffer();
}

void Mouse::OnWheelPressed(int x, int y) noexcept
{
	_isWheelPressed = true;

	_buffer.push(Mouse::Event(Mouse::Event::Type::WheelPressed, *this));
	TrimBuffer();
}

void Mouse::OnWheelReleased(int x, int y) noexcept
{
	_isWheelPressed = false;

	_buffer.push(Mouse::Event(Mouse::Event::Type::WheelReleased, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
	_buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();

}

void Mouse::OnWheelDown(int x, int y) noexcept
{
	_buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	_wheelDeltaCarry += delta;

	//Generates Mouse Scroll event every 120 delta
	while (_wheelDeltaCarry >= WHEEL_DELTA)
	{
		_wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}

	while (_wheelDeltaCarry <= -WHEEL_DELTA)
	{
		_wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}

void Mouse::TrimBuffer() noexcept
{
	while (_buffer.size() > _bufferSize)
	{
		_buffer.pop();
	}
}
#pragma endregion

#pragma region "Event"
Mouse::Event::Event() noexcept : _type(Type::Invalid), _isLeftPressed(false), _isRightPressed(false), _isWheelPressed(false), _x(0), _y(0)
{

}

Mouse::Event::Event(Type type, const Mouse& mouse) noexcept : _type(type), _isLeftPressed(mouse._isLeftPressed), _isRightPressed(mouse._isRightPressed), _isWheelPressed(mouse._isWheelPressed),
_x(mouse._x), _y(mouse._y)
{

}

bool Mouse::Event::IsValid() const noexcept
{
	return _type != Type::Invalid;
}

Mouse::Event::Type Mouse::Event::GetType() const noexcept
{
	return _type;
}

std::pair<int, int> Mouse::Event::GetPos() const noexcept
{
	return { _x, _y };
}
int Mouse::Event::GetPosX() const noexcept
{
	return _x;
}
int Mouse::Event::GetPosY() const noexcept
{
	return _y;
}
bool Mouse::Event::IsLeftPressed() const noexcept
{
	return _isLeftPressed;
}
bool Mouse::Event::IsRightPressed() const noexcept
{
	return _isRightPressed;
}
bool Mouse::Event::IsWheelPressed() const noexcept
{
	return _isWheelPressed;
}
#pragma endregion