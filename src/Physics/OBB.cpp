//
// Created by pierrhum on 08/05/2022.
//

#include "Physics/OBB.hpp"

namespace Elys {
    OBB::OBB(glm::vec3 const &center, glm::vec3 const &size, glm::mat3 const &rotation) :
        mCenter(center), mSize(size), mRotation(rotation) {
        // UpdateVertices();
    }

    void OBB::Update(glm::vec3 const &center, glm::vec3 const &size, glm::mat3 const &rotation) {
        mCenter = center;
        mSize = size;
        mRotation = rotation;

        // UpdateVertices();
    }

    void OBB::UpdateVertices() {
        mVertices = {
            mCenter + mRotation[0] * mSize[0] + mRotation[1] * mSize[1] + mRotation[2] * mSize[2],
            mCenter - mRotation[0] * mSize[0] + mRotation[1] * mSize[1] + mRotation[2] * mSize[2],
            mCenter + mRotation[0] * mSize[0] - mRotation[1] * mSize[1] + mRotation[2] * mSize[2],
            mCenter + mRotation[0] * mSize[0] + mRotation[1] * mSize[1] - mRotation[2] * mSize[2],
            mCenter - mRotation[0] * mSize[0] - mRotation[1] * mSize[1] - mRotation[2] * mSize[2],
            mCenter + mRotation[0] * mSize[0] - mRotation[1] * mSize[1] - mRotation[2] * mSize[2],
            mCenter - mRotation[0] * mSize[0] + mRotation[1] * mSize[1] - mRotation[2] * mSize[2],
            mCenter - mRotation[0] * mSize[0] - mRotation[1] * mSize[1] + mRotation[2] * mSize[2],
        };

        static uint32_t indices[] = {
            6, 1,
            6, 3,
            6, 4,
            2, 7,
            2, 5,
            2, 0,
            0, 1,
            0, 3,
            7, 1,
            7, 4,
            4, 5,
            5, 3
        };

        if (mVAO) {
            mVAO->GetVertexBuffer()->SetData((void *)mVertices.data(),
                                             static_cast<uint32_t>(mVertices.size() * sizeof(glm::vec3)),
                                             GL_DYNAMIC_DRAW);
        } else {
            mVAO = std::make_shared<VertexArray>();
            auto vbo = std::make_shared<VertexBuffer>(
                (void *)mVertices.data(), static_cast<uint32_t>(mVertices.size() * sizeof(glm::vec3)),
                GL_DYNAMIC_DRAW);

            auto ebo = std::make_shared<IndexBuffer>(indices, 24);
            BufferLayout vertexLayout{{"position", sizeof(glm::vec3), 3, GL_FLOAT}};
            vbo->SetLayout(vertexLayout);
            mVAO->SetVertexBuffer(vbo);
            mVAO->SetIndexBuffer(ebo);
        }
    }

    std::pair<float, float> OBB::GetInterval(OBB const &obb, glm::vec3 const &axis) {
        std::pair<float, float> result{
            glm::dot(axis, obb.mVertices[0]),
            glm::dot(axis, obb.mVertices[0])
        };

        for(uint8_t i = 1; i < 8; i++) {
            float projection = glm::dot(axis, obb.mVertices[i]);

            result.first = projection < result.first ? projection : result.first;
            result.second = projection > result.second ? projection : result.second;
        }

        return result;
    }

    bool OBB::CollapseOnAxis(const OBB &left, const OBB &right, const glm::vec3 &axis) {
        auto a = OBB::GetInterval(left, axis);
        auto b = OBB::GetInterval(right, axis);

        return ((b.first <= a.second) && (a.first <= b.second));
    }

    bool OBB::Collapse(const OBB &left, const OBB &right) {
        std::array<glm::vec3, 15> testAxis = {
            left.mRotation[0], left.mRotation[1], left.mRotation[2],
            right.mRotation[0], right.mRotation[1], right.mRotation[2]
        };

        for(uint8_t i = 0; i < 3; i++) {
            testAxis[6 + i * 3 + 0] = glm::cross(testAxis[i], testAxis[3]);
            testAxis[6 + i * 3 + 1] = glm::cross(testAxis[i], testAxis[4]);
            testAxis[6 + i * 3 + 2] = glm::cross(testAxis[i], testAxis[5]);
        }

        for(auto const &axis : testAxis) {
            if (!CollapseOnAxis(left, right, axis)) return false;
        }

        return true;
    }
}