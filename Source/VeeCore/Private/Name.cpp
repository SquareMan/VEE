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


#include "Name.hpp"

#include "Assert.hpp"
#include "Logging.hpp"

#include <mutex>
#include <string>
#include <unordered_map>


struct Storage {
    std::mutex data_mutex;
    std::unordered_map<size_t, std::string> data;
};

/**
 * We need to lazily construct the global name storage like this so that we can safely allow for
 * static Names
 */
static Storage& get_name_storage() {
    static Storage singleton;
    return singleton;
}

namespace vee {

Name::Name()
    : Name(""_hash) {}
Name::Name(StrHash str_hash)
    : hash(str_hash.hash) {
    auto& storage = get_name_storage();
    std::lock_guard lock(storage.data_mutex);
    const auto entry = storage.data.find(hash);
    if (entry == storage.data.end()) {
        log_trace("Storing {} in global Name table", str_hash.str);
        storage.data[hash] = str_hash.str;
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
    auto& storage = get_name_storage();
    std::lock_guard lock(storage.data_mutex);
    const auto entry = storage.data.find(hash);
    if (entry != storage.data.end()) {
        return entry->second;
    }

    VASSERT(false, "No string for hash 0x{:X} exists in the global Name table!", hash);
    return {};
}
} // namespace vee