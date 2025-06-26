//
// Created by square on 6/24/25.
//

#pragma once

#include <glm/vec2.hpp>
#include <imgui.h>
#include <meta>

namespace vee::editor {

template <typename T>
void create_inspector(T& val, std::string_view label) {
    static_assert(false, "Unsupported type");
}

template <typename T>
requires std::is_class_v<T>
void create_inspector(T& val, std::string_view label = std::meta::display_string_of(^^T)) {
    constexpr std::meta::info val_ty = std::meta::dealias(^^T);
    if constexpr (std::meta::is_class_type(val_ty)) {
        ImGui::Text("%s", label.data());
        ImGui::Indent(16);
        template for (constexpr auto field : define_static_array(std::meta::nonstatic_data_members_of(val_ty, std::meta::access_context::unchecked()))) {
            create_inspector(val.[:field:], std::meta::display_string_of(field));
        }
        ImGui::Unindent(16);
    }
}

template <>
void create_inspector(int& val, std::string_view label);

template <>
void create_inspector(float& val, std::string_view label);

template <>
void create_inspector(glm::vec2& val, std::string_view label);

} // namespace vee::editor
