#include <Core/EntryPoint.h>
#include <Core/Systems/HotReloadableScenes.h>

#include "OxrenaLayer.h"

#include "Core/Systems/SystemManager.h"

namespace Oxylus {
class OxrenaApp : public Application {
public:
  OxrenaApp(const AppSpec& spec) : Application(spec) { }
};

Application* create_application(const ApplicationCommandLineArgs args) {
  AppSpec spec;
  spec.name = "Oxrena";
  spec.working_directory = std::filesystem::current_path().string();
  spec.command_line_args = args;
  spec.resources_path = "Assets";
  spec.device_index = 0; // TODO: Make sure it's the discrete gpu if available.

  const auto app = new OxrenaApp(spec);
  app->push_layer(new Oxrena::OxrenaLayer());
  app->get_system_manager()
     ->add_system<HotReloadableScenes>(
       Oxrena::OxrenaLayer::get_assets_path("Scenes/Main.oxscene"));

  return app;
}
}
