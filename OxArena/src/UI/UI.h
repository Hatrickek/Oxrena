#pragma once
#include <Core/Base.h>

namespace Oxylus {
  class VulkanImage;
  class RuntimeConsole;
  class Scene;
}

namespace OxArena {
  class UI {
  public:
    static void Init();

    static void DrawCrosshair();

    static void DrawFinalImage(const Oxylus::Ref<Oxylus::Scene>& scene);
    static void DrawRuntimeConsole(Oxylus::RuntimeConsole& console, bool* blockPlayerInput);
    static void DrawFPSCounter();

  private:
    static struct UIResources {
      Oxylus::Ref<Oxylus::VulkanImage> CrosshairA;
    } s_UIResources;
  };
}
