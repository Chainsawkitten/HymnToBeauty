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
     * @param vulkan Whether to generate SPIR-V source or leave it as an empty string.
     *
     * @return Whether the source could be written correctly.
     */
    bool WriteSource(const std::string& filename, const std::string& headerName, bool vulkan = false);

  private:
    static std::string VariableName(std::string inputName);
    static std::string GetSource(const std::string& filename);
    std::string GetGlslSource() const;
    std::vector<char> GetSpirvSource(bool vulkan) const;
    ShaderSource::ReflectionInfo GetReflectionInfo() const;
    
    static std::string GetVersionString();
    static std::string GetDefaultGlslInclude();
    static std::string GetDefaultSpirvInclude();

    static ShaderSource::ReflectionInfo::PushConstant::Type StringToPushConstantType(const std::string& text);
    static std::string PushConstantTypeToString(ShaderSource::ReflectionInfo::PushConstant::Type type);

    std::string source = "";
    std::string extension = "";
    std::string variableName = "";
};
