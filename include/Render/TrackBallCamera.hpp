//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_TRACKBALL_CAMERA_HPP
#define ELYS_TRACKBALL_CAMERA_HPP

#include <cmath>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Core/Base.hpp>
#include <Core/Geometry.hpp>
#include <Core/Logger.hpp>

#include <Render/Camera.hpp>

namespace Elys {
    class TrackBallCamera : public Camera {
      public:
        TrackBallCamera() = default;

        glm::mat4 GetProjection() const override;
        glm::mat4 GetView() const override;
        Frustum GetFrustum() const override;

        void Rotate(float deltaT, float deltaP);
        void Zoom(float delta);
        void Pan(glm::vec3 direction);
        void SetPosition(glm::vec3 newPosition);
        void SetTarget(glm::vec3 newTarget);
        
        bool IsCapturing();
        void MouseInput(float x, float y, MouseCode button) override;
        void ReleaseInput();

        [[nodiscard]] glm::vec3 GetPosition() const override { return m_position; }
        [[nodiscard]] glm::vec3 GetFront() const { return glm::normalize(m_forward); }

      private:
        void UpdateCameraData() const;

      public:
        float speed = 20.0f;
        float sensitivity = 250.0f;
      private:
        // cache data
        mutable Frustum m_frustum;
        mutable glm::mat4 m_projection{1.0f};
        mutable glm::mat4 m_view{1.0f};

        glm::vec3 m_position{0.0f, 0.0f, 0.0f};
        glm::vec3 m_forward{0.0, 0.0, 1.0f};
        float m_yaw = -90.0f;
        float m_pitch = 0.0f;
        
        bool m_newCapture = true;
        float m_lastMouseX = 0.0f, m_lastMouseY = 0.0f;
    };
} // namespace Elys

#endif // ELYS_TRACKBALL_CAMERA_HPP