//
// Created by Square on 6/14/2025.
//

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
        log_error("Source {} does not exist", handle.hash);
        return nullptr;
    }
    return source_entry->second.get();
}

Sink* Pass::find_sink(SinkHandle handle) const {
    auto sink_entry = sinks_.find(handle);
    if (sink_entry == sinks_.end()) {
        log_error("Sink {} does not exist", handle.hash);
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