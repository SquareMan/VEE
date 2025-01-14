//
// Created by Square on 11/18/2024.
//

#pragma once
#include <memory>

import Vee.Renderer;

namespace vee {

class Sprite {
public:
    std::shared_ptr<vee::Image> image_;
};

} // namespace vee
