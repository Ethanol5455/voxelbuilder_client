#include "vbpch.h"
#include "Game.h"

#include <sstream>

#include <glm/gtc/matrix_transform.hpp>

using namespace std::chrono_literals;

void Game::Run()
{
	if (OnInit() == 0)
		return;
	while (m_Running) {
		m_DeltaTime = m_DeltaTimer.GetElapsedTime().AsSeconds();
		m_DeltaTimer.Restart();

		OnUpdate();
	}
	OnTerminate();
}

int Game::OnInit()
{
	if (enet_initialize() != 0) {
		VB_ERROR("An error occurred while initializing ENet.");
		return 0;
	}
	atexit(enet_deinitialize);

	m_Client = enet_host_create(NULL, 1, 2, 0, 0);
	if (m_Client == NULL) {
		VB_ERROR(
			"An error occurred while trying to create an ENet client host.");
		return 0;
	}

	ENetEvent event;

	enet_address_set_host(&m_Address, "localhost");
	m_Address.port = 1234;

	m_Server = enet_host_connect(m_Client, &m_Address, 2, 0);
	if (m_Server == NULL) {
		VB_ERROR(
			"No available peers for initiating an ENet connection.");
		return 0;
	}

	if (enet_host_service(m_Client, &event, 5000) > 0 &&
	    event.type == ENET_EVENT_TYPE_CONNECT) {
		VB_INFO("Connected to {0}:{1}", m_Address.host, m_Address.port);
	} else {
		enet_peer_reset(m_Server);
		VB_WARN("Unable to connect to {0}:{1}", m_Address.host,
			m_Address.port);
		return 0;
	}

	std::vector<uint8_t> packetData = AssemblePlayerInfoRequest("Ethan");
	ENetPacket *packet = enet_packet_create(
		&packetData[0], packetData.size(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_Server, 0, packet);

	Timer initTimer;

	m_Window = new Window("VBuilder", 1920, 1080);
	m_Window->SetEventCallback(VB_BIND_EVENT_FN(Game::OnEvent));
	m_Resolution = m_Window->GetResolution();
	Input::SetWindow(&*m_Window);

	m_Window->SetVSync(true);
	m_Window->Maximize();

	glfwSetInputMode(*m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// glfwSetInputMode(*m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Needed for debugging on Linux otherwise cursor will be unusable

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	TextRenderer::Init();

	{
		WindowContentScaleChangedEvent e = {
			m_Window->GetXContentScale(),
			m_Window->GetYContentScale()
		};
		OnWindowContentScaleChangedEvent(e);
	}

	m_MapTextureAtlas =
		std::make_unique<Texture>("res/textures/textureAtlas.png");
	m_UtilTextureAtlas =
		std::make_unique<Texture>("res/textures/utilAtlas.png");

	m_TextureShader =
		std::make_unique<Shader>("res/shaders/TextureVertex.glsl",
					 "res/shaders/TextureFragment.glsl");

	WorldRenderer::Init(m_TextureShader.get());

	// Framebuffer Setup
	{
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8,
				       FramebufferTextureFormat::Depth };
		fbSpec.Width = (uint32_t)m_Window->GetResolution().x;
		fbSpec.Height = (uint32_t)m_Window->GetResolution().y;
		fbSpec.Samples = 1;
		m_FrameBuffer = std::make_unique<Framebuffer>(fbSpec);

		m_FBShader =
			std::make_unique<Shader>("res/shaders/FBVertex.glsl",
						 "res/shaders/FBFragment.glsl");
		m_FBShader->Bind();
		m_FBShader->SetUniform1i("u_Texture", 1);

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);

		float positions[] = {
			-1.0f, -1.0f, 0.0f, 0.0f, // 0
			1.0f,  -1.0f, 1.0f, 0.0f, // 1
			1.0f,  1.0f,  1.0f, 1.0f, // 2
			-1.0f, 1.0f,  0.0f, 1.0f // 3
		};

		unsigned int indicies[] = { 0, 1, 2, 2, 3, 0 };

		m_FBVA = std::make_unique<VertexArray>(
			positions, 4 * 4 * (uint32_t)sizeof(float), layout,
			indicies, 6);
	}

	// User Cursor Indicatior Setup
	{
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);

		float positions[] = {
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // 0
			1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // 1
			1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // 2
			-1.0f, 1.0f,  0.0f, 0.0f, 1.0f // 3
		};

		unsigned int indicies[] = { 0, 1, 2, 2, 3, 0 };

		m_IndicatorTexture =
			std::make_unique<Texture>("res/textures/indicator.png");
		m_IndicatorVA = std::make_unique<VertexArray>(
			positions, 4 * 5 * (uint32_t)sizeof(float), layout,
			indicies, 6);
	}

	// World Loading
	m_Window->SetTitle("VBuilder - Loading Map");

	{ // Loading Asset Info
		ItemInfoManager::SetItemsLocked(false);
		ItemInfoManager::ClearAllItems();
		{
			ItemInfo blockInfo;
			blockInfo.Name = "Air";
			blockInfo.Type = ItemType::Air;
			blockInfo.IsTransparent = true;
			blockInfo.ShowInInventory = false;
			ItemInfoManager::AddItem(blockInfo);
		}
		{
			ItemInfo blockInfo;
			blockInfo.Name = "Dirt";
			blockInfo.Type = ItemType::BlockCube;
			auto coords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 1, 9 });
			blockInfo.TopTexCoords = coords;
			blockInfo.SideTexCoords = coords;
			blockInfo.BottomTexCoords = coords;
			ItemInfoManager::AddItem(blockInfo);
		}
		{
			ItemInfo blockInfo;
			blockInfo.Name = "Grass";
			blockInfo.Type = ItemType::BlockCube;
			blockInfo.TopTexCoords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 3, 9 });
			blockInfo.SideTexCoords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 2, 9 });
			blockInfo.BottomTexCoords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 1, 9 });
			ItemInfoManager::AddItem(blockInfo);
		}
		{
			ItemInfo blockInfo;
			blockInfo.Name = "Stone";
			blockInfo.Type = ItemType::BlockCube;
			auto coords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 4, 9 });
			blockInfo.TopTexCoords = coords;
			blockInfo.SideTexCoords = coords;
			blockInfo.BottomTexCoords = coords;
			ItemInfoManager::AddItem(blockInfo);
		}
		{
			ItemInfo blockInfo;
			blockInfo.Name = "Cobblestone";
			blockInfo.Type = ItemType::BlockCube;
			auto coords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 5, 9 });
			blockInfo.TopTexCoords = coords;
			blockInfo.SideTexCoords = coords;
			blockInfo.BottomTexCoords = coords;
			ItemInfoManager::AddItem(blockInfo);
		}
		{
			ItemInfo blockInfo;
			blockInfo.Name = "Log";
			blockInfo.Type = ItemType::BlockCube;
			blockInfo.TopTexCoords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 6, 9 });
			blockInfo.SideTexCoords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 7, 9 });
			blockInfo.BottomTexCoords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 6, 9 });
			ItemInfoManager::AddItem(blockInfo);
		}
		{
			ItemInfo blockInfo;
			blockInfo.Name = "Leaves";
			blockInfo.Type = ItemType::BlockCube;
			blockInfo.IsTransparent = true;
			auto coords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 8, 9 });
			blockInfo.TopTexCoords = coords;
			blockInfo.SideTexCoords = coords;
			blockInfo.BottomTexCoords = coords;
			ItemInfoManager::AddItem(blockInfo);
		}
		{
			ItemInfo blockInfo;
			blockInfo.Name = "Bedrock";
			blockInfo.Type = ItemType::BlockCube;
			auto coords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 9, 9 });
			blockInfo.TopTexCoords = coords;
			blockInfo.SideTexCoords = coords;
			blockInfo.BottomTexCoords = coords;
			ItemInfoManager::AddItem(blockInfo);
		}
		{
			ItemInfo blockInfo;
			blockInfo.Name = "Glass";
			blockInfo.Type = ItemType::BlockCube;
			blockInfo.IsTransparent = true;
			auto coords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 0, 8 });
			blockInfo.TopTexCoords = coords;
			blockInfo.SideTexCoords = coords;
			blockInfo.BottomTexCoords = coords;
			ItemInfoManager::AddItem(blockInfo);
		}

		{
			ItemInfo blockInfo;
			blockInfo.Name = "TopGrass";
			blockInfo.Type = ItemType::BlockCross;
			blockInfo.IsTransparent = true;
			blockInfo.SideTexCoords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 9, 0 });
			ItemInfoManager::AddItem(blockInfo);
		}
		{
			ItemInfo blockInfo;
			blockInfo.Name = "Rose";
			blockInfo.Type = ItemType::BlockCross;
			blockInfo.IsTransparent = true;
			blockInfo.SideTexCoords = Texture::ExtractCoordinates(
				glm::vec2(10, 10), { 8, 0 });
			ItemInfoManager::AddItem(blockInfo);
		}

		ItemInfoManager::SetItemsLocked(true);
	}

	m_Map = std::make_unique<Map>();
	//m_Map = std::make_unique<Map>();
	m_Window->SetTitle("VBuilder - " +
			   std::string((const char *)glGetString(GL_RENDERER)));

	m_Player = std::make_unique<Player>();
	m_Player->SetUsername("Ethan");
	// TODO: Get Player position from server
	//m_Player->SetPosition(m_Map->GetSaveFile().GetPlayerPosition());
	//m_Player->GetCamera().SetYaw(m_Map->GetSaveFile().GetPlayerRotation().x);
	//m_Player->GetCamera().SetPitch(m_Map->GetSaveFile().GetPlayerRotation().y);

	m_HasInitialized = true;
	m_Running = true;

	float ms = initTimer.GetElapsedTime().AsMilliseconds();
	VB_TRACE("Init took {0} ms", ms);

	return 1;
}

