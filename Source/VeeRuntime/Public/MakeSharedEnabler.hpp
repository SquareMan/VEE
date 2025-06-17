//
// Created by Square on 6/16/2025.
//

#pragma once
#include <utility>

template <typename Base>
struct MakeSharedEnabler : Base {
    template <typename... Args>
    explicit MakeSharedEnabler(Args&&... args)
        : Base(std::forward<Args>(args)...) {}
};
