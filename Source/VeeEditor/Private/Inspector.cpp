//
// Created by square on 6/24/25.
//

#include "Inspector.hpp"

namespace vee::editor {
template <>
void create_inspector<int>(int& val, std::string_view label) {
    ImGui::DragInt(label.data(), &val);
}
template <>
void create_inspector<float>(float& val, std::string_view label) {
    ImGui::DragFloat(label.data(), &val);
}
template <>
void create_inspector<glm::vec<2, float>>(glm::vec2& val, std::string_view label) {
    ImGui::DragFloat2(label.data(), &val.x);
}
} // namespace vee::editor