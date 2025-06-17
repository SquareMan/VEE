//
// Created by Square on 6/14/2025.
//

#pragma once

#include "RenderGraph/Handles.hpp"


namespace vee::rdg {
class RenderGraph;
class Source {
public:
    virtual ~Source() = default;
    virtual void resolve(const RenderGraph& rg) = 0;

    SinkRef sink_ref = {};
};
} // namespace vee::rdg
