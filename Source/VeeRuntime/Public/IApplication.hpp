//
// Created by Square on 4/28/2025.
//

#pragma once

namespace vee {
class Engine;
class Renderer;
class IApplication {
public:
    IApplication() = default;
    IApplication(const IApplication&) = delete;
    IApplication(IApplication&&) = delete;
    IApplication& operator=(const IApplication&) = delete;
    IApplication& operator=(IApplication&&) = delete;
    virtual ~IApplication() = default;

    virtual void run() = 0;
    virtual Engine& get_engine() = 0;
    virtual Renderer& get_renderer() = 0;
};
}
