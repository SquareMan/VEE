//
// Created by Square on 7/5/2025.
//

#include "Name.hpp"

// FIXME: Move these to VeeCore
#include "../../../Source/VeeRuntime/Public/Assert.hpp"
#include "../../../Source/VeeRuntime/Public/Logging.hpp"

#include <string>
#include <unordered_map>


static std::unordered_map<size_t, std::string> g_name_storage;

namespace vee {

Name::Name()
    : Name(""_hash) {}
Name::Name(StrHash str_hash)
    : hash(str_hash.hash) {
    const auto entry = g_name_storage.find(hash);
    if (entry == g_name_storage.end()) {
        log_trace("Storing {} in global Name table", str_hash.str);
        g_name_storage[hash] = str_hash.str;
        return;
    }
    VASSERT(
        entry->second == str_hash.str,
        "A hash-collision has occurred in the global Name table! Existing Name: {}; New Name: {}; Hash: 0x{:X}",
        entry->second,
        str_hash.str,
        hash
    );
}
std::string_view Name::to_string() const {
    const auto entry = g_name_storage.find(hash);
    if (entry != g_name_storage.end()) {
        return entry->second;
    }

    VASSERT(false, "No string for hash 0x{:X} exists in the global Name table!", hash);
    return {};
}
} // namespace vee