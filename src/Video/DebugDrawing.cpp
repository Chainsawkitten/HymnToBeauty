#include "DebugDrawing.hpp"

#include "LowLevelRenderer/Interface/LowLevelRenderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "LowLevelRenderer/Interface/Shader.hpp"
#include "LowLevelRenderer/Interface/ShaderProgram.hpp"
#include "DebugDrawing.vert.hpp"
#include "DebugDrawing.frag.hpp"
#include <Video/Renderer.hpp>
#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Video/LowLevelRenderer/Interface/CommandBuffer.hpp>
#include <Video/Geometry/VertexType/StaticVertex.hpp>
#include <Utility/Log.hpp>

#define BUFFER_OFFSET(i) ((char*)nullptr + (i))

using namespace Video;

DebugDrawing::DebugDrawing(Renderer* renderer) {
    this->renderer = renderer;
    lowLevelRenderer = renderer->GetLowLevelRenderer();

    vertexShader = lowLevelRenderer->CreateShader(DEBUGDRAWING_VERT, Shader::Type::VERTEX_SHADER);
    fragmentShader = lowLevelRenderer->CreateShader(DEBUGDRAWING_FRAG, Shader::Type::FRAGMENT_SHADER);
    shaderProgram = lowLevelRenderer->CreateShaderProgram({vertexShader, fragmentShader});

    // Create vertex description.
    VertexDescription::Attribute attribute;
    attribute.size = 3;
    attribute.type = VertexDescription::AttributeType::FLOAT;
    attribute.normalized = false;

    vertexDescription = lowLevelRenderer->CreateVertexDescription(1, &attribute);

    meshVertexDescription = Geometry::VertexType::StaticVertex::GenerateVertexDescription(lowLevelRenderer);

    // Create graphics pipelines.
    GraphicsPipeline::Configuration configuration = {};
    configuration.cullFace = CullFace::NONE;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthComparison = DepthComparison::LESS_EQUAL;

    for (uint8_t i = 0; i < 2; ++i) {
        configuration.depthMode = (i == 0 ? DepthMode::NONE : DepthMode::TEST_WRITE);
        configuration.polygonMode = PolygonMode::FILL;
        configuration.primitiveType = PrimitiveType::POINT;
        pointGraphicsPipeline[i] = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);

        configuration.primitiveType = PrimitiveType::LINE;
        lineGraphicsPipeline[i] = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);

        configuration.primitiveType = PrimitiveType::TRIANGLE;
        fillTriangleGraphicsPipeline[i] = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, meshVertexDescription);

        if (lowLevelRenderer->GetOptionalFeatures().fillModeNonSolid) {
            configuration.polygonMode = PolygonMode::LINE;
            lineTriangleGraphicsPipeline[i] = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, meshVertexDescription);
        }
    }

    // Create point vertex buffer.
    glm::vec3 point(0.f, 0.f, 0.f);
    CreateVertexBuffer(&point, 1, pointVertexBuffer, pointGeometryBinding);

    // Create line vertex buffer.
    glm::vec3 line[2];
    line[0] = glm::vec3(0.f, 0.f, 0.f);
    line[1] = glm::vec3(1.f, 1.f, 1.f);

    CreateVertexBuffer(line, 2, lineVertexBuffer, lineGeometryBinding);

    // Create cuboid vertex buffer.
    glm::vec3 box[24];
    box[0] = glm::vec3(-0.5f, -0.5f, -0.5f);
    box[1] = glm::vec3(0.5f, -0.5f, -0.5f);
    box[2] = glm::vec3(0.5f, -0.5f, -0.5f);
    box[3] = glm::vec3(0.5f, 0.5f, -0.5f);
    box[4] = glm::vec3(0.5f, 0.5f, -0.5f);
    box[5] = glm::vec3(-0.5f, 0.5f, -0.5f);
    box[6] = glm::vec3(0.5f, 0.5f, -0.5f);
    box[7] = glm::vec3(0.5f, 0.5f, 0.5f);
    box[8] = glm::vec3(0.5f, 0.5f, 0.5f);
    box[9] = glm::vec3(0.5f, -0.5f, 0.5f);
    box[10] = glm::vec3(0.5f, -0.5f, 0.5f);
    box[11] = glm::vec3(0.5f, -0.5f, -0.5f);
    box[12] = glm::vec3(-0.5f, 0.5f, -0.5f);
    box[13] = glm::vec3(-0.5f, 0.5f, 0.5f);
    box[14] = glm::vec3(-0.5f, 0.5f, 0.5f);
    box[15] = glm::vec3(-0.5f, -0.5f, 0.5f);
    box[16] = glm::vec3(-0.5f, 0.5f, -0.5f);
    box[17] = glm::vec3(-0.5f, -0.5f, -0.5f);
    box[18] = glm::vec3(-0.5f, -0.5f, 0.5f);
    box[19] = glm::vec3(-0.5f, -0.5f, -0.5f);
    box[20] = glm::vec3(-0.5f, 0.5f, 0.5f);
    box[21] = glm::vec3(0.5f, 0.5f, 0.5f);
    box[22] = glm::vec3(-0.5f, -0.5f, 0.5f);
    box[23] = glm::vec3(0.5f, -0.5f, 0.5f);

    CreateVertexBuffer(box, 24, cuboidVertexBuffer, cuboidGeometryBinding);

    // Create plane vertex buffer.
    glm::vec3 plane[8];
    plane[0] = glm::vec3(-1.f, -1.f, 0.f);
    plane[1] = glm::vec3(1.f, -1.f, 0.f);
    plane[2] = glm::vec3(1.f, -1.f, 0.f);
    plane[3] = glm::vec3(1.f, 1.f, 0.f);
    plane[4] = glm::vec3(1.f, 1.f, 0.f);
    plane[5] = glm::vec3(-1.f, 1.f, 0.f);
    plane[6] = glm::vec3(-1.f, 1.f, 0.f);
    plane[7] = glm::vec3(-1.f, -1.f, 0.f);

    CreateVertexBuffer(plane, 8, planeVertexBuffer, planeGeometryBinding);

    // Create circle vertex buffer.
    glm::vec3* circle;
    CreateCircle(circle, circleVertexCount, 25);
    CreateVertexBuffer(circle, circleVertexCount, circleVertexBuffer, circleGeometryBinding);
    delete[] circle;

    // Create sphere vertex buffer.
    glm::vec3* sphere;
    CreateSphere(sphere, sphereVertexCount, 14);
    CreateVertexBuffer(sphere, sphereVertexCount, sphereVertexBuffer, sphereGeometryBinding);
    delete[] sphere;

    // Create cylinder vertex buffer.
    glm::vec3* cylinder;
    CreateCylinder(cylinder, cylinderVertexCount, 14);
    CreateVertexBuffer(cylinder, cylinderVertexCount, cylinderVertexBuffer, cylinderGeometryBinding);
    delete[] cylinder;

    // Create cone vertex buffer.
    glm::vec3* cone;
    CreateCone(cone, coneVertexCount, 14);
    CreateVertexBuffer(cone, coneVertexCount, coneVertexBuffer, coneGeometryBinding);
    delete[] cone;
}

