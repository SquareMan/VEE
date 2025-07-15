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

namespace vee {
class RenderCtx;
}

namespace vee::rdg {
class RenderGraph;
class Sink {
public:
    virtual ~Sink();

    /**
     * Called by the RenderGraph before Sources and Sinks are wired up to give a chance to
     * initialize and required graphics resources
     * @param ctx Engine global rendering context.
     */
    virtual void init(RenderCtx& ctx) {};

    /**
     * Called by the RenderGraph each frame before execution.
     * @param ctx
     */
    virtual void prepare(const RenderGraph& ctx) {};
};
} // namespace vee::rdg
