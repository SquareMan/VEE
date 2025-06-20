//
// Created by Square on 6/14/2025.
//

#pragma once

#include "Handles.hpp"


#include <memory>
#include <ranges>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace vee::rdg {
class Source;
class Sink;

class Pass {
public:
    virtual ~Pass();

    /**
     * Called by the RenderGraph every frame. Record rendering commands here.
     * @param cmd Command Buffer for rendering.
     */
    virtual void execute(vk::CommandBuffer cmd) = 0;

    /**
     * Link an external Sink to a Source belonging to this Pass. Linked Sinks may be global to the
     * Render Graph or a Sink belonging to another Pass in the Render Graph.
     * @param sink Reference to the sink we want to link.
     * @param source Handle to a source owned by this pass.
     * @return Reference to self
     */
    Pass& link_sink(SinkRef sink, SourceHandle source);

    /**
     * @return Iterator of all Sources belonging to this pass.
     */
    auto iterate_sources() const {
        return sources_ | std::views::values;
    }

    /**
     * @return Iterator of all Sinks belonging to this pass.
     */
    auto iterate_sinks() const {
        return sinks_ | std::views::values;
    }

    /**
     * Attempt to retrieve a pointer to a Source owned by this pass.
     * @param handle Handle to the Source
     * @return The Source, if found
     */
    Source* find_source(SourceHandle handle) const;

    /**
     * Attempt to retrieve a pointer to a Sink owned by this pass.
     * @param handle Handle to the Sink
     * @return The Sink, if found
     */
    Sink* find_sink(SinkHandle handle) const;

protected:
    std::unordered_map<SourceHandle, std::unique_ptr<Source>> sources_;
    std::unordered_map<SinkHandle, std::unique_ptr<Sink>> sinks_;


    /**
     * Register a Source with this Pass and assign it a handle that will be used to refer to it
     * later. Call this during construction of the Pass.
     * @param name Handle to assign
     * @param source Source to register
     */
    void register_source(SourceHandle name, std::unique_ptr<Source> source);

    /**
     * Register a Sink with thes Pass and assign it a handle that will be used to refer to it later.
     * Call this during construction of the Pass.
     * @param name Handle to assign
     * @param sink SInk to register
     */
    void register_sink(SinkHandle name, std::unique_ptr<Sink> sink);

    friend class RenderGraph;
};


} // namespace vee::rdg