void Game::OnUpdate()
{
	if (m_Minimized)
		return;

	m_Window->PollEvents();

	if (m_PlayerInitialized &&
	    m_PlayerServerUpdateTimer.GetElapsedTime().AsSeconds() >= 1.0f) {
		std::vector<uint8_t> packetData =
			AssemblePlayerInfoData(*m_Player);
		ENetPacket *packet =
			enet_packet_create(&packetData[0], packetData.size(),
					   ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(m_Server, 0, packet);

		m_PlayerServerUpdateTimer.Restart();
	}

	ENetEvent event;
	while (enet_host_service(m_Client, &event, 0) > 0) {
		if (event.type == ENET_EVENT_TYPE_CONNECT) {
			VB_INFO("Connected to server @ {0}:{1}.",
				event.peer->address.host,
				event.peer->address.port);
		} else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
			switch (event.packet->data[0]) {
			case PacketType::PlayerInfoData: {
				std::string username = "";
				int i = 1;
				while (event.packet->data[i] != '\0') {
					username += event.packet->data[i];
					i++;
				}
				i++;

				glm::vec3 pos = BytesToNum<glm::vec3>(
					event.packet->data + i);
				i += 12;
				glm::vec2 rot = BytesToNum<glm::vec2>(
					event.packet->data + i);

				m_Player->SetPosition(pos);
				m_Player->GetCamera().SetPitch(rot.x);
				m_Player->GetCamera().SetYaw(rot.y);

				m_PlayerInitialized = true;

				break;
			}
			case PacketType::ChunkContents: {
				m_Map->IntegrateColumnFromServer(
					event.packet->data,
					event.packet->dataLength);
				break;
			}
			default: {
				VB_WARN("Received unknown packet of type {}",
					event.packet->data[0]);
			}
			}

			enet_packet_destroy(event.packet);
		} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
			VB_INFO("Server disconnected.");
			m_Running = false;
			return;
		}
	}

	for (PlayerActionRequest &request : m_PlayerActionRequests) {
		std::vector<uint8_t> packetData =
			AssemblePlayerActionRequest(request);
		ENetPacket *packet =
			enet_packet_create(&packetData[0], packetData.size(),
					   ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(m_Server, 0, packet);
	}
	m_PlayerActionRequests.clear();

	m_FrameCount++;
	if (m_FPSTimer.GetElapsedTime().AsSeconds() > 0.25f) {
		m_FPS = m_FrameCount * 4;
		m_FPSTimer.Restart();
		m_FrameCount = 0;
	}

	if (!m_PlayerInitialized) {
		std::this_thread::sleep_for(10ms);
		return;
	}

	m_Player->Update(m_DeltaTime);

	auto &cameraPos = m_Player->GetCamera().GetPosition();
	m_Map->Update(m_Server, cameraPos, 16 * 16.0f);

	m_Projection = glm::perspective(glm::radians(45.0f),
					m_Resolution.x / m_Resolution.y, 0.1f,
					1000.0f);
	m_View = m_Player->GetCamera().GetView();

	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);

	WorldRenderer::ResetStats();

	m_FrameBuffer->Bind();

	m_Window->Clear({ 0.0f, 0.89f, 1.0f, 1.0f });

	WorldRenderer::SetTexture(m_MapTextureAtlas.get());
	WorldRenderer::BeginScene(m_View, m_Projection);

	m_Map->Render();

	WorldRenderer::EndScene();

	m_FrameBuffer->Unbind();

	m_Window->Clear({ 1.0f, 1.0f, 1.0f, 1.0f });

	//m_TestTexture->Bind(1);
	m_FBShader->Bind();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D,
		      m_FrameBuffer->GetColorAttachmentRendererID());
	m_FBVA->Bind();
	glDrawElements(GL_TRIANGLES, m_FBVA->GetIndexCount(), GL_UNSIGNED_INT,
		       nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	m_TextureShader->Bind();
	m_TextureShader->SetUniform1i("u_Texture", 2);
	glm::mat4 transform =
		glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 1.0f));
	m_TextureShader->SetUniformMat4f("u_Transform", transform);
	float aspect = m_Window->GetAspectRatio();
	glm::mat4 proj = glm::ortho(-aspect, aspect, -1.0f, 1.0f);
	m_TextureShader->SetUniformMat4f("u_ViewProjection", proj);
	m_IndicatorTexture->BindTexture(2);
	m_IndicatorVA->Bind();
	glDrawElements(GL_TRIANGLES, m_IndicatorVA->GetIndexCount(),
		       GL_UNSIGNED_INT, nullptr);

	if (m_ShowStats) {
		glm::mat4 textOrtho =
			glm::ortho(0.0f, m_Resolution.x, 0.0f, m_Resolution.y);
		TextRenderer::BeginScene(glm::mat4(1.0f), textOrtho);
		TextRenderer::RenderText(
			"FrameTime: " +
				std::to_string(
					(uint32_t)(m_DeltaTime * 1000.0f)) +
				" ms",
			(float)m_DebugTextSize,
			m_Resolution.y - (float)m_DebugTextSize);
		TextRenderer::RenderText(
			"FPS: " + std::to_string(m_FPS), (float)m_DebugTextSize,
			m_Resolution.y - 2.0f * (float)m_DebugTextSize);
		TextRenderer::RenderText(
			"Position: x=" +
				std::to_string((int)m_Player->GetCamera()
						       .GetPosition()
						       .x) +
				", y=" +
				std::to_string((int)m_Player->GetCamera()
						       .GetPosition()
						       .y) +
				", z=" +
				std::to_string((int)m_Player->GetCamera()
						       .GetPosition()
						       .z),
			(float)m_DebugTextSize,
			m_Resolution.y - 3.0f * (float)m_DebugTextSize);
		{
			std::stringstream ss;
			const auto &stats = WorldRenderer::GetStats();
			ss << "Draw Calls: " << stats.DrawCalls;
			ss << "\nQuads: " << stats.QuadCount;
			ss << "\nVertices: " << stats.GetTotalVertexCount();
			ss << "\nIndicies: " << stats.GetTotalIndexCount();
			TextRenderer::RenderText(
				ss.str(), (float)m_DebugTextSize,
				m_Resolution.y - 4.0f * (float)m_DebugTextSize);
		}
		{
			std::stringstream ss;
			ss << "Memory: "
			   << m_Map->GetMemoryUsage() / 1024 / 1024 << " MB";
			TextRenderer::RenderText(
				ss.str(), (float)m_Resolution.x / 2.0f,
				m_Resolution.y - (float)m_DebugTextSize);
		}
		{
			std::stringstream ss;
			ss << "Selection: "
			   << ItemInfoManager::GetItemByID(
				      m_Player->GetInventorySelection())
					->Name
			   << " (" << m_Player->GetInventorySelection() << ')';
			TextRenderer::RenderText(
				ss.str(), (float)m_Resolution.x / 2.0f,
				m_Resolution.y - 2.0f * (float)m_DebugTextSize);
		}
	}

	m_Window->SwapBuffers();
}

