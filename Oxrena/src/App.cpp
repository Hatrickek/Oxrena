#include <Core/App.hpp>
#include <Core/DefaultModules.hpp>

#include "Game.hpp"

int main(int argc, char** argv) {
  auto app = ox::App(argc, argv);
  app.with_name("Oxrena")
    .with_window({
      .title = "Oxrena",
      .icon = {},
      .width = 1720,
      .height = 900,
      .flags = ox::WindowFlag::Centered | ox::WindowFlag::Resizable | ox::WindowFlag::HighPixelDensity,
    })
    .with_assets_directory("Assets")
    .with(ox::DefaultModules{})
    .with<oxrena::Game>()
    .run();

  return 0;
}
