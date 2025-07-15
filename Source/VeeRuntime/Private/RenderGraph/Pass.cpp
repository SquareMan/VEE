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


#include "RenderGraph/Pass.hpp"

#include "Assert.hpp"
#include "RenderGraph/Sink.hpp"
#include "RenderGraph/Source.hpp"

namespace vee::rdg {

Pass::~Pass() = default;

Pass& Pass::link_sink(SinkRef sink, SourceHandle source) {
    VASSERT(sources_.contains(source), "Cannot link a sink to a source not contained by this Pass!");
    sources_.at(source)->sink_ref = sink;
    return *this;
}
Source* Pass::find_source(SourceHandle handle) const {
    auto source_entry = sources_.find(handle);
    if (source_entry == sources_.end()) {
        log_error("Source {} does not exist", handle.to_string());
        return nullptr;
    }
    return source_entry->second.get();
}

Sink* Pass::find_sink(SinkHandle handle) const {
    auto sink_entry = sinks_.find(handle);
    if (sink_entry == sinks_.end()) {
        log_error("Sink {} does not exist", handle.to_string());
        return nullptr;
    }
    return sink_entry->second.get();
}

void Pass::register_source(SourceHandle name, std::unique_ptr<Source> source) {
    VASSERT(!sources_.contains(name), "Cannot register more than one source with the same name!");
    sources_.insert({name, std::move(source)});
}
void Pass::register_sink(SinkHandle name, std::unique_ptr<Sink> sink) {
    VASSERT(!sinks_.contains(name), "Cannot register more than one sink with the same name!");
    sinks_.insert({name, std::move(sink)});
}

} // namespace vee::rdg