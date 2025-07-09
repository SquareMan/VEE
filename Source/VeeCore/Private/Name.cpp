//
// Created by Square on 7/5/2025.
//

#include "Name.hpp"

#include "Logging.hpp"
#include "Assert.hpp"

#include <string>
#include <unordered_map>


/**
 * We need to lazily construct the global name storage like this so that we can safely allow for static Names
 */
static std::unordered_map<size_t, std::string>& get_name_storage(){
    static std::unordered_map<size_t, std::string> singleton;
    return singleton;
}

namespace vee {

Name::Name()
    : Name(""_hash) {}
Name::Name(StrHash str_hash)
    : hash(str_hash.hash) {

    auto& storage = get_name_storage();
    const auto entry = storage.find(hash);
    if (entry == storage.end()) {
        log_trace("Storing {} in global Name table", str_hash.str);
        storage[hash] = str_hash.str;
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
    const auto& storage = get_name_storage();
    const auto entry = storage.find(hash);
    if (entry != storage.end()) {
        return entry->second;
    }

    VASSERT(false, "No string for hash 0x{:X} exists in the global Name table!", hash);
    return {};
}
} // namespace vee