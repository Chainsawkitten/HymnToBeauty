#include "Shader.hpp"

#include <iostream>
#include <sstream>
#include <cstdio>
#include <assert.h>

using namespace std;

bool Shader::Open(const string& filename) {
    // Read file.
    ifstream inFile(filename.c_str());
    stringstream buffer;
    buffer << inFile.rdbuf();

    source = buffer.str();

    // Get variable name and extension.
    variableName = VariableName(filename);
    extension = filename.substr(filename.find_last_of('.'));

    /// @todo Handle includes.

    return true;
}

bool Shader::WriteHeader(const string& filename) {
    ofstream outFile(filename.c_str(), ios::trunc);

    if (outFile.is_open()) {
        outFile << "#pragma once " << endl << endl;

        outFile << "#include <ShaderProcessor/ShaderSource.hpp>" << endl << endl;

        outFile << "extern const ShaderSource " << variableName << ";" << endl;
        
        outFile.close();
    } else {
        cout << "Couldn't open " << filename << " for writing." << endl;
        return false;
    }

    return true;
}

bool Shader::WriteSource(const string& filename, const string& headerName, bool vulkan) {
    // Tag for raw string literals, to make it unlikely (but not impossible) for shader to break.
    string tag = "__shader__";

    ofstream outFile(filename.c_str(), ios::trunc);

    if (outFile.is_open()) {
        std::size_t found = headerName.find_last_of("/\\");
        outFile << "#include \"" << headerName.substr(found+1) << "\"" << endl << endl;

        // GLSL source.
        outFile << "static const char glsl[] = {";
        string glslSource = GetGlslSource();
        for (size_t i = 0; i < glslSource.length(); i++) {
            outFile << +glslSource[i];
            if (i + 1 < glslSource.length()) {
                outFile << ",";
            }
        }
        outFile << "};" << endl;

        // SPIR-V source.
        vector<char> spirvSource = GetSpirvSource(vulkan);
        if (vulkan) {
            outFile << "static const uint32_t spirv[] = {";
            assert(spirvSource.size() % sizeof(uint32_t) == 0);
            size_t spirvUintLength = spirvSource.size() / sizeof(uint32_t);
            const uint32_t* spirvUintSource = reinterpret_cast<const uint32_t*>(spirvSource.data());
            for (size_t i = 0; i < spirvUintLength; i++) {
                outFile << spirvUintSource[i];
                if (i + 1 < spirvUintLength) {
                    outFile << ",";
                }
            }
            outFile << "};" << endl << endl;
        }

        // Reflection info.
        ShaderSource::ReflectionInfo reflectionInfo = GetReflectionInfo();

        // Push constants.
        if (reflectionInfo.pushConstantCount > 0) {
            outFile << "static ShaderSource::ReflectionInfo::PushConstant pushConstants[" << reflectionInfo.pushConstantCount << "] = {\n";

            for (unsigned int i = 0; i < reflectionInfo.pushConstantCount; ++i) {
                outFile << "    { " << PushConstantTypeToString(reflectionInfo.pushConstants[i].type) << ", \"" << reflectionInfo.pushConstants[i].name << "\" },\n";
            }

            outFile << "};\n";

            delete[] reflectionInfo.pushConstants;
        }

        // ShaderSource struct.
        outFile << "const ShaderSource " << variableName << " = { ";

        outFile << glslSource.length() << ", glsl, ";
        if (vulkan) {
            outFile << spirvSource.size() << ", spirv, ";
        } else {
            outFile << "0, nullptr, ";
        }
        outFile << (reflectionInfo.hasMatrices ? "true" : "false") << ", ";
        outFile << (reflectionInfo.hasUniforms ? "true" : "false") << ", ";
        outFile << (reflectionInfo.hasStorageBuffer ? "true" : "false") << ", ";
        outFile << (reflectionInfo.storageBufferReadWrite ? "true" : "false") << ", ";
        outFile << reflectionInfo.materialCount << ", ";
        outFile << reflectionInfo.pushConstantCount << ", ";
        outFile << (reflectionInfo.pushConstantCount > 0 ? "pushConstants" : "nullptr") << ", ";
        outFile << "\"" << reflectionInfo.pushConstantBlockName << "\"";
        outFile << " }; " << endl;
        
        outFile.close();
    } else {
        cout << "Couldn't open " << filename << " for writing." << endl;
        return false;
    }

    return true;
}

