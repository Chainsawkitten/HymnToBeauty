#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <ShaderProcessor/ShaderSource.hpp>

/// A shader.
class Shader {
  public:
    /// Open a GLSL file and read contents.
    /**
     * @param filename The name of the file to open.
     *
     * @return Whether the file could be parsed correctly.
     */
    bool Open(const std::string& filename);

    /// Write header file.
    /**
     * @param filename Name of the file to write to.
     *
     * @return Whether the header could be written correctly.
     */
    bool WriteHeader(const std::string& filename);

    /// Write source file.
    /**
     * @param filename Name of the file to write to.
     * @param headerName The name of the header file.
     * @param vulkan Whether to generate SPIR-V source for Vulkan or leave it as an empty string.
     * @param webgpu Whether to generate SPIR-V source for WebGPU or leave it as an empty string.
     *
     * @return Whether the source could be written correctly.
     */
    bool WriteSource(const std::string& filename, const std::string& headerName, bool vulkan = false, bool webgpu = false);

  private:
    static std::string VariableName(std::string inputName);
    static std::string GetSource(const std::string& filename);
    std::string GetGlslSource() const;
    std::vector<uint32_t> GetVulkanSpirvSource(const std::string& filename, bool skip) const;
    std::vector<uint32_t> GetWebGPUSpirvSource(const std::string& filename, bool skip, const ShaderSource::ReflectionInfo& reflectionInfo) const;
    std::vector<uint32_t> GetSpirvSource(const std::string& filename, const std::string& glsl) const;
    ShaderSource::ReflectionInfo GetReflectionInfo() const;
    std::string GetWebGPUGlsl(const ShaderSource::ReflectionInfo& reflectionInfo) const;
    static void WriteSpirv(std::ofstream& file, const std::vector<uint32_t>& spirv);
    
    static std::string GetVersionString();
    static std::string GetDefaultGlslInclude();
    static std::string GetDefaultVulkanInclude();
    static std::string GetDefaultWebGPUInclude();

    static ShaderSource::ReflectionInfo::PushConstant::Type StringToPushConstantType(const std::string& text);
    static std::string PushConstantTypeToString(ShaderSource::ReflectionInfo::PushConstant::Type type);

    std::string source = "";
    std::string extension = "";
    std::string variableName = "";
};