DebugDrawing::~DebugDrawing() {
    delete pointVertexBuffer;
    delete pointGeometryBinding;

    delete lineVertexBuffer;
    delete lineGeometryBinding;

    delete cuboidVertexBuffer;
    delete cuboidGeometryBinding;

    delete planeVertexBuffer;
    delete planeGeometryBinding;

    delete circleVertexBuffer;
    delete circleGeometryBinding;

    delete sphereVertexBuffer;
    delete sphereGeometryBinding;

    delete cylinderVertexBuffer;
    delete cylinderGeometryBinding;

    delete coneVertexBuffer;
    delete coneGeometryBinding;

    for (uint8_t i = 0; i < 2; ++i) {
        delete pointGraphicsPipeline[i];
        delete lineGraphicsPipeline[i];
        delete fillTriangleGraphicsPipeline[i];
        if (lowLevelRenderer->GetOptionalFeatures().fillModeNonSolid) {
            delete lineTriangleGraphicsPipeline[i];
        }
    }

    delete vertexDescription;
    delete meshVertexDescription;

    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;
}

void DebugDrawing::StartDebugDrawing(const glm::mat4& viewProjectionMatrix) {
    matricesBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(glm::mat4), &viewProjectionMatrix);
}

void DebugDrawing::DrawPoint(const Point& point) {
    BindGraphicsPipeline(pointGraphicsPipeline[point.depthTesting]);
    BindGeometry(pointGeometryBinding);

    const glm::mat4 model(glm::translate(glm::mat4(), point.position));

    CommandBuffer* commandBuffer = renderer->GetCommandBuffer();

    PushConstantData pushConst;
    pushConst.modelMatrix = model;
    pushConst.colorSize = glm::vec4(point.color, point.size);

    commandBuffer->PushConstants(&pushConst);
    commandBuffer->Draw(1);
}

