//    Copyright 2025 Steven Casper
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.


#pragma once

#include "Assert.hpp"
#include "RenderGraph/Pass.hpp"
#include "RenderGraph/RenderGraph.hpp"

namespace vee {
class RenderCtx;
}

namespace vee::rdg {

/**
 * Describes the collection of Passes and how their Sources/Sinks are linked together for a
 * RenderGraph. After all Passes are added a RenderGraph can be built/compiled.
 */
class RenderGraphBuilder {
public:
    ~RenderGraphBuilder();

    /**
     * Creates a new empty Pass.
     * @param name a name used to refer to this pass from other passes. Pass names must be unique
     * within a RenderGraph.
     * @param args Arguments to forward to the Pass constructor
     * @return Reference to the newly created pass. Returned reference may be invalidated if this
     * function is called again.
     */
    template <typename T, typename... Args>
        requires std::is_base_of_v<Pass, T>
    Pass& add_pass(PassHandle name, Args&&... args) {
        VASSERT(!passes_.contains(name), "Attempted to add two RenderGraph passes with the same name");
        std::unique_ptr<Pass> pass = std::make_unique<T>(std::forward<Args>(args)...);
        pass_order_.push_back(name);
        return *passes_.insert({name, std::move(pass)}).first->second;
    }

    /**
     * Compile a RenderGraph. This builder is invalidated afterward.
     * @param render_ctx Engine global rendering context
     * @return Compiled RenderGraph
     */
    RenderGraph build(RenderCtx& render_ctx);

protected:
    std::unordered_map<PassHandle, std::unique_ptr<Pass>> passes_;
    std::vector<PassHandle> pass_order_;
};

} // namespace vee::rdg