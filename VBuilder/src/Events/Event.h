#pragma once

enum class EventType {
	None = 0,
	WindowResize,
	WindowClose,
	WindowFocus,
	WindowLostFocus,
	WindowMoved,
	WindowContentScaleChanged,
	KeyPressed,
	KeyReleased,
	KeyTyped,
	MouseButtonPressed,
	MouseButtonReleased,
	MouseMoved,
	MouseScrolled
};

#define EVENT_CLASS_TYPE(type)                          \
	static EventType GetStaticType()                \
	{                                               \
		return EventType::type;                 \
	}                                               \
	virtual EventType GetEventType() const override \
	{                                               \
		return GetStaticType();                 \
	}

class Event {
    public:
	virtual ~Event() = default;

	bool Handled = false;

	virtual EventType GetEventType() const = 0;
};

class EventDispatcher {
    public:
	EventDispatcher(Event &event)
		: m_Event(event)
	{
	}

	// F will be deduced by the compiler
	template <typename T, typename F> bool Dispatch(const F &func)
	{
		if (m_Event.GetEventType() == T::GetStaticType()) {
			m_Event.Handled = func(static_cast<T &>(m_Event));
			return true;
		}
		return false;
	}

    private:
	Event &m_Event;
};