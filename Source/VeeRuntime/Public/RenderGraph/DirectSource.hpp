//
// Created by Square on 6/14/2025.
//

#pragma once

#include "RenderGraph/DirectSink.hpp"
#include "RenderGraph/RenderGraph.hpp"

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