string Shader::VariableName(string inputName) {
    // Get base filename.
    size_t found = inputName.find_last_of("/\\");
    inputName = inputName.substr(found+1);
    
    // Convert to upper case and replace . with _.
    for (string::size_type i=0; i<inputName.length(); i++) {
        inputName[i] = toupper(inputName[i]);
        if (inputName[i] == '.')
            inputName[i] = '_';
    }
    
    return inputName;
}

string Shader::GetGlslSource() const {
    /// @todo Variants

    string glsl = GetVersionString();
    glsl += GetDefaultGlslInclude();
    glsl += source;

    return glsl;
}

vector<char> Shader::GetSpirvSource(bool vulkan) const {
    // Early out if we shouldn't get SPIR-V.
    if (!vulkan) {
        vector<char> spirv;
        return spirv;
    }

    /// @todo Variants

    string glsl = GetVersionString();
    glsl += GetDefaultSpirvInclude();
    glsl += source;

    // Write temporary glsl file as input to glslangvalidator.
    string tempGlslFilename = "temp_for_glslang" + extension;
    ofstream tempFile(tempGlslFilename);
    tempFile << glsl;
    tempFile.close();

    // Run glslangvalidator to generate SPIR-V.
    const char* tempSpirvFilename = "temp_shader_output.spv";
    system((string("glslangvalidator -V --target-env vulkan1.0 -o ") + tempSpirvFilename + " " + tempGlslFilename).c_str());

    // Read back SPIR-V.
    ifstream file(tempSpirvFilename, ios::binary | ios::ate);

    size_t fileSize = (size_t) file.tellg();
    vector<char> spirv(fileSize);
    file.seekg(0);
    file.read(spirv.data(), fileSize);
    file.close();

    // Clean up temp files.
    remove(tempGlslFilename.c_str());
    remove(tempSpirvFilename);

    return spirv;
}

ShaderSource::ReflectionInfo Shader::GetReflectionInfo() const {
    /// @todo More proper lexing.

    // Remove multi-line comments.
    string parseSource = source;
    size_t commentStart = parseSource.find("/*");
    while (commentStart != string::npos) {
        size_t commentEnd = parseSource.find("*/", commentStart + 2);
        if (commentEnd == string::npos)
            break;

        parseSource = parseSource.erase(commentStart, commentEnd - commentStart);
        commentStart = parseSource.find("/*");
    }

    // Parse for resource definitions.
    ShaderSource::ReflectionInfo reflectionInfo = {};
    string line;
    stringstream lineParser(parseSource);
    bool inPushConstantBlock = false;
    vector<ShaderSource::ReflectionInfo::PushConstant> pushConstants;
    while (getline(lineParser, line)) {
        // Remove leading whitespace.
        size_t firstNonWhitespace = line.find_first_not_of(" \t");
        if (firstNonWhitespace != string::npos) {
            line = line.substr(firstNonWhitespace);
        }

        if (inPushConstantBlock) {
            // Parse push constants.
            if (line.find("}") == 0) {
                inPushConstantBlock = false;
                size_t startPos = line.find_first_not_of(" \t", 1);
                size_t endPos = line.find(";", startPos + 1);
                if (startPos == string::npos || endPos == string::npos) {
                    cerr << "Could not find name of push constant block.\n";
                }
                reflectionInfo.pushConstantBlockName = line.substr(startPos, endPos - startPos);
            } else {
                size_t typeEndPos = line.find_first_of(" \t");
                size_t nameStartPos = line.find_first_not_of(" \t", typeEndPos);
                size_t nameEndPos = line.find_first_of(" \t;", nameStartPos);
                if (typeEndPos != string::npos && nameStartPos != string::npos && nameEndPos != string::npos) {
                    ShaderSource::ReflectionInfo::PushConstant pushConstant;
                    pushConstant.type = StringToPushConstantType(line.substr(0, typeEndPos));
                    pushConstant.name = line.substr(nameStartPos, nameEndPos - nameStartPos);
                    pushConstants.push_back(pushConstant);
                }
            }
        } else {
            // Parse definitions.
            if (line.find("MATRICES") == 0) {
                reflectionInfo.hasMatrices = true;
            }

            if (line.find("UNIFORMS") == 0) {
                reflectionInfo.hasUniforms = true;
            }

            if (line.find("STORAGE_BUFFER_RW") == 0) {
                reflectionInfo.hasStorageBuffer = true;
                reflectionInfo.storageBufferReadWrite = true;
            } else if (line.find("STORAGE_BUFFER") == 0) {
                reflectionInfo.hasStorageBuffer = true;
                reflectionInfo.storageBufferReadWrite = false;
            }

            if (line.find("MATERIAL") == 0) {
                reflectionInfo.materialCount++;
            }

            if (line.find("PUSH_CONSTANTS") == 0) {
                inPushConstantBlock = true;
            }
        }
    }

    reflectionInfo.pushConstantCount = pushConstants.size();
    reflectionInfo.pushConstants = new ShaderSource::ReflectionInfo::PushConstant[pushConstants.size()];
    for (size_t i = 0; i < pushConstants.size(); ++i) {
        reflectionInfo.pushConstants[i] = pushConstants[i];
    }

    return reflectionInfo;
}

