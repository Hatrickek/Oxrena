#pragma once
#include <Core/Base.h>

namespace Oxylus {
  class TextureAsset;
  class RuntimeConsole;
  class Scene;
}

namespace Oxrena {
  class UI {
  public:
    static void init();

    static void draw_crosshair();

    static void draw_runtime_console(Oxylus::RuntimeConsole& console, bool* block_player_input);
    static void draw_fps_counter();

  private:
    static struct UIResources {
      Oxylus::Ref<Oxylus::TextureAsset> crosshair_a;
    } s_ui_resources;
  };
}
