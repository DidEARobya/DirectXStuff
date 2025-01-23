#pragma once
#include <queue>

class Mouse
{
	friend class Window;

public:
	class Event
	{
	public:
		enum class Type
		{
			LPressed,
			LReleased,
			RPressed,
			RReleased,
			WheelPressed,
			WheelReleased,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};

		Event() noexcept;
		Event(Type type, const Mouse& mouse) noexcept;

		bool IsValid() const noexcept;
		Type GetType() const noexcept;
		std::pair<int, int> GetPos() const noexcept;
		int GetPosX() const noexcept;
		int GetPosY() const noexcept;
		bool IsLeftPressed() const noexcept;
		bool IsRightPressed() const noexcept;
		bool IsWheelPressed() const noexcept;

	private:
		Type _type;
		bool _isLeftPressed;
		bool _isRightPressed;
		bool _isWheelPressed;
		int _x;
		int _y;
	};

	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator = (const Mouse&) = delete;

	//----INPUT----
	bool IsLeftPressed() const noexcept;
	bool IsRightPressed() const noexcept;
	bool IsWheelPressed() const noexcept;

	//----HELPER----
	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	Mouse::Event Read() noexcept;
	bool IsEmpty() const noexcept;
	void ClearQueue() noexcept;
private:
	//----MESSAGES----
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseEnter() noexcept;
	void OnMouseLeave() noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnWheelPressed(int x, int y) noexcept;
	void OnWheelReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void OnWheelDelta(int x, int y, int delta) noexcept;

	void TrimBuffer() noexcept;
private:
	static constexpr unsigned int _bufferSize = 16u;
	int _x;
	int _y;
	bool _isInWindow;
	bool _isLeftPressed;
	bool _isRightPressed;
	bool _isWheelPressed;
	int _wheelDeltaCarry = 0;
	std::queue<Event> _buffer;
};

