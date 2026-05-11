#pragma once

#include <Scene/Scene.hpp>

namespace oxrena {
class Game {
public:
  constexpr static auto MODULE_NAME = "Oxrena";

  auto init() -> std::expected<void, std::string>;
  auto deinit() -> std::expected<void, std::string>;
  auto update(const ox::Timestep& timestep) -> void;

  std::unique_ptr<ox::Scene> main_scene = nullptr;
};
} // namespace rog
