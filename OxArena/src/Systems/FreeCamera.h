#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Core/Systems/System.h"

namespace OxArena {
  class FreeCamera : public Oxylus::System {
  public:
    FreeCamera(const bool* use) : m_UseCamera(use) { }

    void OnUpdate(Oxylus::Scene* scene, Oxylus::Timestep deltaTime) override;

  private:
    //Camera
    float m_TranslationDampening = 0.6f;
    float m_RotationDampening = 0.3f;
    bool m_SmoothCamera = true;
    float m_MouseSensitivity = 0.5f;
    float m_MovementSpeed = 5.0f;
    bool m_UsingCamera = false;
    const bool* m_UseCamera = nullptr;
    bool m_BlockPlayerInput = false;
    glm::vec2 m_LockedMousePosition = glm::vec2(0.0f);
    glm::vec3 m_TranslationVelocity = glm::vec3(0);
    glm::vec2 m_RotationVelocity = glm::vec2(0);
  };
}
