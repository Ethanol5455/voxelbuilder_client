#pragma once

#include "Core/Timer.h"
#include "Core/Window.h"
#include "Core/Input.h"
#include "Events/Event.h"
#include "Events/MouseEvent.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBufferLayout.h"
#include "Renderer/Texture.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/WorldRenderer.h"
#include "Renderer/TextRenderer.h"

#include "Player/Player.h"
#include "World/Map.h"
#include "Util/RayCast.h"
#include "Util/AssetInfo.h"
#include "Utils/PlatformUtils.h"

#include "Networking/Packets.h"

class Game {
    public:
	Game() = default;
	~Game() = default;

	void Run();

	void Stop()
	{
		m_Running = false;
	}

    private:
	int OnInit();
	void OnUpdate();
	void OnTerminate();

	void OnEvent(Event &e);
	bool OnWindowClose(WindowCloseEvent &e);
	bool OnWindowResize(WindowResizeEvent &e);
	bool OnMouseButtonPressedEvent(MouseButtonPressedEvent &e);
	bool OnKeyReleasedEvent(KeyReleasedEvent &e);
	bool
	OnWindowContentScaleChangedEvent(WindowContentScaleChangedEvent &e);

    private:
	bool m_Running = true;
	bool m_Minimized = false;
	bool m_HasInitialized = false;

	Window *m_Window;

	float m_DeltaTime = 0.0f;
	Timer m_DeltaTimer;

	uint32_t m_FrameCount = 0;
	Timer m_FPSTimer;
	uint32_t m_FPS = 0;

	glm::vec2 m_Resolution = { 1920, 1080 };

	std::unique_ptr<Texture> m_MapTextureAtlas;
	std::unique_ptr<Texture> m_UtilTextureAtlas;

	Timer m_PlayerServerUpdateTimer;
	bool m_PlayerInitialized = false;
	std::unique_ptr<Player> m_Player;
	std::vector<PlayerActionRequest> m_PlayerActionRequests;
	glm::mat4 m_Projection;
	glm::mat4 m_View;

	std::unique_ptr<Framebuffer> m_FrameBuffer;
	std::unique_ptr<Shader> m_FBShader;
	std::unique_ptr<VertexArray> m_FBVA;

	std::unique_ptr<Shader> m_TextureShader;

	std::unique_ptr<VertexArray> m_IndicatorVA;
	std::unique_ptr<Texture> m_IndicatorTexture;

	ENetAddress m_Address;
	ENetHost *m_Client;
	ENetPeer *m_Server;

	std::shared_ptr<Map> m_Map;

	uint32_t m_DebugTextSize = 50;
	bool m_ShowStats = true;

	float m_CPUTimeAcc, m_GPUTimeAcc;
	float m_CPUTime, m_GPUTime;
};