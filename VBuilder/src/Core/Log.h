#pragma once

#include "Base.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

class Log {
    public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger> &GetLogger()
	{
		return s_Logger;
	}

    private:
	static std::shared_ptr<spdlog::logger> s_Logger;
};

// Game log macros
#define VB_TRACE(...) ::Log::GetLogger()->trace(__VA_ARGS__)
#define VB_INFO(...) ::Log::GetLogger()->info(__VA_ARGS__)
#define VB_WARN(...) ::Log::GetLogger()->warn(__VA_ARGS__)
#define VB_ERROR(...) ::Log::GetLogger()->error(__VA_ARGS__)