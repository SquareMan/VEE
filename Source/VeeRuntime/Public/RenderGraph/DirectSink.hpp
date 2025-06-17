//
// Created by Square on 6/17/2025.
//

#pragma once

#include "Sink.hpp"

#include "MakeSharedEnabler.hpp"

#include <memory>

namespace vee::rdg {
template <typename T>
class DirectSink : public Sink {
public:
    static std::unique_ptr<Sink> make(std::shared_ptr<T>& target) {
        return std::make_unique<MakeSharedEnabler<DirectSink>>(target);
    }
    std::shared_ptr<T>& target;

protected:
    explicit DirectSink(std::shared_ptr<T>& target)
        : Sink()
        , target(target) {}
};
} // namespace vee::rdg
