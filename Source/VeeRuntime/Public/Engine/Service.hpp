//
// Created by Square on 11/20/2024.
//

#pragma once

#include "Assert.hpp"
#include <entt/locator/locator.hpp>

namespace vee {

enum class ConstructorVisibility { Public, Private };
template <typename Type, ConstructorVisibility Visibility = ConstructorVisibility::Private>
class Service {
protected:
    struct ConstructionToken {
        explicit ConstructionToken() = default;
    };

public:
    using ServiceType = Type;

    template <typename... Args>
    static void InitService(Args&&... args) {
        VASSERT(!entt::locator<ServiceType>::has_value(), "Service already initialized");

        if constexpr (Visibility == ConstructorVisibility::Private) {
            entt::locator<ServiceType>::emplace(ConstructionToken(), std::forward<Args>(args)...);
        } else {
            entt::locator<ServiceType>::emplace(std::forward<Args>(args)...);
        }
    }

    static void ShutdownService() {
        entt::locator<ServiceType>::reset();
    }

    static auto GetService() -> ServiceType& {
        return entt::locator<ServiceType>::value();
    }
};
} // namespace vee