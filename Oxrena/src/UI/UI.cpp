#include "UI.h"

#include <UI/OxUI.h>
#include <Render/Window.h>
#include <Scene/Scene.h>

#include <UI/RuntimeConsole.h>
#include <Utils/ImGuiScoped.h>


namespace Oxrena {
using namespace Oxylus;

UI::UIResources UI::s_ui_resources;

void UI::init() {
  s_ui_resources.crosshair_a = create_ref<TextureAsset>("Assets/Textures/crosshaira.tga");
}

void UI::draw_crosshair() {
  constexpr auto flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                         ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                         ImGuiWindowFlags_NoBackground;
  ImGuiScoped::StyleVar style(ImGuiStyleVar_WindowPadding, ImVec2{});
  OxUI::center_next_window();
  if (ImGui::Begin("Crosshair", nullptr, flags)) {
    OxUI::image(s_ui_resources.crosshair_a->get_texture(), ImVec2{32.0f, 32.0f});
    ImGui::End();
  }
}

void UI::draw_fps_counter() {
  constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
                                            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                                            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                                            ImGuiWindowFlags_NoMove;
  constexpr float PAD = 10.0f;
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  const ImVec2 work_pos = viewport->WorkPos;
  ImVec2 window_pos;
  window_pos.x = (work_pos.x + PAD);
  window_pos.y = work_pos.y + PAD;
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.35f);
  if (ImGui::Begin("Performance Overlay", nullptr, window_flags)) {
    ImGui::Text("%.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::End();
  }
}

void UI::draw_runtime_console(RuntimeConsole& console, bool* block_player_input) {
  if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent, false)) {
    console.visible = !console.visible;
  }
  static float s_AnimationCounter = 0;
  if (console.visible) {
    if (s_AnimationCounter >= 0.4f)
      s_AnimationCounter = 0.4f;
    else
      s_AnimationCounter += Application::get_timestep() * 3.0f;
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->WorkPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize({ImGui::GetMainViewport()->WorkSize.x, ImGui::GetMainViewport()->WorkSize.y * s_AnimationCounter},
                             ImGuiCond_Always);
    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration |
                                             ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
    ImGuiScoped::StyleVar round(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGuiScoped::StyleColor col1(ImGuiCol_FrameBg, ImVec4(0.000f, 0.000f, 0.000f, 1.000f));
    ImGuiScoped::StyleColor col2(ImGuiCol_WindowBg, ImVec4(0.000f, 0.000f, 0.000f, 0.784f));
    ImGuiScoped::StyleColor col3(ImGuiCol_MenuBarBg, ImVec4(0.100f, 0.100f, 0.100f, 1.000f));
    console.on_imgui_render(windowFlags);
    *block_player_input = true;
  }
  else {
    s_AnimationCounter = 0.0f;
    *block_player_input = false;
  }
}
}
