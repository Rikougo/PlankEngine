//
// Created by Sam on 3/24/2022.
//

#include "ECS/Components/Node.hpp"

using glm::vec3;
using glm::mat4;
using glm::quat;

using std::vector;

namespace Elys {
    Node::Node()
        : m_parent(nullptr), m_localPosition(0.0f, 0.0f, 0.0f),
          m_localRotation(vec3(0.0f, 0.0f, 0.0f)), m_localScale(1.0f, 1.0f, 1.0f)
    {}

    void Node::SetParent(Node *parent) {
        if (parent == this) {
            ELYS_CORE_WARN("Can't add self as parent !");
            return;
        }

        if (m_parent) {
            m_parent->RemoveChild(this);
        }

        if (parent) {
            m_parent = parent;
            m_parent->AddChild(this);
        } else {
            m_parent = nullptr;
        }
    }
    void Node::AddChild(Node *child) {
        if (std::find(m_children.begin(), m_children.end(), child) != m_children.end()) {
            ELYS_CORE_WARN("Child node is already a child of parent node.");
            return;
        }

        if (child == this) {
            ELYS_CORE_WARN("Can't put self as child.");
            return;
        }

        m_children.push_back(child);
        child->m_parent = this;
    }
    void Node::RemoveChild(Node *node) {
        auto it = std::find(m_children.begin(), m_children.end(), node);
        if (it != m_children.end()) {
            (*it)->m_parent = nullptr;
            m_children.erase(it);
        }
        else ELYS_CORE_WARN("Child not found.");
    }

    Node* Node::Parent() const { return m_parent; }
    vector<Node *> Node::Children() const { return m_children; }

    void Node::OnDelete() {
        if (m_parent)
            m_parent->RemoveChild(this);

        // set new parent the parent of deleted node
        // no matter if it hasn't any parent (children will have no parent)
        for (auto child : m_children) {
            child->SetParent(m_parent);
        }
    };

    vec3 Node::InheritedPosition() const {
        if (!m_globalUpdated)
            UpdateTransform();
        return m_globalPosition;
    }
    vec3 Node::InheritedScale() const {
        if (!m_globalUpdated)
            UpdateTransform();
        return m_globalScale;
    }
    quat Node::InheritedRotation() const {
        if (!m_globalUpdated)
            UpdateTransform();
        return m_globalRotation;
    }
    mat4 Node::InheritedTransform() const {
        if (!m_globalUpdated)
            UpdateTransform();

        return m_globalTransform;
    }

    bool Node::InheritedEnabled() const {
        if (m_parent) return m_parent->LocalEnabled() && m_localEnabled;
        else return m_localEnabled;
    }

    vec3 Node::LocalPosition() const { return m_localPosition; }
    vec3 Node::LocalScale() const { return m_localScale; }
    quat Node::LocalRotation() const { return m_localRotation; }
    mat4 Node::LocalTransform() const { return m_localTransform; }
    bool Node::LocalEnabled() const { return m_localEnabled; }

    void Node::Move(vec3 translation) {
        m_localPosition += translation;
        InvalidateNode();
    }
    void Node::Rotate(quat rotation) {
        m_localRotation *= rotation;
        InvalidateNode();
    }
    void Node::Scale(vec3 scale) {
        m_localScale *= scale; InvalidateNode(); }

    void Node::SetPosition(vec3 position) {
        if (position != m_localPosition) {
            InvalidateNode();
            m_localPosition = position;
        }
    }
    void Node::SetPosition(float x, float y, float z) { SetPosition(vec3(x, y, z)); }

    void Node::SetRotation(quat rotation) {
        if (m_localRotation != rotation) {
            InvalidateNode();
            m_localRotation = rotation;
        }
    }
    void Node::SetRotation(vec3 eulerAngles) { SetRotation(quat(eulerAngles)); }
    void Node::SetRotation(float eulerX, float eulerY, float eulerZ) { SetRotation(vec3(eulerX, eulerY ,eulerZ)); }

    void Node::SetScale(vec3 scale) {
        if (m_localScale != scale) {
            m_localScale = scale;
            InvalidateNode();
        }
    }
    void Node::SetScale(float x, float y, float z) { SetScale(vec3(x, y, z)); }
    void Node::SetScale(float uniformScale) { SetScale(vec3(uniformScale, uniformScale, uniformScale)); }

    void Node::SetEnabled(bool enabled) { m_localEnabled = enabled; }

    void Node::InvalidateNode() const {
        for(auto child : m_children) child->InvalidateNode();

        m_globalUpdated = false;
    }

    void Node::UpdateTransform() const {
        if (m_parent) {
            if (!m_parent->m_globalUpdated)
                m_parent->UpdateTransform();

            m_globalPosition =
                m_parent->m_globalRotation * (m_parent->m_globalScale * m_localPosition) +
                m_parent->m_globalPosition;
            m_globalRotation = m_parent->m_globalRotation * m_localRotation;
            m_globalScale = m_parent->m_globalScale * m_localScale;
        } else {
            m_globalPosition = m_localPosition;
            m_globalRotation = m_localRotation;
            m_globalScale = m_localScale;
        }

        m_globalTransform = mat4(1.0f);
        m_globalTransform = glm::translate(m_globalTransform, m_globalPosition);
        m_globalTransform *= glm::mat4_cast(m_globalRotation);
        m_globalTransform = glm::scale(m_globalTransform, m_globalScale);

        m_localTransform = mat4(1.0f);
        m_localTransform = glm::translate(m_localTransform, m_localPosition);
        m_localTransform *= glm::mat4_cast(m_localRotation);
        m_localTransform = glm::translate(m_localTransform, m_localScale);

        m_globalUpdated = true;
    }
} // namespace Elys