#include "OxrenaLayer.h"

#include <imgui.h>
#include <Scene/SceneSerializer.h>

#include <Render/RendererConfig.h>

#include "Core/Application.h"

#include "Render/RenderPipeline.h"

#include "Scene/SceneRenderer.h"

#include "Systems/CharacterSystem.h"
#include "Systems/FreeCamera.h"
#include "Systems/PickupSystem.h"
#include "Systems/WeaponSystem.h"

#include "UI/UI.h"

namespace Oxrena {
using namespace Oxylus;
OxrenaLayer* OxrenaLayer::s_instance = nullptr;

OxrenaLayer::OxrenaLayer() : Layer("Game Layer") {
  s_instance = this;
}

void OxrenaLayer::on_attach(EventDispatcher& dispatcher) {
  UI::init();

  auto& style = ImGui::GetStyle();
  style.WindowMenuButtonPosition = ImGuiDir_Left;

  runtime_console.render_menu_bar = false;
  runtime_console.visible = false;
  runtime_console.set_focus_to_keyboard_always = true;
  runtime_console.register_command("free_camera", fmt::format("free_camera {0}", free_camera), &free_camera);

  dispatcher.sink<ReloadSceneEvent>().connect<&OxrenaLayer::on_scene_reload>(*this);
  load_scene();
}

void OxrenaLayer::on_detach() { }

void OxrenaLayer::on_update(const Timestep& delta_time) {
  m_scene->on_runtime_update(delta_time);
}

void OxrenaLayer::on_imgui_render() {
  m_scene->on_imgui_render(Application::get_timestep());

  UI::draw_runtime_console(runtime_console, &blocking_player_input);
  UI::draw_fps_counter();
  UI::draw_crosshair();
}

void OxrenaLayer::load_scene() {
  m_scene = create_ref<Scene>();
  const SceneSerializer serializer(m_scene);
  serializer.deserialize(get_assets_path("Scenes/Main.oxscene"));

  // Scene components
  auto& registery = m_scene->m_registry;
  for (const auto ch_view = registery.view<CharacterControllerComponent>(); const auto entity : ch_view) {
    Entity{entity, m_scene.get()}.add_component<CharacterComponent>();
  }

  m_scene->on_runtime_start();

  m_scene->add_system<FreeCamera>(&free_camera)
         ->add_system<CharacterSystem>()
         ->add_system<PickupSystem>();
}

bool OxrenaLayer::on_scene_reload(ReloadSceneEvent&) {
  load_scene();
  OX_CORE_INFO("Scene reloaded.");
  return true;
}
}