void DebugDrawing::DrawLine(const Line& line) {
    BindGraphicsPipeline(lineGraphicsPipeline[line.depthTesting]);
    BindGeometry(lineGeometryBinding);

    const glm::mat4 model(glm::translate(glm::mat4(), line.startPosition) * glm::scale(glm::mat4(), line.endPosition - line.startPosition));

    CommandBuffer* commandBuffer = renderer->GetCommandBuffer();

    PushConstantData pushConst;
    pushConst.modelMatrix = model;
    pushConst.colorSize = glm::vec4(line.color, 1.0f);

    commandBuffer->PushConstants(&pushConst);
    commandBuffer->SetLineWidth(line.width);
    commandBuffer->Draw(2);
}

void DebugDrawing::DrawCuboid(const Cuboid& cuboid) {
    BindGraphicsPipeline(lineGraphicsPipeline[cuboid.depthTesting]);
    BindGeometry(cuboidGeometryBinding);

    glm::mat4 model(cuboid.matrix * glm::scale(glm::mat4(), cuboid.dimensions));

    CommandBuffer* commandBuffer = renderer->GetCommandBuffer();

    PushConstantData pushConst;
    pushConst.modelMatrix = model;
    pushConst.colorSize = glm::vec4(cuboid.color, 10.0f);

    commandBuffer->PushConstants(&pushConst);
    commandBuffer->SetLineWidth(cuboid.lineWidth);
    commandBuffer->Draw(24);
}

void DebugDrawing::DrawPlane(const Plane& plane) {
    BindGraphicsPipeline(lineGraphicsPipeline[plane.depthTesting]);
    BindGeometry(planeGeometryBinding);

    glm::mat4 model(glm::scale(glm::mat4(), glm::vec3(plane.size * 0.5f, 1.f)));
    float yaw = atan2(plane.normal.x, plane.normal.z);
    float pitch = atan2(plane.normal.y, sqrt(plane.normal.x * plane.normal.x + plane.normal.z * plane.normal.z));
    model = glm::rotate(glm::mat4(), yaw, glm::vec3(0.f, 1.f, 0.f)) * model;
    model = glm::rotate(glm::mat4(), pitch, glm::vec3(1.f, 0.f, 0.f)) * model;
    model = glm::translate(glm::mat4(), plane.position) * model;

    CommandBuffer* commandBuffer = renderer->GetCommandBuffer();

    PushConstantData pushConst;
    pushConst.modelMatrix = model;
    pushConst.colorSize = glm::vec4(plane.color, 10.0f);

    commandBuffer->PushConstants(&pushConst);
    commandBuffer->SetLineWidth(plane.lineWidth);
    commandBuffer->Draw(8);
}

void DebugDrawing::DrawCircle(const Circle& circle) {
    BindGraphicsPipeline(lineGraphicsPipeline[circle.depthTesting]);
    BindGeometry(circleGeometryBinding);

    glm::mat4 model(glm::scale(glm::mat4(), glm::vec3(circle.radius, circle.radius, circle.radius)));
    float yaw = atan2(circle.normal.x, circle.normal.z);
    float pitch = atan2(circle.normal.y, sqrt(circle.normal.x * circle.normal.x + circle.normal.z * circle.normal.z));
    model = glm::rotate(glm::mat4(), yaw, glm::vec3(0.f, 1.f, 0.f)) * model;
    model = glm::rotate(glm::mat4(), pitch, glm::vec3(1.f, 0.f, 0.f)) * model;
    model = glm::translate(glm::mat4(), circle.position) * model;

    CommandBuffer* commandBuffer = renderer->GetCommandBuffer();

    PushConstantData pushConst;
    pushConst.modelMatrix = model;
    pushConst.colorSize = glm::vec4(circle.color, 10.0f);

    commandBuffer->PushConstants(&pushConst);
    commandBuffer->SetLineWidth(circle.lineWidth);
    commandBuffer->Draw(circleVertexCount);
}

