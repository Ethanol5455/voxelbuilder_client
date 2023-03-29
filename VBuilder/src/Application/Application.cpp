#include "vbpch.h"

#include <signal.h>

#include "Game/Game.h"

#include "Utils/PlatformUtils.h"

#ifdef VB_DEDICATED_GPU // Only defined on Windows
extern "C" {
_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

Game *game;

void HandleSignalCallback(int signum)
{
	std::cout << "Interrupt signal (" << signum << ") received.\n";

	if (!game)
		return;
	game->Stop();
}

int main()
{
	::Log::Init();

	signal(SIGINT, HandleSignalCallback);

	game = new Game();
	game->Run();
	delete game;
}