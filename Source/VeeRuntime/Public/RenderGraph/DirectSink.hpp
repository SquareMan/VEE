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
