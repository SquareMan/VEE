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

#include "FNV-1a.hpp"

#include <optional>
#include <string_view>


namespace vee {

/**
 * Intermediate step for Name construction. This allows for the string hashing step to be performed
 * at compile-time, when possible.
 */
struct StrHash {
    size_t hash;
    const char* str;

    explicit constexpr StrHash(const char* str)
        : hash(utils::fnv1a_from_cstr(str))
        , str(str) {}
};

constexpr StrHash operator""_hash(const char* str, std::size_t len) {
    return StrHash{str};
}

/**
 * A Name is a lightweight string handle that facilitates efficient passing-by-value and O(1)
 * equality comparison. Names uphold the property that for any given Names A and B, A == B if and
 * only if the underlying string represented by A and B are equivalent (i.e. strcmp would return 0)
 */
class Name {
public:
    /**
     * Construct an empty Name. The empty Name represents the string "" (empty string).
     */
    Name();
    /**
     * Construct a new Name. The underlying string will be added to the global Name table if it has
     * not yet been added by a previous Name.
     * @param str_hash A string along with its pre-computed hash.
     */
    Name(StrHash str_hash); // NOLINT(*-explicit-constructor)

    Name(const Name&) = default;
    Name& operator=(const Name&) = default;
    Name(Name&&) = default;
    Name& operator=(Name&&) = default;


    /**
     * Tests if two Names are equal. Names are equal if and only if both were created from
     * equivalent strings.
     * @param other Name to compare
     */
    bool operator==(const Name& other) const {
        return hash == other.hash;
    }


    /**
     * Tests if two Names are different.
     * @see operator==()
     * @param other Name to compare
     */
    bool operator!=(const Name& other) const {
        return !operator==(other);
    }

    /**
     * @return the underlying string represented by this Name.
     */
    [[nodiscard]] std::string_view to_string() const;

private:
    std::size_t hash;
    friend std::hash<Name>;
};
} // namespace vee

template <>
struct std::hash<vee::StrHash> {
    std::size_t operator()(const vee::StrHash& str_hash) const noexcept {
        return str_hash.hash;
    }
};

template <>
struct std::hash<vee::Name> {
    std::size_t operator()(const vee::Name& handle) const noexcept {
        return handle.hash;
    }
};
