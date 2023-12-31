#include "RuntimeLayer.h"

#include <imgui.h>
#include <Scene/SceneSerializer.h>

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
    m_RuntimeConsole.Visible = false;
    m_RuntimeConsole.SetFocusToKeyboardAlways = true;
    m_RuntimeConsole.RegisterCommand("ssao",
      fmt::format("ssao {}", RendererConfig::Get()->SSAOConfig.Enabled),
      [] {
        RendererConfig::Get()->SSAOConfig.Enabled = !RendererConfig::Get()->SSAOConfig.Enabled;
        RendererConfig::Get()->ConfigChangeDispatcher.trigger<RendererConfig::ConfigChangeEvent>();
      });
    m_RuntimeConsole.RegisterCommand("free_camera", fmt::format("free_camera {0}", m_FreeCamera), &m_FreeCamera);

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

    // Scene components
    auto& registery = m_Scene->m_Registry;
    const auto chView = registery.view<CharacterControllerComponent>();
    for (const auto entity : chView) {
      Entity{entity, m_Scene.get()}.AddComponent<CharacterComponent>();
    }

    // Custom components
    const auto customView = registery.view<CustomComponent>();
    for (const auto entity : customView) {
      auto& component = customView.get<CustomComponent>(entity);
      if (component.Name == "HealthPickup") {
        auto& c = Entity{entity, m_Scene.get()}.AddComponent<HealthPickupComponent>();
        c.Value = std::stoi(component.Fields[0].Value);
      }
      else if (component.Name == "ArmorPickup") {
        auto& c = Entity{entity, m_Scene.get()}.AddComponent<ArmorPickupComponent>();
        c.Value = std::stoi(component.Fields[0].Value);
      }
      else if (component.Name == "ShotgunComponent") {
        auto& c = Entity{entity, m_Scene.get()}.AddComponent<ShotgunComponent>();
        c.Damage = std::stoi(component.Fields[0].Value);
      }
      else if (component.Name == "WeaponPickup") {
        auto& c = Entity{entity, m_Scene.get()}.AddComponent<WeaponPickupComponent>();
        if (component.Fields[0].Value == "Shotgun")
          c.Type = WeaponType::SHOTGUN;
      }
    }

    m_Scene->OnRuntimeStart();

    m_Scene->AddSystem<FreeCamera>(&m_FreeCamera)
           ->AddSystem<CharacterSystem>()
           ->AddSystem<PickupSystem>();
  }

  bool RuntimeLayer::OnSceneReload(ReloadSceneEvent&) {
    LoadScene();
    OX_CORE_INFO("Scene reloaded.");
    return true;
  }
}
