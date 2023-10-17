//
// Created by Sam on 3/10/2022.
//

#ifndef ELYS_LAYER_STACK_HPP
#define ELYS_LAYER_STACK_HPP

#include <memory>
#include <vector>

#include <Core/Layer.hpp>

namespace Elys {
    class LayerStack {
      public:
        LayerStack() = default;
        ~LayerStack();

        void PushLayer(std::shared_ptr<Layer> layer);
        void PushOverlay(std::shared_ptr<Layer> overlay);
        void PopLayer(std::shared_ptr<Layer> layer);
        void PopOverlay(std::shared_ptr<Layer> overlay);

        std::vector<std::shared_ptr<Layer>>::iterator begin() { return m_layers.begin(); }
        std::vector<std::shared_ptr<Layer>>::iterator end() { return m_layers.end(); }
        std::vector<std::shared_ptr<Layer>>::reverse_iterator rbegin() { return m_layers.rbegin(); }
        std::vector<std::shared_ptr<Layer>>::reverse_iterator rend() { return m_layers.rend(); }

        [[nodiscard]] std::vector<std::shared_ptr<Layer>>::const_iterator begin() const {
            return m_layers.begin();
        }
        [[nodiscard]] std::vector<std::shared_ptr<Layer>>::const_iterator end() const {
            return m_layers.end();
        }
        [[nodiscard]] std::vector<std::shared_ptr<Layer>>::const_reverse_iterator rbegin() const {
            return m_layers.rbegin();
        }
        [[nodiscard]] std::vector<std::shared_ptr<Layer>>::const_reverse_iterator rend() const {
            return m_layers.rend();
        }

      private:
        std::vector<std::shared_ptr<Layer>> m_layers;
        unsigned int m_layerInsertIndex = 0;
    };
}

#endif // ELYS_LAYER_STACK_HPP
