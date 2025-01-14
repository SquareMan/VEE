//
// Created by Square on 11/20/2024.
//

#pragma once
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

    template <typename ServiceClass = ServiceType, typename... Args>
    static void InitService(Args&&... args) {
        assert(!entt::locator<ServiceType>::has_value() && "Service already initialized");

        if constexpr (Visibility == ConstructorVisibility::Private) {
            entt::locator<ServiceType>::reset(new ServiceClass(ConstructionToken(), args...));
        } else {
            entt::locator<ServiceType>::reset(new ServiceClass(args...));
        }
    }

    static auto GetService() -> ServiceType& {
        return entt::locator<ServiceType>::value();
    }
};
} // namespace vee