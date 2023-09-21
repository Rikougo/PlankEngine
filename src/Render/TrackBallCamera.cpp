//
// Created by Sam on 3/14/2022.
//

#include <Render/TrackBallCamera.hpp>

namespace Elys {
    glm::mat4 TrackBallCamera::GetProjection() const {
        if (m_dirty)
            UpdateCameraData();
        return m_projection;
    }
    glm::mat4 TrackBallCamera::GetView() const {
        if (m_dirty)
            UpdateCameraData();
        return m_view;
    }
    Frustum TrackBallCamera::GetFrustum() const {
        if (m_dirty)
            UpdateCameraData();
        return m_frustum;
    }

    void TrackBallCamera::Rotate(float deltaX, float deltaY) {
        m_yaw += deltaX;
        m_pitch += deltaY;

        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

        m_forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_forward.y = sin(glm::radians(m_pitch));
        m_forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

        m_dirty = true;
    }
    void TrackBallCamera::Pan(glm::vec3 direction) {
        m_forward = glm::normalize(m_forward);
        glm::vec3 right = glm::normalize(glm::cross(m_forward, {0.0f, 1.0f, 0.0f}));
        glm::vec3 up = glm::normalize(glm::cross(right, m_forward));

        auto deltaSide = right * direction.x * speed;
        auto deltaUP = up * direction.y * speed;
        auto deltaForward = m_forward * direction.z * speed;

        m_position += deltaSide + deltaUP + deltaForward;

        m_dirty = true;
    }
    void TrackBallCamera::Zoom(float delta) {
        Pan({0.0f, 0.0f, 1.0f * delta});
    }
    void TrackBallCamera::SetPosition(glm::vec3 newPosition) {
        m_position = newPosition;
        m_dirty = true;
    }
    void TrackBallCamera::SetTarget(glm::vec3 newTarget) {
        m_forward = glm::normalize(newTarget - m_position);
        m_dirty = true;
    }

    bool TrackBallCamera::IsCapturing() { return !m_newCapture; }
    void TrackBallCamera::MouseInput(float x, float y, MouseCode button) {
        if (m_newCapture) {
            m_lastMouseX = (x / mViewWidth);
            m_lastMouseY = (y / m_viewHeight);
        }

        float l_dx = m_lastMouseX - (x / mViewWidth);
        float l_dy = (y / m_viewHeight) - m_lastMouseY;
        
        switch (button) { 
            case Mouse::ButtonRight:
                Rotate(-l_dx * sensitivity, -l_dy * sensitivity);
                break;
            case Mouse::ButtonMiddle:
                Pan({l_dx, l_dy, 0.0f});
                break;
            default:
                break;
        }
        
        m_lastMouseX = (x / mViewWidth);
        m_lastMouseY = (y / m_viewHeight);
        
        m_newCapture = false;
    }
    void TrackBallCamera::ReleaseInput() {
        m_newCapture = true;
    }

    void TrackBallCamera::UpdateCameraData() const {
        auto position = GetPosition();
        auto forward = glm::normalize(m_forward);
        auto right =
            glm::normalize(glm::cross(forward, {0.0f, 1.0f, 0.0f})); // cross(front, worldup)
        glm::vec3 up = glm::normalize(glm::cross(right, forward));

        const float halfVSide = m_far * tanf(m_fov * .5f);
        const float halfHSide = halfVSide * m_ratioAspect;
        const glm::vec3 frontXFar = m_far * forward;

        // MATRIX
        m_projection = glm::perspective(m_fov, m_ratioAspect, m_near, m_far);
        m_view = glm::lookAt(position, position + forward, {0.0f, 1.0f, 0.0f});

        auto vp = m_projection * m_view;
        glm::vec4 row1 = vp[0], row2 = vp[1], row3 = vp[2], row4 = vp[3];

        m_frustum.nearFace   = Geometry::Plan(position + (m_near * forward), forward);
        m_frustum.farFace    = Geometry::Plan(position + frontXFar, -forward);
        m_frustum.rightFace  = Geometry::Plan(position, glm::cross(up, frontXFar + right * halfHSide));
        m_frustum.leftFace   = Geometry::Plan(position, glm::cross(frontXFar - right * halfHSide, up));
        m_frustum.topFace    = Geometry::Plan(position, glm::cross(right, frontXFar - up * halfVSide));
        m_frustum.bottomFace = Geometry::Plan(position, glm::cross(frontXFar + up * halfVSide, right));

        m_dirty = false;
    }
} // namespace Elys