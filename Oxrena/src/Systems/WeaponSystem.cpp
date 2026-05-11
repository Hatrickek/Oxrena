#include "WeaponSystem.h"
#include <Core/Components.h>

#include <Scene/Scene.h>

namespace Oxrena {
using namespace Oxylus;

void WeaponSystem::on_update(Scene* scene, const Oxylus::Timestep&  deltaTime) {
  //auto& registery = scene->m_Registry;
  //registery.view<TransformComponent, ShotgunComponent>()
  //         .each([](TransformComponent& transform, ShotgunComponent& component) {
  //         });
}

void WeaponSystem::on_imgui_render(Scene* scene, const Oxylus::Timestep&  deltaTime) { }
}
