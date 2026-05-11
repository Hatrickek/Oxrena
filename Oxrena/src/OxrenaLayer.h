#pragma once
#include <Core/Layer.h>

#include "Core/Entity.h"
#include "Core/Systems/HotReloadableScenes.h"

#include "Scene/Scene.h"
#include "UI/RuntimeConsole.h"

namespace Oxrena {
  class OxrenaLayer : public Oxylus::Layer {
  public:
    bool blocking_player_input = false;
    bool free_camera = false;

    OxrenaLayer();
    ~OxrenaLayer() override = default;
    void on_attach(Oxylus::EventDispatcher& dispatcher) override;
    void on_detach() override;
    void on_update(const Oxylus::Timestep& delta_time) override;
    void on_imgui_render() override;

    static OxrenaLayer* get() { return s_instance; }

    static std::string get_assets_path(const std::string_view path) {
      return ("Assets" / std::filesystem::path(path)).string();
    }

  private:
    void load_scene();
    bool on_scene_reload(Oxylus::ReloadSceneEvent& e);

  private:
    static OxrenaLayer* s_instance;

    Oxylus::EventDispatcher m_dispatcher;

    // Scene
    Oxylus::Ref<Oxylus::Scene> m_scene;

    //UI
    Oxylus::RuntimeConsole runtime_console;
  };
}
