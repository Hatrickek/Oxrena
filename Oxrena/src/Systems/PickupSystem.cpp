#include "PickupSystem.h"

#include <Scene/Scene.h>
#include <Core/Entity.h>

#include "CharacterSystem.h"

namespace Oxrena {
using namespace Oxylus;

void update_pickup_component(TransformComponent& transform,
                           PickupComponent& component,
                           TagComponent& tag,
                           const Timestep& delta_time) {
  constexpr float turn_degree = 35.0f;
  transform.rotation.y += glm::radians(turn_degree) * delta_time;
  if (component.PickedUp) {
    tag.enabled = false;
    component.RespawnTimer -= delta_time;
    component.RespawnTimer = glm::clamp(component.RespawnTimer, 0.0f, component.MaxRespawnTime);
    if (component.RespawnTimer < 0.01f) {
      tag.enabled = true;
      component.PickedUp = false;
    }
  }
}

void PickupSystem::on_update(Scene* scene, const Timestep& delta_time) {
  auto& registery = scene->m_registry;
  registery.view<TransformComponent, HealthPickupComponent>()
           .each([&delta_time, &registery](const entt::entity e,
                                         TransformComponent& transform,
                                         HealthPickupComponent& component) {
              auto& tag = registery.get<TagComponent>(e);
              update_pickup_component(transform, component, tag, delta_time);
            });
}

void PickupSystem::on_imgui_render(Scene* scene, const Timestep& deltaTime) { }

void PickupSystem::on_contact_added(Scene* scene,
                                    const JPH::Body& body1,
                                    const JPH::Body& body2,
                                    const JPH::ContactManifold& manifold,
                                    const JPH::ContactSettings& settings) {
  auto& registery = scene->m_registry;
  const auto characterView = registery.view<CharacterComponent, CharacterControllerComponent>();

  // TODO(hatrickek): Don't query for every rb component instead just query for pickups.
  const auto rbView = registery.view<RigidbodyComponent>();

  for (const auto rbEntity : rbView) {
    const auto& rb = rbView.get<RigidbodyComponent>(rbEntity);

    const auto& id = ((JPH::Body*)rb.runtime_body)->GetID();

    if (!body1.IsSensor() || id != body1.GetID()) {
      continue;
    }

    auto entity = Entity(rbEntity, scene);

    if (entity.has_component<HealthPickupComponent>()) {
      for (const auto chEntity : characterView) {
        const auto& [ch, cc] = characterView.get<CharacterComponent, CharacterControllerComponent>(chEntity);
        if (body2.GetID() == cc.character->GetBodyID()) {
          auto& c = entity.get_component<HealthPickupComponent>();
          ch.Health += c.Value;
          c.PickedUp = true;
          c.RespawnTimer = c.MaxRespawnTime;
        }
      }
    }
    else if (entity.has_component<ArmorPickupComponent>()) {
      for (const auto chEntity : characterView) {
        const auto& [ch, cc] = characterView.get<CharacterComponent, CharacterControllerComponent>(chEntity);
        if (body2.GetID() == cc.character->GetBodyID()) {
          auto& c = entity.get_component<ArmorPickupComponent>();
          ch.Armor += c.Value;
          c.PickedUp = true;
          c.RespawnTimer = c.MaxRespawnTime;
        }
      }
    }
  }
}
}
