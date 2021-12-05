#pragma once

#include "../Interface/LowLevelRenderer.hpp"

#include <vulkan/vulkan.h>
#include <map>
#include <vector>
#include "../Interface/ShaderProgram.hpp"
#include "VulkanCommandBuffer.hpp"

struct GLFWwindow;

namespace Video {

class VulkanBuffer;
class VulkanBufferAllocator;

/// Low-level renderer implementing Vulkan.
class VulkanRenderer : public LowLevelRenderer {
  public:
    /// Create new Vulkan renderer.
    /**
     * @param window The window to render to.
     */
    explicit VulkanRenderer(GLFWwindow* window);

    /// Destructor.
    ~VulkanRenderer() final;

    CommandBuffer* CreateCommandBuffer() final;
    void BeginFrame() final;
    void Submit(CommandBuffer* commandBuffer) final;
    void Present() final;
    Buffer* CreateBuffer(Buffer::BufferUsage bufferUsage, uint32_t size, const void* data = nullptr) final;
    Buffer* CreateTemporaryBuffer(Buffer::BufferUsage bufferUsage, uint32_t size, const void* data = nullptr) final;
    VertexDescription* CreateVertexDescription(unsigned int attributeCount, const VertexDescription::Attribute* attributes, bool indexBuffer = false) final;
    GeometryBinding* CreateGeometryBinding(const VertexDescription* vertexDescription, Buffer* vertexBuffer, GeometryBinding::IndexType indexType = GeometryBinding::IndexType::NONE, const Buffer* indexBuffer = nullptr) final;
    Shader* CreateShader(const ShaderSource& shaderSource, Shader::Type type) final;
    ShaderProgram* CreateShaderProgram(std::initializer_list<const Shader*> shaders) final;
    Texture* CreateTexture(const glm::uvec2 size, Texture::Type type, Texture::Format format, int components = 0, unsigned char* data = nullptr) final;
    RenderPass* CreateRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation) final;
    RenderPass* CreateAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples) final;
    GraphicsPipeline* CreateGraphicsPipeline(const ShaderProgram* shaderProgram, const GraphicsPipeline::Configuration& configuration, const VertexDescription* vertexDescription = nullptr) final;
    ComputePipeline* CreateComputePipeline(const ShaderProgram* shaderProgram) final;
    void Wait() final;
    unsigned char* ReadImage(RenderPass* renderPass) final;
    const std::vector<Profiling::Event>& GetTimeline() const final;
    const OptionalFeatures& GetOptionalFeatures() const final;

    /// Get current swap chain image.
    /**
     * @return The current swap chain image.
     */
    VkImage GetCurrentSwapChainImage() const;

    /// Get the number of swap chain images.
    /**
     * @return The number of swap chain images.
     */
    uint32_t GetSwapChainImageCount() const;

    /// Get the empty descriptor set layout.
    VkDescriptorSetLayout GetEmptyDescriptorSetLayout() const;

    /// Get the descriptor set layout for a buffer binding.
    /**
     * @param bindingType The binding type.
     *
     * @return The descriptor set layout.
     */
    VkDescriptorSetLayout GetBufferDescriptorSetLayout(ShaderProgram::BindingType bindingType) const;

    /// Get the descriptor set layout for a storage buffer binding.
    /**
     * @param buffers The number of buffers to bind.
     *
     * @return The descriptor set layout.
     */
    VkDescriptorSetLayout GetStorageBufferDescriptorSetLayout(unsigned int buffers) const;

    /// Get the descriptor set layout for a material binding.
    /**
     * @param textures The number of textures to bind.
     *
     * @return The descriptor set layout.
     */
    VkDescriptorSetLayout GetMaterialDescriptorSetLayout(unsigned int textures) const;

    /// Get a descriptor set for a given binding type and buffer.
    /**
     * @param bindingType The binding type to bind the buffer to.
     * @param buffer The buffer to bind.
     *
     * @return The descriptor set.
     */
    VkDescriptorSet GetDescriptorSet(ShaderProgram::BindingType bindingType, VulkanBuffer* buffer);

    /// Get a descriptor set for a given binding of storage buffers.
    /**
     * @param buffers The buffers to bind.
     *
     * @return The descriptor set.
     */
    VkDescriptorSet GetStorageBufferDescriptorSet(std::initializer_list<Buffer*> buffers);

    /// Get a descriptor set for a given binding of textures.
    /**
     * @param textures The textures to bind.
     *
     * @return The descriptor set.
     */
    VkDescriptorSet GetDescriptorSet(std::initializer_list<Texture*> textures);

    /// Get the current frame's query pool.
    /**
     * @return The current frame's query pool.
     */
    VkQueryPool GetQueryPool();

    /// Get a free query from the current frame's pool.
    /**
     * @return The query.
     */
    uint32_t GetFreeQuery();

  private:
    VulkanRenderer(const VulkanRenderer& other) = delete;

    void CreateInstance();
    bool CheckValidationLayersSupported(const std::vector<const char*>& validationLayers) const;

    void CreateSurface(GLFWwindow* window);

    struct SwapChainSupport {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    void CreateDevice(SwapChainSupport& swapChainSupport);
    VkPhysicalDevice PickPhysicalDevice(const std::vector<const char*>& extensions) const;
    uint32_t GetDeviceScore(VkPhysicalDevice device, const std::vector<const char*>& extensions) const;
    bool CheckDeviceExtensionsSupported(VkPhysicalDevice device, const std::vector<const char*>& extensions) const;
    uint32_t GetQueueFamily(VkPhysicalDevice device) const;
    SwapChainSupport GetSwapChainSupport(VkPhysicalDevice device) const;
    const std::vector<const char*> GetEnabledExtensions(const std::vector<const char*>& mandatoryExtensions);
    VkPhysicalDeviceFeatures GetEnabledFeatures();

    void CreateSwapChain(const SwapChainSupport& swapChainSupport);
    void FreeSwapChain();
    void RecreateSwapChain();
    VkSurfaceFormatKHR ChooseSurfaceFormat(const SwapChainSupport& swapChainSupport) const;
    VkPresentModeKHR ChoosePresentMode(const SwapChainSupport& swapChainSupport) const;
    VkExtent2D ChooseExtent(const SwapChainSupport& swapChainSupport) const;

    void CreateImageViews();

    void CreateSyncObjects();

    void CreateCommandPool();

    void CreateBakedDescriptorSetLayouts();
    void CreateDescriptorPool();

    void CreateQueries();
    void ResetQueries(uint32_t queryPool);

    void AcquireSwapChainImage();

    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;

    uint32_t graphicsQueueFamily;
    VkQueue queue;

    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    uint32_t imageIndex;
    uint32_t currentFrame = 0;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> renderFinishedFences;
    VkFence recreateFence;

    VkCommandPool commandPool;

    VulkanBufferAllocator* bufferAllocator;
    uint32_t nonCoherentAtomSize;

    VkDescriptorSetLayout emptyDescriptorSetLayout;
    VkDescriptorSetLayout bufferDescriptorSetLayouts[ShaderProgram::BindingType::BINDING_TYPES];
    std::vector<std::map<ShaderProgram::BindingType, std::vector<VkDescriptorSet>>> bufferDescriptorSetCache;
    unsigned int currentBufferDescriptorSet[ShaderProgram::BindingType::BINDING_TYPES];

    static const unsigned int bakedStorageBufferDescriptorSetLayouts = 16;
    VkDescriptorSetLayout storageBufferDescriptorSetLayouts[bakedStorageBufferDescriptorSetLayouts];
    std::vector<std::map<VkDescriptorSetLayout, std::vector<VkDescriptorSet>>> storageBufferDescriptorSetCache;
    unsigned int currentStorageBufferDescriptorSet[bakedStorageBufferDescriptorSetLayouts];

    static const unsigned int bakedMaterialDescriptorSetLayouts = 16;
    VkDescriptorSetLayout materialDescriptorSetLayouts[bakedMaterialDescriptorSetLayouts];
    std::vector<std::map<VkDescriptorSetLayout, std::vector<VkDescriptorSet>>> materialDescriptorSetCache;
    unsigned int currentMaterialDescriptorSet[bakedMaterialDescriptorSetLayouts];

    VkDescriptorPool descriptorPool;

    OptionalFeatures optionalFeatures;
    
    double timestampPeriod;
    static const unsigned int maxQueries = 2 * 50;
    std::vector<VkQueryPool> queryPools;
    std::vector<Profiling::Event> finishedEvents;
    std::vector<uint32_t> currentQuery;
    VkCommandBuffer* queryCommandBuffers;
    VkSemaphore* queryResetSemaphores;
    bool firstSubmission;
    bool* needQueryWait;
    std::vector<VulkanCommandBuffer::Timing>* submittedTimings;
    uint64_t results[maxQueries];
    double* submissionTimes;
};

} // namespace Video
