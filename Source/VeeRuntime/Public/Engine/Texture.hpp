//
// Created by Square on 5/4/2025.
//

#pragma once
#include <memory>
#include <vulkan/vulkan.hpp>

namespace vee {
class Image;
}
namespace vee {
class Texture {
public:
    struct CreateError {};
    static std::expected<std::shared_ptr<Texture>, CreateError> create(const char* path, vk::Format format = vk::Format::eB8G8R8A8Srgb);
protected:
    Texture() = default;
    std::shared_ptr<Image> image_;

    friend class Material;
};
}
