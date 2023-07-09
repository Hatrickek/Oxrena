#pragma once
#include <Core/Systems/System.h>

namespace OxArena {

  struct PickupComponent {
    bool PickedUp = false;
    uint32_t Value = 0;
    float RespawnTimer = 0.0f;
    float MaxRespawnTime = 5.0f;
  };

  struct HealthPickupComponent : PickupComponent {
  };

  struct ArmorPickupComponent : PickupComponent { 
  };

  enum class WeaponType : uint32_t {
    MINIGUN = 0,
    SHOTGUN,
    ROCKET_LAUNCHER,
    RAYGUN,
  };

  struct WeaponPickupComponent {
    WeaponType Type = {};
  };

  struct AmmoPickupComponent {
    WeaponType Type = {};
    uint32_t Value = 10;
  };

  class PickupSystem : public Oxylus::System {
  public:
    void OnUpdate(Oxylus::Scene* scene, Oxylus::Timestep deltaTime) override;
    void OnImGuiRender(Oxylus::Scene* scene, Oxylus::Timestep deltaTime) override;
    void OnContactAdded(Oxylus::Scene* scene,
      const JPH::Body& body1,
      const JPH::Body& body2,
      const JPH::ContactManifold& manifold,
      const JPH::ContactSettings& settings) override;

  };
}
