//
// Created by Square on 1/13/2025.
//

module;
#include "Engine/Service.hpp"
#include "Engine/Engine.hpp"
export module Vee.Engine;

import Vee.Renderer;

namespace vee {
export class Application : public Service<Application> {
public:
    virtual ~Application() = default;

    virtual void run() = 0;

    virtual Engine& get_engine() = 0;
    virtual Renderer& get_renderer() = 0;
};
};