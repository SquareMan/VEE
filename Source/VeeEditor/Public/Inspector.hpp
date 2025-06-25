//
// Created by square on 6/24/25.
//

#pragma once

#include "Logging.hpp"

#include <glm/glm.hpp>
#include <imgui.h>
#include <meta>
#include <string>

namespace vee::editor {

template <typename T>
static void create_inspector(T& val, std::string_view label = std::meta::display_string_of(^^T)) {
    constexpr std::meta::info val_ty = std::meta::dealias(^^T);
    if constexpr (val_ty == ^^int) {
        ImGui::DragInt(label.data(), &val);
    } else if constexpr (val_ty == ^^float) {
        ImGui::DragFloat(label.data(), &val);
    } else if constexpr (val_ty == std::meta::dealias(^^glm::vec2)) {
        ImGui::DragFloat2(label.data(), reinterpret_cast<float*>(&val.x));
    } else if constexpr (std::meta::is_class_type(val_ty)) {
        ImGui::Text(label.data());
        ImGui::Indent(16);
        template for (constexpr auto field : define_static_array(std::meta::nonstatic_data_members_of(val_ty, std::meta::access_context::unchecked()))) {
            create_inspector(val.[:field:], std::meta::display_string_of(field));
        }
        ImGui::Unindent(16);
    } else {
        log_warning("Unsupported edit type {}", std::meta::display_string_of(val_ty));
    }
}


} // namespace vee::editor
