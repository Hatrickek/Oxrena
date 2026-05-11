#pragma once
#include <Core/Systems/System.h>

namespace Oxrena {
struct WeaponComponent {
  int32_t Damage = 0;
};

struct ShotgunComponent : WeaponComponent { };

class WeaponSystem : public Oxylus::System {
public:
  void on_update(Oxylus::Scene* scene, const Oxylus::Timestep&  deltaTime) override;
  void on_imgui_render(Oxylus::Scene* scene, const Oxylus::Timestep&  deltaTime) override;
};
}
