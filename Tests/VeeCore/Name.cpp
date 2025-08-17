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


#include <catch2/catch_test_macros.hpp>

#include <Name.hpp>

TEST_CASE("Default Name is the Empty String") {
    using namespace vee;

    Name empty;
    Name explicit_empty(""_hash);

    REQUIRE(empty == explicit_empty);
}

TEST_CASE("String can be retrieved") {
    using namespace vee;

    Name my_name("TestAsset"_hash);
    REQUIRE(my_name.to_string() == "TestAsset");
}

SCENARIO("You need to create a Name") {
    using namespace vee;

    GIVEN("A string known at compile time") {
        THEN("You can create a string") {
            Name name("Known String"_hash);
            REQUIRE(name.to_string() == "Known String");
        }
    }
    GIVEN("An existing Name") {
        Name existing("old_name"_hash);

        THEN("It can be copied") {
            Name new_name = existing;
            REQUIRE(new_name == existing);
            REQUIRE(new_name.to_string() == "old_name");
        }
        THEN("It can be moved") {
            Name new_name = std::move(existing);
            REQUIRE(new_name.to_string() == "old_name");
        }
    }
}

SCENARIO("Names can be compared") {
    using namespace vee;

    GIVEN("Two Names that are the same") {
        Name first("TestAsset"_hash);
        Name second("TestAsset"_hash);

        WHEN("They are compared") {
            THEN("They are equal") {
                REQUIRE(first == second);
            }
            THEN("They are not not equal") {
                REQUIRE_FALSE(first != second);
            }
        }
    }
    GIVEN("Two Names that are not the same") {
        Name first("TestAsset"_hash);
        Name second("SeparateAsset"_hash);

        WHEN("They are compared") {
            THEN("They are not equal") {
                REQUIRE(first != second);
            }
            THEN("They are not equal") {
                REQUIRE(!(first == second));
            }
        }
    }
}