string Shader::GetVersionString() {
    return "#version 450\n\n";
}

string Shader::GetDefaultGlslInclude() {
    string glsl = "#define VertexIndex gl_VertexID\n";
    glsl += "#define InstanceIndex gl_InstanceID\n";
    glsl += "#define MATRICES layout(std140, binding = 0) uniform MatrixBlock\n";
    glsl += "#define UNIFORMS layout(std140, binding = 1) uniform FragmentUniformBlock\n";
    glsl += "#define STORAGE_BUFFER layout(std430, binding = 0) readonly buffer StorageBufferBlock\n";
    glsl += "#define STORAGE_BUFFER_RW layout(std430, binding = 0) buffer StorageBufferBlock\n";
    glsl += "#define PUSH_CONSTANTS uniform struct PushConstants\n";
    glsl += "#define FixPosition() \n";
    glsl += "#define FixFramebufferCoordinates(TEX_COORDS) TEX_COORDS.y = 1.0 - TEX_COORDS.y;\n";
    glsl += "#define MATERIAL(INDEX,NAME) layout(binding = INDEX) uniform sampler2D NAME;\n";

    return glsl;
}

string Shader::GetDefaultSpirvInclude() {
    string glsl = "#define VertexIndex gl_VertexIndex\n";
    glsl += "#define InstanceIndex gl_InstanceIndex\n";
    glsl += "#define MATRICES layout(std140, set = 0, binding = 0) uniform MatrixBlock\n";
    glsl += "#define UNIFORMS layout(std140, set = 2, binding = 0) uniform FragmentUniformBlock\n";
    glsl += "#define STORAGE_BUFFER layout(std430, set = 3, binding = 0) readonly buffer StorageBufferBlock\n";
    glsl += "#define STORAGE_BUFFER_RW layout(std430, set = 3, binding = 0) buffer StorageBufferBlock\n";
    glsl += "#define PUSH_CONSTANTS layout(push_constant) uniform PushConstants\n";
    glsl += "#define FixPosition() gl_Position.y = -gl_Position.y; gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;\n";
    glsl += "#define FixFramebufferCoordinates(TEX_COORDS)\n";
    glsl += "#define MATERIAL(INDEX,NAME) layout(set = 1, binding = INDEX) uniform sampler2D NAME;\n";

    return glsl;
}

ShaderSource::ReflectionInfo::PushConstant::Type Shader::StringToPushConstantType(const string& text) {
    if (text == "vec3") {
        return ShaderSource::ReflectionInfo::PushConstant::Type::VEC3;
    } else if (text == "vec4") {
        return ShaderSource::ReflectionInfo::PushConstant::Type::VEC4;
    } else if (text == "mat4") {
        return ShaderSource::ReflectionInfo::PushConstant::Type::MAT4;
    }

    std::cerr << "Failed to parse push constant size.\n";
    return ShaderSource::ReflectionInfo::PushConstant::Type::VEC3;
}

string Shader::PushConstantTypeToString(ShaderSource::ReflectionInfo::PushConstant::Type type) {
    string text = "ShaderSource::ReflectionInfo::PushConstant::Type::";

    switch (type) {
    case ShaderSource::ReflectionInfo::PushConstant::Type::VEC3:
        text += "VEC3";
        break;
    case ShaderSource::ReflectionInfo::PushConstant::Type::VEC4:
        text += "VEC4";
        break;
    case ShaderSource::ReflectionInfo::PushConstant::Type::MAT4:
        text += "MAT4";
        break;
    }

    return text;
}
