#pragma once
#include <Core/Layer.h>

#include "Core/Entity.h"
#include "Core/Systems/HotReloadableScenes.h"

#include "Scene/Scene.h"
#include "UI/RuntimeConsole.h"

namespace OxArena {
  class RuntimeLayer : public Oxylus::Layer {
  public:
    bool m_BlockingPlayerInput = false;
    bool m_FreeCamera = false;

    RuntimeLayer();
    ~RuntimeLayer() override;
    void OnAttach(Oxylus::EventDispatcher& dispatcher) override;
    void OnDetach() override;
    void OnUpdate(Oxylus::Timestep deltaTime) override;
    void OnImGuiRender() override;

    static RuntimeLayer* Get() { return s_Instance; }

    static std::string GetAssetsPath(const std::string_view path) {
      return ("Assets" / std::filesystem::path(path)).string();
    }

  private:
    void LoadScene();
    bool OnSceneReload(Oxylus::ReloadSceneEvent& e);

  private:
    static RuntimeLayer* s_Instance;

    Oxylus::EventDispatcher m_Dispatcher;

    // Scene
    Oxylus::Ref<Oxylus::Scene> m_Scene;

    //UI
    Oxylus::RuntimeConsole m_RuntimeConsole;
  };
}