void DebugDrawing::DrawSphere(const Sphere& sphere) {
    BindGraphicsPipeline(lineGraphicsPipeline[sphere.depthTesting]);
    BindGeometry(sphereGeometryBinding);

    glm::mat4 model(glm::scale(glm::mat4(), glm::vec3(sphere.radius, sphere.radius, sphere.radius)));
    model = glm::translate(glm::mat4(), sphere.position) * model;

    CommandBuffer* commandBuffer = renderer->GetCommandBuffer();

    PushConstantData pushConst;
    pushConst.modelMatrix = model;
    pushConst.colorSize = glm::vec4(sphere.color, 10.0f);

    commandBuffer->PushConstants(&pushConst);
    commandBuffer->SetLineWidth(sphere.lineWidth);
    commandBuffer->Draw(sphereVertexCount);
}

void DebugDrawing::DrawCylinder(const Cylinder& cylinder) {
    BindGraphicsPipeline(lineGraphicsPipeline[cylinder.depthTesting]);
    BindGeometry(cylinderGeometryBinding);

    glm::mat4 model(glm::scale(glm::mat4(), glm::vec3(cylinder.radius, cylinder.length, cylinder.radius)));
    model = cylinder.matrix * model;

    CommandBuffer* commandBuffer = renderer->GetCommandBuffer();

    PushConstantData pushConst;
    pushConst.modelMatrix = model;
    pushConst.colorSize = glm::vec4(cylinder.color, 10.0f);

    commandBuffer->PushConstants(&pushConst);
    commandBuffer->SetLineWidth(cylinder.lineWidth);
    commandBuffer->Draw(cylinderVertexCount);
}

void DebugDrawing::DrawCone(const Cone& cone) {
    BindGraphicsPipeline(lineGraphicsPipeline[cone.depthTesting]);
    BindGeometry(coneGeometryBinding);

    glm::mat4 model(glm::scale(glm::mat4(), glm::vec3(cone.radius, cone.height, cone.radius)));
    model = cone.matrix * model;

    CommandBuffer* commandBuffer = renderer->GetCommandBuffer();

    PushConstantData pushConst;
    pushConst.modelMatrix = model;
    pushConst.colorSize = glm::vec4(cone.color, 10.0f);

    commandBuffer->PushConstants(&pushConst);
    commandBuffer->SetLineWidth(cone.lineWidth);
    commandBuffer->Draw(coneVertexCount);
}

void DebugDrawing::DrawMesh(const Mesh& mesh) {
    assert(mesh.geometryBinding != nullptr);

    if (mesh.wireFrame && !lowLevelRenderer->GetOptionalFeatures().fillModeNonSolid) {
        Log(Log::WARNING) << "fillModeNonSolid not supported. Failed to draw wireframe mesh.\n";
        return;
    }

    BindGraphicsPipeline(mesh.wireFrame ? lineTriangleGraphicsPipeline[mesh.depthTesting] : fillTriangleGraphicsPipeline[mesh.depthTesting]);
    BindGeometry(mesh.geometryBinding);

    CommandBuffer* commandBuffer = renderer->GetCommandBuffer();

    PushConstantData pushConst;
    pushConst.modelMatrix = mesh.matrix;
    pushConst.colorSize = glm::vec4(mesh.color, 10.0f);

    commandBuffer->PushConstants(&pushConst);
    commandBuffer->DrawIndexed(mesh.indexCount);
}

void DebugDrawing::EndDebugDrawing() {
    BindGeometry(nullptr);
    BindGraphicsPipeline(nullptr);
}

void DebugDrawing::BindGraphicsPipeline(GraphicsPipeline* graphicsPipeline) {
    if (boundGraphicsPipeline != graphicsPipeline) {
        if (graphicsPipeline != nullptr) {
            CommandBuffer* commandBuffer = renderer->GetCommandBuffer();
            commandBuffer->BindGraphicsPipeline(graphicsPipeline);
            commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), renderer->GetRenderSurfaceSize());
            commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::MATRICES, matricesBuffer);
        }

        boundGraphicsPipeline = graphicsPipeline;
    }
}

void DebugDrawing::BindGeometry(GeometryBinding* geometryBinding) {
    if (boundGeometry != geometryBinding) {
        if (geometryBinding != nullptr) {
            CommandBuffer* commandBuffer = renderer->GetCommandBuffer();
            commandBuffer->BindGeometry(geometryBinding);
        }

        boundGeometry = geometryBinding;
    }
}

