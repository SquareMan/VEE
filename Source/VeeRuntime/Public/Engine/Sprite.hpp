//
// Created by Square on 11/18/2024.
//

#pragma once
#include "Renderer/Image.hpp"

#include <memory>

namespace vee {
class Material;

class Sprite {
public:
    std::shared_ptr<Material> material_;
};

} // namespace vee
