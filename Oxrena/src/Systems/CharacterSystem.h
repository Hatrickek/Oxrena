#pragma once
#include "Core/Components.h"
#include "Core/Types.h"
#include "Core/Systems/System.h"

namespace Oxrena {
struct CharacterComponent {
  uint32_t Health = 50;
  uint32_t Armor = 0;
};

class CharacterSystem : public Oxylus::System {
public:
  ~CharacterSystem() override;
  void on_init() override;
  void on_update(Oxylus::Scene* scene, const Oxylus::Timestep& delta_time) override;
  void on_fixed_update(Oxylus::Scene* scene, float delta_time) override;
  void on_imgui_render(Oxylus::Scene* scene, const Oxylus::Timestep& delta_time) override;
  void on_contact_added(Oxylus::Scene* scene,
                        const JPH::Body& body1,
                        const JPH::Body& body2,
                        const JPH::ContactManifold& manifold,
                        const JPH::ContactSettings& settings) override;

private:
  struct MovementArgs {
    JPH::Vec3 wish_dir = {};
    JPH::Vec3 movement_direction = {};
    Oxylus::CharacterControllerComponent character_controller;
    bool jump_queued = {};
    bool is_grounded = {};
  };

  void queue_jump(bool auto_bunny_hop);
  void ground_move(JPH::Vec3& velocity, const MovementArgs& args, float delta_time);
  static void air_control(JPH::Vec3& velocity,
                          JPH::Vec3 target_dir,
                          JPH::Vec3 mov_input,
                          float target_speed,
                          float air_control);
  static void air_move(JPH::Vec3& velocity, const MovementArgs& args, float delta_time);
  static void apply_friction(JPH::Vec3& velocity,
                             float ground_deceleration,
                             bool is_grounded,
                             float friction,
                             float delta_time,
                             float t);
  static void accelerate(JPH::Vec3& velocity, const JPH::Vec3& target_dir, float target_speed, float accel);
  bool m_jump_queued;
};
}
