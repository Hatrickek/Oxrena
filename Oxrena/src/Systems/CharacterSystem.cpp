#include "CharacterSystem.h"

#include "OxrenaLayer.h"
#include "Core/Components.h"
#include "Core/Input.h"

#include "Render/Window.h"

#include "Scene/Scene.h"
#include "UI/OxUI.h"

namespace Oxrena {
using namespace Oxylus;

// Temporary globals
static float s_max_horizontal_velocity = 0.0f;
static float s_max_vertical_velocity = 0.0f;

static bool s_lock_camera = false;
static Vec2 s_locked_mouse_position = {};
static float s_mouse_sensitivity = 1.0f;
static bool s_using_camera = false;

CharacterSystem::~CharacterSystem() = default;

void CharacterSystem::on_init() { }

void CharacterSystem::on_update(Scene* scene, const Timestep& delta_time) {}

void CharacterSystem::on_fixed_update(Scene* scene, float delta_time) {
  auto& registery = scene->m_registry;
  const auto character_view = registery.view<TransformComponent, CharacterControllerComponent>();
  const auto camera_view = registery.view<TransformComponent, CameraComponent>();

  if (OxrenaLayer::get()->blocking_player_input) {
    s_lock_camera = false;
    return;
  }

  for (const auto camera_entity : camera_view) {
    auto&& [cameraTransform, cameraComponent] = camera_view.get<TransformComponent, CameraComponent>(camera_entity);
    for (const auto entity : character_view) {
      auto&& [chTransform, chComponent] = character_view.get<TransformComponent, CharacterControllerComponent>(entity);

      chComponent.jump_force = 18;

      JPH::Vec3 wish_dir = {};
      JPH::Vec3 movement_dir = {};
      if (Input::get_key_held(KeyCode::W))
        movement_dir.SetX(1);
      if (Input::get_key_held(KeyCode::A))
        movement_dir.SetZ(-1);
      if (Input::get_key_held(KeyCode::S))
        movement_dir.SetX(-1);
      if (Input::get_key_held(KeyCode::D))
        movement_dir.SetZ(1);

      wish_dir = movement_dir;

      if (wish_dir != JPH::Vec3::sZero())
        wish_dir = wish_dir.Normalized();

      JPH::Vec3 cam_fwd = {cameraComponent.system->get_front().x, cameraComponent.system->get_front().y, cameraComponent.system->get_front().z};
      cam_fwd.SetY(0.0f);
      cam_fwd = cam_fwd.NormalizedOr(JPH::Vec3::sAxisX());
      JPH::Quat rotation = JPH::Quat::sFromTo(JPH::Vec3::sAxisX(), cam_fwd);
      wish_dir = rotation * wish_dir;

      queue_jump(chComponent.auto_bunny_hop);

      const bool is_grounded = chComponent.character->IsSupported();

      // Set movement state.
      MovementArgs args{
        .wish_dir = wish_dir,
        .movement_direction = movement_dir,
        .character_controller = chComponent,
        .jump_queued = m_jump_queued,
        .is_grounded = is_grounded
      };

      if (is_grounded) {
        ground_move(chComponent.current_velocity, args, delta_time);
      }
      else {
        air_move(chComponent.current_velocity, args, delta_time);
      }

      // Rotate the character and camera.

      const auto& camera = cameraComponent.system;
      const Vec2 yaw_pitch = Vec2(camera->get_yaw(), camera->get_pitch());
      Vec2 final_yaw_pitch = yaw_pitch;

      if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
        s_lock_camera = !s_lock_camera;

      if (s_lock_camera && Window::is_focused()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        const Vec2 new_mouse_position = Input::get_mouse_position();

        if (!s_using_camera) {
          s_using_camera = true;
          s_locked_mouse_position = new_mouse_position;
        }

        Input::set_cursor_position(s_locked_mouse_position.x, s_locked_mouse_position.y);

        const Vec2 change = (new_mouse_position - s_locked_mouse_position) * (s_mouse_sensitivity / 1000.0f);
        final_yaw_pitch.x += change.x;
        final_yaw_pitch.y = final_yaw_pitch.y - change.y;
      }
      else {
        s_using_camera = false;
      }

      auto char_rotation = JPH::Quat::sEulerAngles({0.0f, -final_yaw_pitch.x, 0.0f});
      chComponent.character->SetRotation(char_rotation);

      Vec3 final_position = chTransform.position;
      final_position.y += chComponent.character_height_standing;

      cameraTransform.position = final_position;
      cameraTransform.rotation.x = glm::clamp(final_yaw_pitch.y, glm::radians(-89.0f), glm::radians(89.0f));
      cameraTransform.rotation.y = final_yaw_pitch.x;

      // Cancel movement in opposite direction of normal when touching something we can't walk up
      const JPH::Character::EGroundState ground_state = chComponent.character->GetGroundState();
      if (ground_state == JPH::Character::EGroundState::OnSteepGround || ground_state == JPH::Character::EGroundState::NotSupported) {
        JPH::Vec3 normal = chComponent.character->GetGroundNormal();
        normal.SetY(0.0f);
        const float dot = normal.Dot(chComponent.current_velocity);
        if (dot < 0.0f)
          chComponent.current_velocity -= (dot * normal) / normal.LengthSq();
      }

      // Move the character.
      OX_CORE_ASSERT(!chComponent.current_velocity.IsNaN())
      chComponent.character->SetLinearVelocity(chComponent.current_velocity);
    }
  }
}

void CharacterSystem::on_imgui_render(Scene* scene, const Oxylus::Timestep& delta_time) {
  auto& registery = scene->m_registry;
  const auto character_view = registery.view<TransformComponent, CharacterControllerComponent, CharacterComponent>();
  for (const auto entity : character_view) {
    auto&& [transform, component, ch] = character_view.get<TransformComponent, CharacterControllerComponent, CharacterComponent>(entity);

    if (ImGui::Begin("Character Debug")) {
      OxUI::begin_properties();
      OxUI::property("Health", &ch.Health);
      OxUI::property("Armor", &ch.Armor);
      OxUI::property_vector("Position", transform.position);
      OxUI::property_vector("Rotation", transform.rotation);
      OxUI::property("AutoBunnyHop", &component.auto_bunny_hop);
      OxUI::property("JumpForce", &component.jump_force, 1.0f, 10.0f);
      OxUI::property("CollisionTolerance", &component.collision_tolerance, 0.05f, 0.5f);

      auto velocity = glm::make_vec3(component.current_velocity.mF32);
      auto speed = component.character->GetLinearVelocity().Length();
      OxUI::text("IsGrounded", fmt::format("{}", component.character->IsSupported()).c_str());
      OxUI::property_vector("Velocity", velocity);
      OxUI::property("Speed", &speed);
      if (s_max_vertical_velocity <= velocity.y)
        s_max_vertical_velocity = velocity.y;
      OxUI::text("Max Vertical Velocity", fmt::format("{}", s_max_vertical_velocity).c_str());
      if (s_max_horizontal_velocity <= velocity.x)
        s_max_horizontal_velocity = velocity.x;
      OxUI::text("Max Horizontal Velocity", fmt::format("{}", s_max_horizontal_velocity).c_str());
      OxUI::end_properties();
    }
    ImGui::End();
  }
}

void CharacterSystem::on_contact_added(Scene* scene, const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, const JPH::ContactSettings& settings) {
  auto& registery = scene->m_registry;
  const auto character_view = registery.view<TransformComponent, CharacterControllerComponent>();
  const auto rb_view = registery.view<TagComponent, TransformComponent, RigidbodyComponent>();

  for (const auto rb_entity : rb_view) {
    auto&& [tag, rbTransform, rb] = rb_view.get<TagComponent, TransformComponent, RigidbodyComponent>(rb_entity);

    const auto& id = ((JPH::Body*)rb.runtime_body)->GetID();

    if (!body1.IsSensor() || id != body1.GetID()) {
      continue;
    }

    if (tag.tag == "BouncePad") {
      for (const auto ch_entity : character_view) {
        auto&& [chTransform, ch] = character_view.get<TransformComponent, CharacterControllerComponent>(ch_entity);
        if (body2.GetID() == ch.character->GetBodyID()) {
          ch.character->SetLinearVelocity(JPH::Vec3{0.0f, 12.0f, 0.0f}, false);
        }
      }
    }
    else if (tag.tag == "AccelPad") {
      for (const auto ch_entity : character_view) {
        auto&& [chTransform, ch] = character_view.get<TransformComponent, CharacterControllerComponent>(ch_entity);
        if (body2.GetID() == ch.character->GetBodyID()) {
          ch.character->SetLinearVelocity(JPH::Vec3(rb.translation.x, rb.translation.y, rb.translation.z) * 30, false);
        }
      }
    }
  }
}

void CharacterSystem::queue_jump(const bool auto_bunny_hop) {
  if (auto_bunny_hop) {
    m_jump_queued = Input::get_key_pressed(KeyCode::Space);
    return;
  }

  if (Input::get_key_pressed(KeyCode::Space) && !m_jump_queued) {
    m_jump_queued = true;
  }

  if (Input::get_key_pressed(KeyCode::Space)) {
    m_jump_queued = false;
  }
}

void CharacterSystem::ground_move(JPH::Vec3& velocity, const MovementArgs& args, const float delta_time) {
  // Do not apply friction if the player is queueing up the next jump
  if (!args.jump_queued) {
    apply_friction(velocity, args.character_controller.ground_settings.deceleration, args.is_grounded, args.character_controller.friction, delta_time, 1.0f);
  }
  else {
    apply_friction(velocity, args.character_controller.ground_settings.deceleration, args.is_grounded, args.character_controller.friction, delta_time, 0.0f);
  }

  auto wishspeed = args.wish_dir.Length();
  wishspeed *= args.character_controller.ground_settings.max_speed;

  accelerate(velocity, args.wish_dir, wishspeed, args.character_controller.ground_settings.acceleration);

  // Reset the gravity velocity
  velocity.SetY(-args.character_controller.gravity * delta_time);

  if (args.jump_queued) {
    velocity.SetY(args.character_controller.jump_force);
    m_jump_queued = false;
  }
}

void CharacterSystem::air_move(JPH::Vec3& velocity, const MovementArgs& args, const float delta_time) {
  float accel;

  float wishspeed = args.wish_dir.Length();
  wishspeed *= args.character_controller.air_settings.max_speed;

  // CPM Air control.
  const float wishspeed2 = wishspeed;
  if (velocity.Dot(args.wish_dir) < 0) {
    accel = args.character_controller.air_settings.deceleration;
  }
  else {
    accel = args.character_controller.air_settings.acceleration;
  }

  // If the player is ONLY strafing left or right
  if (args.movement_direction.GetZ() == 0.0f && args.movement_direction.GetX() != 0.0f) {
    if (wishspeed > args.character_controller.strafe_settings.max_speed) {
      wishspeed = args.character_controller.strafe_settings.max_speed;
    }

    accel = args.character_controller.strafe_settings.acceleration;
  }

  accelerate(velocity, args.wish_dir, wishspeed, accel);
  if (args.character_controller.air_control > 0) {
    air_control(velocity, args.wish_dir, args.wish_dir, wishspeed2, args.character_controller.air_control);
  }

  // Apply gravity
  velocity.SetY(velocity.GetY() - args.character_controller.gravity * delta_time);
}

void CharacterSystem::air_control(JPH::Vec3& velocity, const JPH::Vec3 target_dir, JPH::Vec3 mov_input, const float target_speed, const float air_control) {
  // Only control air movement when moving forward or backward.
  if (glm::abs(target_dir.GetZ()) < 0.001f || glm::abs(target_speed) < 0.001f) {
    return;
  }

  const float z_speed = velocity.GetY();
  velocity.SetY(0);
  /* Next two lines are equivalent to idTech's VectorNormalize() */
  const float speed = velocity.Length();
  velocity = velocity.Normalized();

  const float dot = velocity.Dot(target_dir);
  float k = 32;
  k *= air_control * dot * dot * Application::get_timestep();

  // Change direction while slowing down.
  if (dot > 0) {
    velocity.SetX(velocity.GetX() * speed + target_dir.GetX() * k);
    velocity.SetY(velocity.GetY() * speed + target_dir.GetY() * k);
    velocity.SetZ(velocity.GetZ() * speed + target_dir.GetZ() * k);

    velocity = velocity.Normalized();
  }

  velocity.SetX(velocity.GetX() * speed);
  velocity.SetY(z_speed); // Note this line
  velocity.SetZ(velocity.GetZ() * speed);
}

void CharacterSystem::apply_friction(JPH::Vec3& velocity, const float ground_deceleration, const bool is_grounded, const float friction, const float delta_time, const float t) {
  JPH::Vec3 vec = velocity;
  vec.SetY(0.0f);
  const float speed = vec.Length();
  float drop = 0;

  // Only apply friction when grounded.
  if (is_grounded) {
    const float control = speed < ground_deceleration ? ground_deceleration : speed;
    drop = control * friction * delta_time * t;
  }

  float new_speed = speed - drop;
  // m_PlayerFriction = newSpeed; // For logging
  if (new_speed < 0) {
    new_speed = 0;
  }

  if (speed > 0) {
    new_speed /= speed;
  }

  velocity.SetX(velocity.GetX() * new_speed);
  velocity.SetZ(velocity.GetZ() * new_speed);
}

void CharacterSystem::accelerate(JPH::Vec3& velocity, const JPH::Vec3& target_dir, const float target_speed, const float accel) {
  const float currentspeed = velocity.Dot(target_dir);
  const float addspeed = target_speed - currentspeed;
  if (addspeed <= 0) {
    return;
  }

  float accelspeed = accel * Application::get_timestep() * target_speed;
  if (accelspeed > addspeed) {
    accelspeed = addspeed;
  }

  velocity.SetX(velocity.GetX() + accelspeed * target_dir.GetX());
  velocity.SetZ(velocity.GetZ() + accelspeed * target_dir.GetZ());
}
}
