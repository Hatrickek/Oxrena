#include "WeaponSystem.h"
#include <Core/Components.h>

#include <Scene/Scene.h>

namespace OxArena {
  using namespace Oxylus;

  void WeaponSystem::OnUpdate(Scene* scene, Timestep deltaTime) {
    //auto& registery = scene->m_Registry;
    //registery.view<TransformComponent, ShotgunComponent>()
    //         .each([](TransformComponent& transform, ShotgunComponent& component) {
    //         });
  }

  void WeaponSystem::OnImGuiRender(Scene* scene, Timestep deltaTime) {
    
  }
}
