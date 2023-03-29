#pragma once

#include "Event.h"

class WindowResizeEvent : public Event {
    public:
	WindowResizeEvent(uint32_t width, uint32_t height)
		: m_Width(width)
		, m_Height(height)
	{
	}

	uint32_t GetWidth() const
	{
		return m_Width;
	}
	uint32_t GetHeight() const
	{
		return m_Height;
	}

	EVENT_CLASS_TYPE(WindowResize);

    private:
	uint32_t m_Width, m_Height;
};

class WindowCloseEvent : public Event {
    public:
	WindowCloseEvent() = default;

	EVENT_CLASS_TYPE(WindowClose);
};

class WindowContentScaleChangedEvent : public Event {
    public:
	WindowContentScaleChangedEvent(float xScale, float yScale)
		: m_xScale(xScale)
		, m_yScale(yScale)
	{
	}

	float GetXScale()
	{
		return m_xScale;
	}
	float GetYScale()
	{
		return m_yScale;
	}

	EVENT_CLASS_TYPE(WindowContentScaleChanged);

    private:
	float m_xScale, m_yScale;
};