#include <OxylusEngine.h>
#include <Assets/AssetManager.h>
#include <Core/EntryPoint.h>
#include <Core/Systems/HotReloadableScenes.h>

#include "RuntimeLayer.h"

namespace Oxylus {
  class OxylusRuntime : public Application {
  public:
    OxylusRuntime(const AppSpec& spec) : Application(spec) { }
  };

  Application* CreateApplication(const ApplicationCommandLineArgs args) {
    AppSpec spec;
    spec.Name = "OxArena";
    spec.WorkingDirectory = std::filesystem::current_path().string();
    spec.CommandLineArgs = args;
    spec.CustomWindowTitle = false;
    spec.UseImGui = true;
    spec.ResourcesPath = "Assets";

    const auto app = new OxylusRuntime(spec);
    app->PushLayer(new ::OxArena::RuntimeLayer());
    app->GetSystemManager()
        .AddSystem<HotReloadableScenes>(::OxArena::RuntimeLayer::GetAssetsPath("Scenes/Main.oxscene"));

    return app;
  }
}
