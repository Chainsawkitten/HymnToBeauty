#include "DebugDrawing.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "Shader/Shader.hpp"
#include "Shader/ShaderProgram.hpp"
#include "DebugDrawing.vert.hpp"
#include "DebugDrawing.frag.hpp"
#include "VideoErrorCheck.hpp"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

using namespace Video;

DebugDrawing::DebugDrawing() {
    Shader* vertexShader = new Shader(DEBUGDRAWING_VERT, DEBUGDRAWING_VERT_LENGTH, GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(DEBUGDRAWING_FRAG, DEBUGDRAWING_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = new ShaderProgram({ vertexShader, fragmentShader });
    delete vertexShader;
    delete fragmentShader;

    // Get uniform locations.
    viewProjectionLocation = shaderProgram->GetUniformLocation("viewProjection");
    modelLocation = shaderProgram->GetUniformLocation("model");
    colorLocation = shaderProgram->GetUniformLocation("color");
    sizeLocation = shaderProgram->GetUniformLocation("size");

    // Create point vertex array.
    glBindVertexArray(0);

    glm::vec3 point(0.f, 0.f, 0.f);
    CreateVertexArray(&point, 1, pointVertexBuffer, pointVertexArray);

    // Create line vertex array.
    glm::vec3 line[2];
    line[0] = glm::vec3(0.f, 0.f, 0.f);
    line[1] = glm::vec3(1.f, 1.f, 1.f);

    CreateVertexArray(line, 2, lineVertexBuffer, lineVertexArray);

    // Create cuboid vertex array.
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

    CreateVertexArray(box, 24, cuboidVertexBuffer, cuboidVertexArray);

    // Create plane vertex array.
    glm::vec3 plane[8];
    plane[0] = glm::vec3(-1.f, -1.f, 0.f);
    plane[1] = glm::vec3(1.f, -1.f, 0.f);
    plane[2] = glm::vec3(1.f, -1.f, 0.f);
    plane[3] = glm::vec3(1.f, 1.f, 0.f);
    plane[4] = glm::vec3(1.f, 1.f, 0.f);
    plane[5] = glm::vec3(-1.f, 1.f, 0.f);
    plane[6] = glm::vec3(-1.f, 1.f, 0.f);
    plane[7] = glm::vec3(-1.f, -1.f, 0.f);

    CreateVertexArray(plane, 8, planeVertexBuffer, planeVertexArray);

    // Create circle vertex array.
    glm::vec3* circle;
    CreateCircle(circle, circleVertexCount, 25);
    CreateVertexArray(circle, circleVertexCount, circleVertexBuffer, circleVertexArray);
    delete[] circle;

    // Create sphere vertex array.
    glm::vec3* sphere;
    CreateSphere(sphere, sphereVertexCount, 14);
    CreateVertexArray(sphere, sphereVertexCount, sphereVertexBuffer, sphereVertexArray);
    delete[] sphere;

    // Create cylinder vertex array.
    glm::vec3* cylinder;
    CreateCylinder(cylinder, cylinderVertexCount, 14);
    CreateVertexArray(cylinder, cylinderVertexCount, cylinderVertexBuffer, cylinderVertexArray);
    delete[] cylinder;

    // Create cone vertex array.
    glm::vec3* cone;
    CreateCone(cone, coneVertexCount, 14);
    CreateVertexArray(cone, coneVertexCount, coneVertexBuffer, coneVertexArray);
    delete[] cone;
}

DebugDrawing::~DebugDrawing() {
    glDeleteBuffers(1, &cuboidVertexBuffer);
    glDeleteVertexArrays(1, &cuboidVertexArray);

    glDeleteBuffers(1, &pointVertexBuffer);
    glDeleteVertexArrays(1, &pointVertexArray);

    glDeleteBuffers(1, &lineVertexBuffer);
    glDeleteVertexArrays(1, &lineVertexArray);

    glDeleteBuffers(1, &planeVertexBuffer);
    glDeleteVertexArrays(1, &planeVertexArray);

    glDeleteBuffers(1, &circleVertexBuffer);
    glDeleteVertexArrays(1, &circleVertexArray);

    glDeleteBuffers(1, &sphereVertexBuffer);
    glDeleteVertexArrays(1, &sphereVertexArray);

    glDeleteBuffers(1, &cylinderVertexBuffer);
    glDeleteVertexArrays(1, &cylinderVertexArray);

    glDeleteBuffers(1, &coneVertexBuffer);
    glDeleteVertexArrays(1, &coneVertexArray);

    delete shaderProgram;
}

void DebugDrawing::StartDebugDrawing(const glm::mat4& viewProjectionMatrix) {
    shaderProgram->Use();
    glUniformMatrix4fv(viewProjectionLocation, 1, GL_FALSE, &viewProjectionMatrix[0][0]);
}

void DebugDrawing::DrawPoint(const Point& point) {
    BindVertexArray(pointVertexArray);

    glm::mat4 model(glm::translate(glm::mat4(), point.position));

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
    point.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(colorLocation, 1, &point.color[0]);
    glUniform1f(sizeLocation, point.size);
    glDrawArrays(GL_POINTS, 0, 1);
}

void DebugDrawing::DrawLine(const Line& line) {
    BindVertexArray(lineVertexArray);

    glm::mat4 model(glm::translate(glm::mat4(), line.startPosition) * glm::scale(glm::mat4(), line.endPosition - line.startPosition));

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
    line.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(colorLocation, 1, &line.color[0]);
    glLineWidth(line.width);
    glDrawArrays(GL_LINES, 0, 2);
}

void DebugDrawing::DrawCuboid(const Cuboid& cuboid) {
    BindVertexArray(cuboidVertexArray);

    glm::mat4 model(cuboid.matrix * glm::scale(glm::mat4(), cuboid.dimensions));

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
    cuboid.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(colorLocation, 1, &cuboid.color[0]);
    glUniform1f(sizeLocation, 10.f);
    glLineWidth(cuboid.lineWidth);
    glDrawArrays(GL_LINES, 0, 24);
}

void DebugDrawing::DrawPlane(const Plane& plane) {
    BindVertexArray(planeVertexArray);

    glm::mat4 model(glm::scale(glm::mat4(), glm::vec3(plane.size * 0.5f, 1.f)));
    float yaw = atan2(plane.normal.x, plane.normal.z);
    float pitch = atan2(plane.normal.y, sqrt(plane.normal.x * plane.normal.x + plane.normal.z * plane.normal.z));
    model = glm::rotate(glm::mat4(), yaw, glm::vec3(0.f, 1.f, 0.f)) * model;
    model = glm::rotate(glm::mat4(), pitch, glm::vec3(1.f, 0.f, 0.f)) * model;
    model = glm::translate(glm::mat4(), plane.position) * model;

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
    plane.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(colorLocation, 1, &plane.color[0]);
    glUniform1f(sizeLocation, 10.f);
    glLineWidth(plane.lineWidth);
    glDrawArrays(GL_LINES, 0, 8);
}

void DebugDrawing::DrawCircle(const Circle& circle) {
    BindVertexArray(circleVertexArray);

    glm::mat4 model(glm::scale(glm::mat4(), glm::vec3(circle.radius, circle.radius, circle.radius)));
    float yaw = atan2(circle.normal.x, circle.normal.z);
    float pitch = atan2(circle.normal.y, sqrt(circle.normal.x * circle.normal.x + circle.normal.z * circle.normal.z));
    model = glm::rotate(glm::mat4(), yaw, glm::vec3(0.f, 1.f, 0.f)) * model;
    model = glm::rotate(glm::mat4(), pitch, glm::vec3(1.f, 0.f, 0.f)) * model;
    model = glm::translate(glm::mat4(), circle.position) * model;

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
    circle.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(colorLocation, 1, &circle.color[0]);
    glUniform1f(sizeLocation, 10.f);
    glLineWidth(circle.lineWidth);
    glDrawArrays(GL_LINES, 0, circleVertexCount);
}

void DebugDrawing::DrawSphere(const Sphere& sphere) {
    BindVertexArray(sphereVertexArray);

    glm::mat4 model(glm::scale(glm::mat4(), glm::vec3(sphere.radius, sphere.radius, sphere.radius)));
    model = glm::translate(glm::mat4(), sphere.position) * model;

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
    sphere.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(colorLocation, 1, &sphere.color[0]);
    glUniform1f(sizeLocation, 10.f);
    glLineWidth(sphere.lineWidth);
    glDrawArrays(GL_LINES, 0, sphereVertexCount);
}

void DebugDrawing::DrawCylinder(const Cylinder& cylinder) {
    BindVertexArray(cylinderVertexArray);

    glm::mat4 model(glm::scale(glm::mat4(), glm::vec3(cylinder.radius, cylinder.length, cylinder.radius)));
    model = cylinder.matrix * model;

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
    cylinder.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(colorLocation, 1, &cylinder.color[0]);
    glUniform1f(sizeLocation, 10.f);
    glLineWidth(cylinder.lineWidth);
    glDrawArrays(GL_LINES, 0, cylinderVertexCount);
}

void DebugDrawing::DrawCone(const Cone& cone) {
    BindVertexArray(coneVertexArray);

    glm::mat4 model(glm::scale(glm::mat4(), glm::vec3(cone.radius, cone.height, cone.radius)));
    model = cone.matrix * model;

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
    cone.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(colorLocation, 1, &cone.color[0]);
    glUniform1f(sizeLocation, 10.f);
    glLineWidth(cone.lineWidth);
    glDrawArrays(GL_LINES, 0, coneVertexCount);
}

void DebugDrawing::DrawMesh(const Mesh& mesh) {
    VIDEO_ERROR_CHECK("DebugDrawing::DrawMesh");
    assert(mesh.vertexArray != 0);

    glBindVertexArray(mesh.vertexArray);

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &mesh.matrix[0][0]);
    mesh.wireFrame ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mesh.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glUniform3fv(colorLocation, 1, &mesh.color[0]);
    glUniform1f(sizeLocation, 10.f);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void DebugDrawing::EndDebugDrawing() {
    glEnable(GL_DEPTH_TEST);
    BindVertexArray(0);
}

void DebugDrawing::BindVertexArray(GLuint vertexArray) {
    if (boundVertexArray != vertexArray) {
        glBindVertexArray(vertexArray);
        boundVertexArray = vertexArray;
    }
}

void DebugDrawing::CreateVertexArray(const glm::vec3* positions, unsigned int positionCount, GLuint& vertexBuffer, GLuint& vertexArray) {
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, positionCount * sizeof(glm::vec3), positions, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));

    glBindVertexArray(0);
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

void DebugDrawing::GenerateBuffers(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, Mesh& mesh) {
    assert(mesh.vertexBuffer != 0);
    assert(mesh.indexBuffer != 0);
    assert(mesh.vertexArray == 0);

    glGenBuffers(1, &mesh.vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    mesh.indexCount = static_cast<unsigned int>(indices.size());

    glGenVertexArrays(1, &mesh.vertexArray);
    glBindVertexArray(mesh.vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));

    glBindVertexArray(0);
}

void DebugDrawing::DeleteBuffers(Mesh& mesh) {
    if (mesh.vertexArray != 0)
        glDeleteVertexArrays(1, &mesh.vertexArray);
    if (mesh.vertexBuffer != 0)
        glDeleteBuffers(1, &mesh.vertexBuffer);
    if (mesh.indexBuffer != 0)
        glDeleteBuffers(1, &mesh.indexBuffer);

    mesh.vertexArray = 0;
    mesh.vertexBuffer = 0;
    mesh.indexBuffer = 0;
}
