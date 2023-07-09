#pragma once
#include <Core/Systems/System.h>

namespace OxArena {
  struct WeaponComponent {
    int32_t Damage = 0;
  };

  struct ShotgunComponent : WeaponComponent {
  };

  class WeaponSystem : public Oxylus::System {
  public:
    void OnUpdate(Oxylus::Scene* scene, Oxylus::Timestep deltaTime) override;
    void OnImGuiRender(Oxylus::Scene* scene, Oxylus::Timestep deltaTime) override;
  };
}
