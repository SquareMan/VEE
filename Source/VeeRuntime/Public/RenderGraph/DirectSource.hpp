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

#include "Logging.hpp"
#include "RenderGraph/DirectSink.hpp"
#include "RenderGraph/RenderGraph.hpp"
#include "RenderGraph/Source.hpp"

namespace vee::rdg {
template <typename T>
class DirectSource : public Source {
public:
    static std::unique_ptr<Source> make(std::shared_ptr<T>& target) {
        struct MakeSharedEnabler : DirectSource {
            explicit MakeSharedEnabler(std::shared_ptr<T>& target)
                : DirectSource(target) {}
        };
        return std::make_unique<MakeSharedEnabler>(target);
    }

    void resolve(const RenderGraph& ctx) override {
        auto sink = dynamic_cast<DirectSink<T>*>(ctx.find_sink(sink_ref));
        if (sink == nullptr) {
            log_error("Linked sink was not of type {}!", typeid(DirectSink<T>).name());
            return;
        }

        target = sink->target;
        if (target == nullptr) {
            log_error("Failed to resolve target for {}!", typeid(DirectSource).name());
        }
    }


protected:
    explicit DirectSource(std::shared_ptr<T>& target)
        : Source()
        , target(target) {}

    std::shared_ptr<T>& target;
    friend class RenderGraph;
};
} // namespace vee::rdg