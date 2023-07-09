#include "PickupSystem.h"

#include <Scene/Scene.h>
#include <Core/Entity.h>

#include "CharacterSystem.h"

namespace OxArena {

  using namespace Oxylus;

  void UpdatePickupComponent(TransformComponent& transform,
                             PickupComponent& component,
                             TagComponent& tag,
                             float deltaTime) {
    constexpr float turnDegree = 35.0f;
    transform.Rotation.y += glm::radians(turnDegree) * deltaTime;
    if (component.PickedUp) {
      tag.Enabled = false;
      component.RespawnTimer -= deltaTime;
      component.RespawnTimer = glm::clamp(component.RespawnTimer, 0.0f, component.MaxRespawnTime);
      if (component.RespawnTimer < 0.01f) {
        tag.Enabled = true;
        component.PickedUp = false;
      }
    }
  }

  void PickupSystem::OnUpdate(Scene* scene, Timestep deltaTime) {
    auto& registery = scene->m_Registry;
    registery.view<TransformComponent, HealthPickupComponent>()
             .each([deltaTime, &registery](const entt::entity e,
                                           TransformComponent& transform,
                                           HealthPickupComponent& component) {
                auto& tag = registery.get<TagComponent>(e);
                UpdatePickupComponent(transform, component, tag, deltaTime);
              });
  }

  void PickupSystem::OnImGuiRender(Scene* scene, Timestep deltaTime) { }

  void PickupSystem::OnContactAdded(Scene* scene,
                                    const JPH::Body& body1,
                                    const JPH::Body& body2,
                                    const JPH::ContactManifold& manifold,
                                    const JPH::ContactSettings& settings) {
    auto& registery = scene->m_Registry;
    const auto characterView = registery.view<CharacterComponent, CharacterControllerComponent>();

    // TODO(hatrickek): Don't query for every rb component instead just query for pickups.
    const auto rbView = registery.view<RigidbodyComponent>();

    for (const auto rbEntity : rbView) {
      const auto& rb = rbView.get<RigidbodyComponent>(rbEntity);

      const auto& id = ((JPH::Body*)rb.RuntimeBody)->GetID();

      if (!body1.IsSensor() || id != body1.GetID()) {
        continue;
      }

      auto entity = Entity(rbEntity, scene);

      if (entity.HasComponent<HealthPickupComponent>()) {
        for (const auto chEntity : characterView) {
          const auto& [ch, cc] = characterView.get<CharacterComponent, CharacterControllerComponent>(chEntity);
          if (body2.GetID() == cc.Character->GetBodyID()) {
            auto& c = entity.GetComponent<HealthPickupComponent>();
            ch.Health += c.Value;
            c.PickedUp = true;
            c.RespawnTimer = c.MaxRespawnTime;
          }
        }
      }
      else if (entity.HasComponent<ArmorPickupComponent>()) {
        for (const auto chEntity : characterView) {
          const auto& [ch, cc] = characterView.get<CharacterComponent, CharacterControllerComponent>(chEntity);
          if (body2.GetID() == cc.Character->GetBodyID()) {
            auto& c = entity.GetComponent<ArmorPickupComponent>();
            ch.Armor += c.Value;
            c.PickedUp = true;
            c.RespawnTimer = c.MaxRespawnTime;
          }
        }
      }
    }
  }
}
