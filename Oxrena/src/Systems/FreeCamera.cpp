#include "FreeCamera.h"
#include <imgui.h>

#include "OxrenaLayer.h"

#include "Core/Components.h"
#include "Core/Input.h"
#include "Scene/Scene.h"
#include "Utils/OxMath.h"
#include "Utils/Timestep.h"

namespace Oxrena {
using namespace Oxylus;

void FreeCamera::on_update(Scene* scene, const Timestep& deltaTime) {
  if (!*m_UseCamera) {
    if (m_BlockPlayerInput) {
      OxrenaLayer::get()->blocking_player_input = false;
      m_BlockPlayerInput = false;
    }
    return;
  }

  OxrenaLayer::get()->blocking_player_input = true;
  m_BlockPlayerInput = true;

  auto& registery = scene->m_registry;
  const auto group = registery.view<TransformComponent, CameraComponent>();
  for (const auto entity : group) {
    auto&& [transform, component] = group.get<TransformComponent, CameraComponent>(entity);
    auto& camera = component.system;
    const glm::vec3& position = camera->get_position();
    const glm::vec2 yawPitch = glm::vec2(camera->get_yaw(), camera->get_pitch());
    glm::vec3 finalPosition = position;
    glm::vec2 finalYawPitch = yawPitch;

    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
      ImGui::SetMouseCursor(ImGuiMouseCursor_None);
      const glm::vec2 newMousePosition = Input::get_mouse_position();

      if (!m_UsingCamera) {
        m_UsingCamera = true;
        m_LockedMousePosition = newMousePosition;
      }

      Input::set_cursor_position(m_LockedMousePosition.x, m_LockedMousePosition.y);

      const glm::vec2 change = (newMousePosition - m_LockedMousePosition) * m_MouseSensitivity;
      finalYawPitch.x += change.x;
      finalYawPitch.y = glm::clamp(finalYawPitch.y - change.y, -89.9f, 89.9f);

      const float maxMoveSpeed = m_MovementSpeed * (ImGui::IsKeyDown(ImGuiKey_LeftShift) ? 3.0f : 1.0f);
      if (ImGui::IsKeyDown(ImGuiKey_W))
        finalPosition += camera->get_front() * maxMoveSpeed;
      else if (ImGui::IsKeyDown(ImGuiKey_S))
        finalPosition -= camera->get_front() * maxMoveSpeed;
      if (ImGui::IsKeyDown(ImGuiKey_D))
        finalPosition += camera->get_right() * maxMoveSpeed;
      else if (ImGui::IsKeyDown(ImGuiKey_A))
        finalPosition -= camera->get_right() * maxMoveSpeed;

      if (ImGui::IsKeyDown(ImGuiKey_Q)) {
        finalPosition.y -= maxMoveSpeed;
      }
      else if (ImGui::IsKeyDown(ImGuiKey_E)) {
        finalPosition.y += maxMoveSpeed;
      }
    }
    else {
      m_UsingCamera = false;
    }

    const glm::vec3 dampedPosition = Math::smooth_damp(position,
                                                       finalPosition,
                                                       m_TranslationVelocity,
                                                       m_TranslationDampening,
                                                       10000.0f,
                                                       deltaTime);
    const glm::vec2 dampedYawPitch = Math::smooth_damp(yawPitch,
                                                       finalYawPitch,
                                                       m_RotationVelocity,
                                                       m_RotationDampening,
                                                       1000.0f,
                                                       deltaTime);

    transform.position = m_SmoothCamera ? dampedPosition : finalPosition;
    transform.rotation.x = m_SmoothCamera ? dampedYawPitch.y : finalYawPitch.y;
    transform.rotation.y = m_SmoothCamera ? dampedYawPitch.x : finalYawPitch.x;
  }
}
}