void DebugDrawing::CreateVertexBuffer(const glm::vec3* positions, unsigned int positionCount, Buffer*& vertexBuffer, GeometryBinding*& geometryBinding) {
    vertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER, positionCount * sizeof(glm::vec3), static_cast<const void*>(positions));
    geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer);
}

void DebugDrawing::CreateCircle(glm::vec3*& positions, unsigned int& vertexCount, unsigned int detail) {
    vertexCount = detail * 2;
    positions = new glm::vec3[vertexCount];

    unsigned int i = 0;
    for (unsigned int j = 0; j <= detail; ++j) {
        float angle = static_cast<float>(j) / detail * 2.0f * glm::pi<float>();
        positions[i++] = glm::vec3(cos(angle), sin(angle), 0.0f);
        if (j > 0 && j < detail)
            positions[i++] = glm::vec3(cos(angle), sin(angle), 0.0f);
    }
}

// Create UV-sphere with given number of parallel and meridian lines.
void DebugDrawing::CreateSphere(glm::vec3*& positions, unsigned int& vertexCount, unsigned int detail) {
    vertexCount = detail * (4 * detail - 2);
    positions = new glm::vec3[vertexCount];

    // Horizontal lines (meridians).
    unsigned int i = 0;
    for (unsigned int m = 1; m < detail; ++m) {
        float meridian = glm::pi<float>() * m / detail;
        for (unsigned int p = 0; p <= detail; ++p) {
            float parallel = 2.0f * glm::pi<float>() * p / detail;
            float angle = glm::pi<float>() * 0.5f - meridian;
            float y = sin(angle);
            float x = cos(angle);
            positions[i++] = glm::vec3(x * cos(parallel), y, x * sin(parallel));
            if (p > 0 && p < detail)
                positions[i++] = glm::vec3(x * cos(parallel), y, x * sin(parallel));
        }
    }

    // Vertical lines (parallels).
    for (unsigned int p = 0; p < detail; ++p) {
        float parallel = 2.0f * glm::pi<float>() * p / detail;
        for (unsigned int m = 0; m <= detail; ++m) {
            float meridian = glm::pi<float>() * m / detail;
            float angle = glm::pi<float>() * 0.5f - meridian;
            float y = sin(angle);
            float x = cos(angle);
            positions[i++] = glm::vec3(x * cos(parallel), y, x * sin(parallel));
            if (m > 0 && m < detail)
                positions[i++] = glm::vec3(x * cos(parallel), y, x * sin(parallel));
        }
    }
}

void DebugDrawing::CreateCylinder(glm::vec3*& positions, unsigned int& vertexCount, unsigned int detail) {
    vertexCount = detail * 6;
    positions = new glm::vec3[vertexCount];

    unsigned int i = 0;
    for (unsigned int j = 0; j < detail; ++j) {
        float angle1 = static_cast<float>(j) / detail * 2.0f * glm::pi<float>();
        float angle2 = static_cast<float>(j + 1) / detail * 2.0f * glm::pi<float>();

        positions[i++] = glm::vec3(cos(angle1), 0.5f, sin(angle1));
        positions[i++] = glm::vec3(cos(angle1), -0.5f, sin(angle1));

        positions[i++] = glm::vec3(cos(angle1), 0.5f, sin(angle1));
        positions[i++] = glm::vec3(cos(angle2), 0.5f, sin(angle2));

        positions[i++] = glm::vec3(cos(angle1), -0.5f, sin(angle1));
        positions[i++] = glm::vec3(cos(angle2), -0.5f, sin(angle2));
    }
}

void DebugDrawing::CreateCone(glm::vec3*& positions, unsigned int& vertexCount, unsigned int detail) {
    vertexCount = detail * 4;
    positions = new glm::vec3[vertexCount];

    unsigned int i = 0;
    for (unsigned int j = 0; j < detail; ++j) {
        float angle = 2.0f * glm::pi<float>() * static_cast<float>(j) / detail;

        positions[i++] = glm::vec3(0.0f, 0.5f, 0.0f);
        positions[i++] = glm::vec3(cos(angle), -0.5f, sin(angle));
        positions[i++] = glm::vec3(cos(angle), -0.5f, sin(angle));

        angle = 2.0f * glm::pi<float>() * static_cast<float>(j + 1) / detail;
        positions[i++] = glm::vec3(cos(angle), -0.5f, sin(angle));
    }
}
