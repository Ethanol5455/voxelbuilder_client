#ifndef VB_PLATFORM_LINUX
#pragma once
#endif

#include <enet/enet.h>

#ifdef VB_PLATFORM_WINDOWS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>

#include "Core/Base.h"

#include "Core/Log.h"