#pragma once

#include "Event.h"

#include <GLFW/glfw3.h>

class KeyEvent : public Event {
    public:
	int GetKeyCode() const
	{
		return m_KeyCode;
	}

    protected:
	KeyEvent(int keyCode)
		: m_KeyCode(keyCode)
	{
	}

	int m_KeyCode;
};

class KeyPressedEvent : public KeyEvent {
    public:
	KeyPressedEvent(int keyCode, int repeatCount)
		: KeyEvent(keyCode)
		, m_RepeatCount(repeatCount)
	{
	}

	int GetRepeatCount() const
	{
		return m_RepeatCount;
	}

	EVENT_CLASS_TYPE(KeyPressed);

    private:
	int m_RepeatCount;
};

class KeyReleasedEvent : public KeyEvent {
    public:
	KeyReleasedEvent(int keyCode)
		: KeyEvent(keyCode)
	{
	}

	EVENT_CLASS_TYPE(KeyReleased);
};

class KeyTypedEvent : public KeyEvent {
	KeyTypedEvent(int keyCode)
		: KeyEvent(keyCode)
	{
	}

	EVENT_CLASS_TYPE(KeyTyped);
};