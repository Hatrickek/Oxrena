#include "RuntimeLayer.h"

#include <imgui.h>
#include <Scene/SceneSerializer.h>
#include <entt.hpp>

#include <Render/RendererConfig.h>

#include "Systems/CharacterSystem.h"
#include "Systems/FreeCamera.h"
#include "Systems/PickupSystem.h"
#include "Systems/WeaponSystem.h"

#include "UI/UI.h"

namespace OxArena {
  using namespace Oxylus;
  RuntimeLayer* RuntimeLayer::s_Instance = nullptr;

  RuntimeLayer::RuntimeLayer() : Layer("Game Layer") {
    s_Instance = this;
  }

  RuntimeLayer::~RuntimeLayer() = default;

  void RuntimeLayer::OnAttach(EventDispatcher& dispatcher) {
    UI::Init();

    auto& style = ImGui::GetStyle();
    style.WindowMenuButtonPosition = ImGuiDir_Left;

    m_RuntimeConsole.RenderMenuBar = false;

    dispatcher.sink<ReloadSceneEvent>().connect<&RuntimeLayer::OnSceneReload>(*this);
    LoadScene();
  }

  void RuntimeLayer::OnDetach() { }

  void RuntimeLayer::OnUpdate(Timestep deltaTime) {
    m_Scene->OnRuntimeUpdate(deltaTime);
  }

  void RuntimeLayer::OnImGuiRender() {
    UI::DrawFinalImage(m_Scene);

    m_Scene->OnImGuiRender(Application::GetTimestep());

    UI::DrawRuntimeConsole(m_RuntimeConsole, &m_BlockingPlayerInput);

    UI::DrawFPSCounter();

    UI::DrawCrosshair();
  }

  void RuntimeLayer::LoadScene() {
    m_Scene = CreateRef<Scene>();
    const SceneSerializer serializer(m_Scene);
    serializer.Deserialize(GetAssetsPath("Scenes/Main.oxscene"));

    m_Scene->OnRuntimeStart();

    m_Scene->AddSystem<CharacterSystem>();
  }

  bool RuntimeLayer::OnSceneReload(ReloadSceneEvent&) {
    LoadScene();
    OX_CORE_INFO("Scene reloaded.");
    return true;
  }
}
