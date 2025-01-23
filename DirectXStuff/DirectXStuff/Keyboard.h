#pragma once
#include <Queue>
#include <bitset>

class Keyboard
{
	friend class Window;

public:
	class Event
	{
	public:
		enum class Type
		{
			Pressed,
			Released,
			Invalid
		};

		Event() noexcept;
		Event(Type type, unsigned char code) noexcept;

		bool IsPressed() const noexcept;
		bool IsReleased() const noexcept;
		bool IsValid() const noexcept;
		unsigned char GetCode() const noexcept;
	private:
		Type _type;
		unsigned char _code;
	};

public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator = (const Keyboard&) = delete;

	void ClearAllQueues() noexcept;

	//---- KEY EVENTS ----
	bool IsKeyPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;
	bool IsKeyEmpty() const noexcept;
	void ClearKeyQueue() noexcept;

	//---- CHAR EVENTS ----
	char ReadChar() noexcept;
	bool IsCharEmpty() const noexcept;
	void ClearCharQueue() noexcept;

	//---- AUTOREPEAT ----
	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool IsAutoRepeateEnabled() const noexcept;

private:
	//----MESSAGES----
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearKeyStates() noexcept;

	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

private:
	static constexpr unsigned int _nKeys = 256u;
	static constexpr unsigned int _bufferSize = 16u;
	bool _isAutoRepeatEnabled = false;
	std::bitset<_nKeys> _keystates;
	std::queue<Event> _keybuffer;
	std::queue<char> _charbuffer;
};


