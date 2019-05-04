#pragma once

#include "../Interface/VertexDescription.hpp"

#include <vulkan/vulkan.h>

namespace Video {

class Buffer;

/// Vulkan implementation of VertexDescription.
class VulkanVertexDescription : public VertexDescription {
  public:
    /// Create new Vulkan vertex description.
    /**
     * @param attributeCount The number of vertex attributes.
     * @param attributes The array of attributes.
     *
     * @return The created vertex description.
     */
    VulkanVertexDescription(unsigned int attributeCount, const VertexDescription::Attribute* attributes);

    /// Destructor.
    ~VulkanVertexDescription() final;

    /// Get the vertex input binding description.
    /**
     * @return The vertex input binding description.
     */
    const VkVertexInputBindingDescription* GetBindingDescription() const;

    /// Get the number of attributes.
    /**
     * @return The number of attributes.
     */
    unsigned int GetAttributeCount() const;

    /// Get the vertex input attribute descriptions.
    /**
     * @return The vertex input attribute descriptions.
     */
    const VkVertexInputAttributeDescription* GetAttributeDescriptions() const;

  private:
      VulkanVertexDescription(const VulkanVertexDescription& other) = delete;

      VkVertexInputBindingDescription bindingDescription;

      unsigned int attributeDescriptionCount;
      VkVertexInputAttributeDescription* attributeDescriptions;
};

}
