//
// Created by Sam on 3/28/2022.
//

#include <Render/AABB.hpp>

namespace Elys {
    // https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
    bool AABB::IsInFrustum(Frustum frustum, mat4 transform) const {
        vec3 tMin = transform * glm::vec4(lo, 1.0f), tMax = transform * glm::vec4(hi, 1.0f);
        AABB transformedAABB(tMin, tMax);

        return transformedAABB.IsForwardPlan(frustum.topFace) &&
            transformedAABB.IsForwardPlan(frustum.bottomFace) &&
            transformedAABB.IsForwardPlan(frustum.leftFace) &&
            transformedAABB.IsForwardPlan(frustum.rightFace) &&
            transformedAABB.IsForwardPlan(frustum.nearFace) &&
            transformedAABB.IsForwardPlan(frustum.farFace);
    }

    bool AABB::IsForwardPlan(const Geometry::Plan& plan) const {
        vec3 center = (hi + lo) * 0.5f;
        vec3 extent = hi - center;

        float r = extent.x * std::abs(plan.normal.x) + extent.y + std::abs(plan.normal.y) + extent.z * std::abs(plan.normal.z);
        float signedDistance = plan.GetSignedDistance(center);
        return -r <= signedDistance;
    }
    array<vec3, 8> AABB::Vertices() const {
        return {};
    }
}