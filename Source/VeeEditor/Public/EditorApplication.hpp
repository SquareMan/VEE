//
// Created by Square on 10/22/2024.
//

#pragma once
#include "Application.hpp"


namespace vee {
class EditorApplication : public Application {
public:
    explicit EditorApplication(platform::Window&& window);

    void run() override;
};
}; // namespace vee
