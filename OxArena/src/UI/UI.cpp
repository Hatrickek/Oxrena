#include "UI.h"

#include <imgui.h>
#include <Render/Window.h>
#include <Scene/Scene.h>

#include <UI/RuntimeConsole.h>
#include <Utils/ImGuiScoped.h>

namespace OxArena {
  using namespace Oxylus;

  UI::UIResources UI::s_UIResources;

  void UI::Init() {
    VulkanImageDescription desc;
    desc.Path = "Assets/Textures/crosshaira.tga";
    desc.CreateDescriptorSet = true;
    s_UIResources.CrosshairA = CreateRef<VulkanImage>(desc);
  }

  void UI::DrawCrosshair() {
    constexpr auto flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | 
                           ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                           ImGuiWindowFlags_NoBackground;
    ImGuiScoped::StyleVar style(ImGuiStyleVar_WindowPadding, ImVec2{});
    const auto center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::Begin("Crosshair", nullptr, flags)) {
      ImGui::Image(s_UIResources.CrosshairA->GetDescriptorSet(), ImVec2{32.0f, 32.0f});
      ImGui::End();
    }
  }

  void UI::DrawFinalImage(const Ref<Scene>& scene) {
    constexpr auto flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
                           | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGuiScoped::StyleVar style(ImGuiStyleVar_WindowPadding, ImVec2{});
    if (ImGui::Begin("FinalImage", nullptr, flags)) {
      ImGui::Image(scene->GetRenderer().GetRenderPipeline()->GetFinalImage().GetDescriptorSet(),
        ImVec2{(float)Window::GetWidth(), (float)Window::GetHeight()});
      ImGui::End();
    }
  }

  void UI::DrawFPSCounter() {
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

  void UI::DrawRuntimeConsole(RuntimeConsole& console, bool* blockPlayerInput) {
    if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent, false)) {
      console.Visible = !console.Visible;
    }
    static float s_AnimationCounter = 0;
    if (console.Visible) {
      if (s_AnimationCounter >= 0.4f)
        s_AnimationCounter = 0.4f;
      else
        s_AnimationCounter += Application::GetTimestep() * 3.0f;
      ImGui::SetNextWindowPos(ImGui::GetMainViewport()->WorkPos, ImGuiCond_Always);
      ImGui::SetNextWindowSize({
          ImGui::GetMainViewport()->WorkSize.x,
          ImGui::GetMainViewport()->WorkSize.y * s_AnimationCounter
        },
        ImGuiCond_Always);
      constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration |
                                               ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
      ImGuiScoped::StyleColor col1(ImGuiCol_FrameBg, ImVec4(0.000f, 0.000f, 0.000f, 1.000f));
      ImGuiScoped::StyleColor col2(ImGuiCol_WindowBg, ImVec4(0.000f, 0.000f, 0.000f, 0.784f));
      ImGuiScoped::StyleColor col3(ImGuiCol_MenuBarBg, ImVec4(0.100f, 0.100f, 0.100f, 1.000f));
      console.OnImGuiRender(windowFlags);
      *blockPlayerInput = true;
    }
    else {
      s_AnimationCounter = 0.0f;
      *blockPlayerInput = false;
    }
  }
}