void Game::OnTerminate()
{
	m_Window->SetTitle("VBuilder - Shutting Down");
	//m_Map->GetSaveFile().SetPlayerPosition(m_Player->GetCamera().GetPosition());
	//m_Map->GetSaveFile().SetPlayerRotation(glm::vec2(m_Player->GetCamera().GetPitch(), m_Player->GetCamera().GetYaw()));

	WorldRenderer::Shutdown();

	TextRenderer::Shutdown();

	enet_peer_disconnect(m_Server, 0);

	enet_host_flush(m_Client);

	std::this_thread::sleep_for(20ms);

	enet_peer_reset(m_Server);

	enet_host_destroy(m_Client);

	delete m_Window;
}

void Game::OnEvent(Event &e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(
		VB_BIND_EVENT_FN(Game::OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(
		VB_BIND_EVENT_FN(Game::OnWindowResize));
	dispatcher.Dispatch<MouseButtonPressedEvent>(
		VB_BIND_EVENT_FN(Game::OnMouseButtonPressedEvent));
	dispatcher.Dispatch<KeyReleasedEvent>(
		VB_BIND_EVENT_FN(Game::OnKeyReleasedEvent));
	dispatcher.Dispatch<WindowContentScaleChangedEvent>(
		VB_BIND_EVENT_FN(Game::OnWindowContentScaleChangedEvent));
}

bool Game::OnWindowClose(WindowCloseEvent &e)
{
	m_Running = false;
	return true;
}

bool Game::OnWindowResize(WindowResizeEvent &e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0) {
		m_Minimized = true;
		return false;
	}

	m_Minimized = false;
	glViewport(0, 0, e.GetWidth(), e.GetHeight());
	m_Resolution = { e.GetWidth(), e.GetHeight() };

	if (m_HasInitialized) {
		m_FrameBuffer->Resize(e.GetWidth(), e.GetHeight());
	}

	return false;
}

glm::vec3 PlayerToBlockCoords(glm::vec3 position)
{
	if (position.x < 0)
		position.x -= 1.0f;
	if (position.y < 0)
		position.y -= 1.0f;
	if (position.z < 0)
		position.z -= 1.0f;

	return glm::vec3((int)position.x, (int)position.y, (int)position.z);
}

bool Game::OnMouseButtonPressedEvent(MouseButtonPressedEvent &e)
{
	if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_1 ||
	    e.GetMouseButton() == GLFW_MOUSE_BUTTON_2) {
		auto &cameraPosition = m_Player->GetCamera().GetPosition();
		auto &cameraFront = m_Player->GetCamera().GetFront();

		RayCast ray(cameraFront, 0.05f, 6.0f);

		while (ray.Step()) {
			glm::vec3 blockPosition = PlayerToBlockCoords(
				cameraPosition + ray.GetPositionOffset());

			int block = m_Map->GetBlock(blockPosition);
			if (block != -1 && block != 0) {
				if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_1) {
					m_PlayerActionRequests.push_back(
						PlayerActionRequest{
							ChunkUpdateType::
								DestroyBlockEvent,
							blockPosition });
					// m_Map->SetBlock(blockPosition, ItemInfoManager::GetIDByName("Air"), true);
					// VB_TRACE("Block Removed at ({0},{1},{2})", blockPosition.x, blockPosition.y, blockPosition.z);
					break;
				} else if (e.GetMouseButton() ==
					   GLFW_MOUSE_BUTTON_2) {
					while (ray.StepBack()) {
						glm::vec3 placePosition = PlayerToBlockCoords(
							cameraPosition +
							ray.GetPositionOffset());

						int placeBlock =
							m_Map->GetBlock(
								placePosition);
						if (placeBlock == 0) {
							m_PlayerActionRequests.push_back(PlayerActionRequest{
								ChunkUpdateType::
									PlaceBlockEvent,
								placePosition,
								(uint32_t)m_Player
									->GetInventorySelection() });
							// m_Map->SetBlock(placePosition, m_Player->GetInventorySelection(), true);
							// VB_TRACE("Block Placed at ({0},{1},{2})", blockPosition.x, blockPosition.y, blockPosition.z);
							break;
						}
					}
					break;
				}
			}
		}
	}

	return false;
}

bool Game::OnKeyReleasedEvent(KeyReleasedEvent &e)
{
	if (e.GetKeyCode() == GLFW_KEY_F3)
		m_ShowStats = !m_ShowStats;

	if (e.GetKeyCode() >= GLFW_KEY_1 && e.GetKeyCode() <= GLFW_KEY_9) {
		int code = e.GetKeyCode() - GLFW_KEY_1 + 1;
		if (e.GetKeyCode() == GLFW_KEY_0)
			code = 10;
		m_Player->SetInventorySelection(code);
	}

	if (e.GetKeyCode() == GLFW_KEY_ESCAPE) {
		m_Running = false;
		return true;
	}
	return false;
}

bool Game::OnWindowContentScaleChangedEvent(WindowContentScaleChangedEvent &e)
{
	m_DebugTextSize = (int)(25.0f * e.GetXScale());
	TextRenderer::SetTextSize(m_DebugTextSize);
	return false;
}
