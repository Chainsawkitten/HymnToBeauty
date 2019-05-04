#include "VulkanVertexDescription.hpp"

#include <Utility/Log.hpp>
#include <assert.h>

namespace Video {

VulkanVertexDescription::VulkanVertexDescription(unsigned int attributeCount, const VertexDescription::Attribute* attributes) {
    assert(attributeCount > 0);
    assert(attributes != nullptr);

    // Binding description.
    bindingDescription.binding = 0;
    bindingDescription.stride = 0;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Attribute descriptions.
    attributeDescriptionCount = attributeCount;
    attributeDescriptions = new VkVertexInputAttributeDescription[attributeCount];

    for (unsigned int i = 0; i < attributeCount; ++i) {
        attributeDescriptions[i].location = i;
        attributeDescriptions[i].binding = 0;
        attributeDescriptions[i].offset = bindingDescription.stride;

        uint32_t size = 0;
        switch (attributes[i].type) {
        case AttributeType::UNSIGNED_BYTE:
            size = sizeof(uint8_t) * attributes[i].size;
            switch (attributes[i].size) {
            case 1:
                attributeDescriptions[i].format = (attributes[i].normalized ? VK_FORMAT_R8_UNORM : VK_FORMAT_R8_UINT);
                break;
            case 2:
                attributeDescriptions[i].format = (attributes[i].normalized ? VK_FORMAT_R8G8_UNORM : VK_FORMAT_R8G8_UINT);
                break;
            case 3:
                attributeDescriptions[i].format = (attributes[i].normalized ? VK_FORMAT_R8G8B8_UNORM : VK_FORMAT_R8G8B8_UINT);
                break;
            case 4:
                attributeDescriptions[i].format = (attributes[i].normalized ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_UINT);
                break;
            default:
                assert(0);
            }
            break;
        case AttributeType::INT:
            assert(!attributes[i].normalized);
            size = sizeof(int32_t) * attributes[i].size;
            switch (attributes[i].size) {
            case 1:
                attributeDescriptions[i].format = VK_FORMAT_R32_UINT;
                break;
            case 2:
                attributeDescriptions[i].format = VK_FORMAT_R32G32_UINT;
                break;
            case 3:
                attributeDescriptions[i].format = VK_FORMAT_R32G32B32_UINT;
                break;
            case 4:
                attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_UINT;
                break;
            default:
                assert(0);
            }
            break;
        case AttributeType::FLOAT:
            assert(!attributes[i].normalized);
            size = sizeof(float) * attributes[i].size;
            switch (attributes[i].size) {
            case 1:
                attributeDescriptions[i].format = VK_FORMAT_R32_SFLOAT;
                break;
            case 2:
                attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
                break;
            case 3:
                attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case 4:
                attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
                break;
            default:
                assert(0);
            }
            break;
        default:
            assert(0);
        }

        bindingDescription.stride += size;
    }
}

VulkanVertexDescription::~VulkanVertexDescription() {
    delete[] attributeDescriptions;
}

const VkVertexInputBindingDescription* VulkanVertexDescription::GetBindingDescription() const {
    return &bindingDescription;
}

unsigned int VulkanVertexDescription::GetAttributeCount() const {
    return attributeDescriptionCount;
}

const VkVertexInputAttributeDescription* VulkanVertexDescription::GetAttributeDescriptions() const {
    return attributeDescriptions;
}

} // namespace Video
