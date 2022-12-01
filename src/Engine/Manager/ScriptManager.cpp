#include "ScriptManager.hpp"

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptarray/scriptarray.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptmath/scriptmath.h>
#include <scriptstdstring/scriptstdstring.h>
#include <Utility/Log.hpp>
#include <Utility/Window.hpp>
#include <Video/Geometry/Geometry3D.hpp>
#include <map>
#include <typeindex>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <iostream>

#include "../Util/FileSystem.hpp"
#include "../Util/RayIntersection.hpp"
#include "../Util/MousePicking.hpp"
#include "../Hymn.hpp"
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Script.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/Camera.hpp"
#include "../Component/Listener.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/RigidBody.hpp"
#include "../Component/SoundSource.hpp"
#include "../Component/SpotLight.hpp"
#include "../Component/Sprite.hpp"
#include "../Geometry/Model.hpp"
#include "../Input/Input.hpp"
#include "../Script/ScriptFile.hpp"

#include "Managers.hpp"
#include "InputManager.hpp"
#include "DebugDrawingManager.hpp"
#include "PhysicsManager.hpp"
#include "ResourceManager.hpp"
#include "RenderManager.hpp"

// Fix windows.h pollution.
#ifdef _WIN32
#undef SendMessage
#endif

using namespace Component;

static void AngelScriptMessageCallback(const asSMessageInfo* message, void* param) {
    Log() << message->section << " (" << message->row << ", " << message->col << " : ";

    switch (message->type) {
    case asMSGTYPE_ERROR:
        Log() << "Error";
        break;
    case asMSGTYPE_INFORMATION:
        Log() << "Information";
        break;
    case asMSGTYPE_WARNING:
        Log() << "Warning";
        break;
    }

    Log() << " : " << message->message << "\n";
}

static std::string CallstackToString(asIScriptContext* ctx) {
    std::string callstack = "Callstack:\n";
    for (asUINT n = 0; n < ctx->GetCallstackSize(); n++) {
        asIScriptFunction* func;
        const char* scriptSection;
        int line, column;
        func = ctx->GetFunction(n);
        line = ctx->GetLineNumber(n, &column, &scriptSection);
        callstack.append(func->GetDeclaration());
        callstack.append(":");
        callstack.append(std::to_string(line));
        callstack.append(",");
        callstack.append(std::to_string(column));
        callstack.append("\n");
    }
    return callstack;
}

static std::string VariablesToString(asIScriptContext* ctx, asUINT stackLevel) {
    // Print the value of each variable, including parameters
    int numVars = ctx->GetVarCount(stackLevel);
    std::string variables = "Variables:\n";
    for (int n = 0; n < numVars; n++) {
        int typeId = ctx->GetVarTypeId(n, stackLevel);
        void* varPointer = ctx->GetAddressOfVar(n, stackLevel);
        if (typeId == asTYPEID_INT32) {
            variables.append(ctx->GetVarDeclaration(n, stackLevel));
            variables.append(" = ");
            variables.append(std::to_string(*(int*)varPointer));
            variables.append("\n");
        } else if (typeId == asTYPEID_FLOAT) {
            variables.append(ctx->GetVarDeclaration(n, stackLevel));
            variables.append(" = ");
            variables.append(std::to_string(*(float*)varPointer));
            variables.append("\n");
        }
    }
    return variables;
}
// An example line callback
static void AngelScriptDebugLineCallback(asIScriptContext* ctx, const std::map<std::string, std::set<int>>* breakpoints) {
    const char* scriptSection;
    int line = ctx->GetLineNumber(0, 0, &scriptSection);

    if (scriptSection) {
        std::string fileName(scriptSection);
        fileName = fileName.substr(fileName.find_last_of("/") + 1);

        // Determine if we have reached a break point
        if (breakpoints->find(fileName) != breakpoints->end() && breakpoints->at(fileName).find(line) != breakpoints->at(fileName).end()) {
            // A break point has been reached so the execution of the script should be suspended
            // Show the call stack
            std::string callstack = CallstackToString(ctx);
            std::string variables = VariablesToString(ctx, 0);
        }
    }
}

static void print(const std::string& message) {
    Log() << message;
}

static void RegisterUpdate() {
    Managers().scriptManager->RegisterUpdate(Managers().scriptManager->currentEntity);
}

static bool ButtonInput(int buttonIndex) {
    return Input::GetInstance().CheckButton(buttonIndex);
}

static glm::vec2 GetCursorXY() {
    return Managers().inputManager->GetCursorXY();
}

static void SendMessage(Entity* recipient, int type) {
    Managers().scriptManager->SendMessage(recipient, Managers().scriptManager->currentEntity, type);
}

static void RestartScene() {
    Hymn().restart = true;
}

static void vec2Constructor(float x, float y, void* memory) {
    glm::vec2* vec = static_cast<glm::vec2*>(memory);
    vec->x = x;
    vec->y = y;
}

static void vec3Constructor(float x, float y, float z, void* memory) {
    glm::vec3* vec = static_cast<glm::vec3*>(memory);
    vec->x = x;
    vec->y = y;
    vec->z = z;
}

static void vec4Constructor(float x, float y, float z, float w, void* memory) {
    glm::vec4* vec = static_cast<glm::vec4*>(memory);
    vec->x = x;
    vec->y = y;
    vec->z = z;
    vec->w = w;
}

static void quatConstructor(float w, float x, float y, float z, void* memory) {
    *static_cast<glm::quat*>(memory) = glm::quat(w, x, y, z);
}

template <typename type> void glmConstructor(void* memory) {
    *static_cast<type*>(memory) = type();
}

template <typename type> type glmAdd(const type& a, const void* memory) {
    return *static_cast<const type*>(memory) + a;
}

template <typename type> type glmSub(const type& a, const void* memory) {
    return *static_cast<const type*>(memory) - a;
}

template <typename S, typename T> S glmMul(T a, const void* memory) {
    return *static_cast<const S*>(memory) * a;
}

template <typename type> type glmMulR(float a, const void* memory) {
    return a * *static_cast<const type*>(memory);
}

template <typename type> type glmDiv(float a, const void* memory) {
    return *static_cast<const type*>(memory) / a;
}

template <typename type> type glmDivR(float a, const void* memory) {
    return a / *static_cast<const type*>(memory);
}

template <typename type> type glmNeg(const void* memory) {
    return -*static_cast<const type*>(memory);
}

static glm::vec3 mat3MulVec3(const glm::vec3& a, const void* memory) {
    return *static_cast<const glm::mat3*>(memory) * a;
}

static glm::vec4 mat4MulVec4(const glm::vec4& a, const void* memory) {
    return *static_cast<const glm::mat4*>(memory) * a;
}

ScriptManager::ScriptManager(Utility::Window* window) {
    this->window = window;

    // Create the script engine
    engine = asCreateScriptEngine();

    // Set the message callback to receive information on errors in human readable form.
    engine->SetMessageCallback(asFUNCTION(AngelScriptMessageCallback), 0, asCALL_CDECL);

    // Register add-ons.
    RegisterStdString(engine);
    RegisterScriptArray(engine, true);
    RegisterScriptDictionary(engine);
    RegisterScriptMath(engine);

    engine->RegisterEnum("input");

    // Register GLM types.
    engine->RegisterObjectType("vec2", sizeof(glm::vec2), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::vec2>());
    engine->RegisterObjectProperty("vec2", "float x", asOFFSET(glm::vec2, x));
    engine->RegisterObjectProperty("vec2", "float y", asOFFSET(glm::vec2, y));
    engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(vec2Constructor), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2 opAdd(const vec2 &in) const", asFUNCTIONPR(glmAdd, (const glm::vec2&, const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2 opSub(const vec2 &in) const", asFUNCTIONPR(glmSub, (const glm::vec2&, const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2 opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2 opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2 opDiv(float) const", asFUNCTIONPR(glmDiv, (float, const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2 opDiv_r(float) const", asFUNCTIONPR(glmDivR, (float, const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2& opAddAssign(const vec2 &in) const", asMETHODPR(glm::vec2, operator+=,(const glm::vec2&), glm::vec2&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec2", "vec2& opSubAssign(const vec2 &in) const", asMETHODPR(glm::vec2, operator-=,(const glm::vec2&), glm::vec2&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec2", "vec2& opMulAssign(float) const", asMETHODPR(glm::vec2, operator*=,(float), glm::vec2&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec2", "vec2& opDivAssign(float) const", asMETHODPR(glm::vec2, operator/=,(float), glm::vec2&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec2", "vec2 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::vec2), asCALL_CDECL_OBJLAST);

    engine->RegisterObjectType("vec3", sizeof(glm::vec3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::vec3>());
    engine->RegisterObjectProperty("vec3", "float x", asOFFSET(glm::vec3, x));
    engine->RegisterObjectProperty("vec3", "float y", asOFFSET(glm::vec3, y));
    engine->RegisterObjectProperty("vec3", "float z", asOFFSET(glm::vec3, z));
    engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(vec3Constructor), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opAdd(const vec3 &in) const", asFUNCTIONPR(glmAdd, (const glm::vec3&, const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opSub(const vec3 &in) const", asFUNCTIONPR(glmSub, (const glm::vec3&, const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opDiv(float) const", asFUNCTIONPR(glmDiv, (float, const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opDiv_r(float) const", asFUNCTIONPR(glmDivR, (float, const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3& opAddAssign(const vec3 &in) const", asMETHODPR(glm::vec3, operator+=,(const glm::vec3&), glm::vec3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec3", "vec3& opSubAssign(const vec3 &in) const", asMETHODPR(glm::vec3, operator-=,(const glm::vec3&), glm::vec3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec3", "vec3& opMulAssign(float) const", asMETHODPR(glm::vec3, operator*=,(float), glm::vec3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec3", "vec3& opDivAssign(float) const", asMETHODPR(glm::vec3, operator/=,(float), glm::vec3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec3", "vec3 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::vec3), asCALL_CDECL_OBJLAST);

    engine->RegisterObjectType("vec4", sizeof(glm::vec4), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::vec4>());
    engine->RegisterObjectProperty("vec4", "float x", asOFFSET(glm::vec4, x));
    engine->RegisterObjectProperty("vec4", "float y", asOFFSET(glm::vec4, y));
    engine->RegisterObjectProperty("vec4", "float z", asOFFSET(glm::vec4, z));
    engine->RegisterObjectProperty("vec4", "float w", asOFFSET(glm::vec4, w));
    engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(vec4Constructor), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4 opAdd(const vec4 &in) const", asFUNCTIONPR(glmAdd, (const glm::vec4&, const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4 opSub(const vec4 &in) const", asFUNCTIONPR(glmSub, (const glm::vec4&, const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4 opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4 opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4 opDiv(float) const", asFUNCTIONPR(glmDiv, (float, const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4 opDiv_r(float) const", asFUNCTIONPR(glmDivR, (float, const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4& opAddAssign(const vec4 &in) const", asMETHODPR(glm::vec4, operator+=,(const glm::vec4&), glm::vec4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec4", "vec4& opSubAssign(const vec4 &in) const", asMETHODPR(glm::vec4, operator-=,(const glm::vec4&), glm::vec4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec4", "vec4& opMulAssign(float) const", asMETHODPR(glm::vec4, operator*=,(float), glm::vec4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec4", "vec4& opDivAssign(float) const", asMETHODPR(glm::vec4, operator/=,(float), glm::vec4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec4", "vec4 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::vec4), asCALL_CDECL_OBJLAST);

    engine->RegisterObjectType("mat3", sizeof(glm::mat3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::mat3>());
    engine->RegisterObjectBehaviour("mat3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(glmConstructor<glm::mat3>, (void*), void), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opAdd(const mat3 &in) const", asFUNCTIONPR(glmAdd, (const glm::mat3&, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opSub(const mat3 &in) const", asFUNCTIONPR(glmSub, (const glm::mat3&, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opMul(const mat3 &in) const", asFUNCTIONPR(glmMul, (const glm::mat3&, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opDiv(float) const", asFUNCTIONPR(glmDiv, (float, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opDiv_r(float) const", asFUNCTIONPR(glmDivR, (float, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3& opAddAssign(const mat3 &in) const", asMETHODPR(glm::mat3, operator+=,(const glm::mat3&), glm::mat3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat3", "mat3& opSubAssign(const mat3 &in) const", asMETHODPR(glm::mat3, operator-=,(const glm::mat3&), glm::mat3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat3", "mat3& opMulAssign(float) const", asMETHODPR(glm::mat3, operator*=,(float), glm::mat3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat3", "mat3& opDivAssign(float) const", asMETHODPR(glm::mat3, operator/=,(float), glm::mat3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat3", "mat3 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "vec3 opMul(const vec3 &in) const", asFUNCTION(mat3MulVec3), asCALL_CDECL_OBJLAST);

    engine->RegisterObjectType("mat4", sizeof(glm::mat4), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::mat4>());
    engine->RegisterObjectBehaviour("mat4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(glmConstructor<glm::mat4>, (void*), void), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opAdd(const mat4 &in) const", asFUNCTIONPR(glmAdd, (const glm::mat4&, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opSub(const mat4 &in) const", asFUNCTIONPR(glmSub, (const glm::mat4&, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opMul(const mat4 &in) const", asFUNCTIONPR(glmMul, (const glm::mat4&, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opDiv(float) const", asFUNCTIONPR(glmDiv, (float, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opDiv_r(float) const", asFUNCTIONPR(glmDivR, (float, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4& opAddAssign(const mat4 &in) const", asMETHODPR(glm::mat4, operator+=,(const glm::mat4&), glm::mat4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat4", "mat4& opSubAssign(const mat4 &in) const", asMETHODPR(glm::mat4, operator-=,(const glm::mat4&), glm::mat4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat4", "mat4& opMulAssign(float) const", asMETHODPR(glm::mat4, operator*=,(float), glm::mat4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat4", "mat4& opDivAssign(float) const", asMETHODPR(glm::mat4, operator/=,(float), glm::mat4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat4", "mat4 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "vec4 opMul(const vec4 &in) const", asFUNCTION(mat4MulVec4), asCALL_CDECL_OBJLAST);

    engine->RegisterObjectType("quat", sizeof(glm::quat), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::quat>());
    engine->RegisterObjectBehaviour("quat", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(quatConstructor), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("quat", "quat opAdd(const quat &in) const", asFUNCTIONPR(glmAdd, (const glm::quat&, const void*), glm::quat), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("quat", "quat opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::quat), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("quat", "quat opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::quat), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("quat", "quat opMul(const quat &in) const", asFUNCTIONPR(glmMul, (const glm::quat&, const void*), glm::quat), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("quat", "quat& opMulAssign(float) const", asMETHODPR(glm::quat, operator*=,(float), glm::quat&), asCALL_THISCALL);
    engine->RegisterObjectMethod("quat", "quat opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::quat), asCALL_CDECL_OBJLAST);

    // Register GLM functions.
    engine->RegisterGlobalFunction("vec2 normalize(const vec2 &in)", asFUNCTIONPR(glm::normalize, (const glm::vec2&), glm::vec2), asCALL_CDECL);
    engine->RegisterGlobalFunction("vec3 normalize(const vec3 &in)", asFUNCTIONPR(glm::normalize, (const glm::vec3&), glm::vec3), asCALL_CDECL);
    engine->RegisterGlobalFunction("vec4 normalize(const vec4 &in)", asFUNCTIONPR(glm::normalize, (const glm::vec4&), glm::vec4), asCALL_CDECL);
    engine->RegisterGlobalFunction("float length(const vec2 &in)", asFUNCTIONPR(glm::length, (const glm::vec2&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float length(const vec3 &in)", asFUNCTIONPR(glm::length, (const glm::vec3&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float length(const vec4 &in)", asFUNCTIONPR(glm::length, (const glm::vec4&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("vec3 cross(const vec3 &in, const vec3 &in)", asFUNCTIONPR(glm::cross, (const glm::vec3&, const glm::vec3&), glm::vec3), asCALL_CDECL);
    engine->RegisterGlobalFunction("float dot(const vec2 &in, const vec2 &in)", asFUNCTIONPR(glm::dot, (const glm::vec2&, const glm::vec2&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float dot(const vec3 &in, const vec3 &in)", asFUNCTIONPR(glm::dot, (const glm::vec3&, const glm::vec3&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float dot(const vec4 &in, const vec4 &in)", asFUNCTIONPR(glm::dot, (const glm::vec4&, const glm::vec4&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float distance(const vec2 &in, const vec2 &in)", asFUNCTIONPR(glm::distance, (const glm::vec2&, const glm::vec2&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float distance(const vec3 &in, const vec3 &in)", asFUNCTIONPR(glm::distance, (const glm::vec3&, const glm::vec3&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float distance(const vec4 &in, const vec4 &in)", asFUNCTIONPR(glm::distance, (const glm::vec4&, const glm::vec4&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("mat3 inverse(const mat3 &in)", asFUNCTIONPR(glm::inverse, (const glm::mat3&), glm::mat3), asCALL_CDECL);
    engine->RegisterGlobalFunction("mat4 inverse(const mat4 &in)", asFUNCTIONPR(glm::inverse, (const glm::mat4&), glm::mat4), asCALL_CDECL);
    engine->RegisterGlobalFunction("mat3 transpose(const mat3 &in)", asFUNCTIONPR(glm::transpose, (const glm::mat3&), glm::mat3), asCALL_CDECL);
    engine->RegisterGlobalFunction("mat4 transpose(const mat4 &in)", asFUNCTIONPR(glm::transpose, (const glm::mat4&), glm::mat4), asCALL_CDECL);
    engine->RegisterGlobalFunction("float determinant(const mat3 &in)", asFUNCTIONPR(glm::determinant, (const glm::mat3&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float determinant(const mat4 &in)", asFUNCTIONPR(glm::determinant, (const glm::mat4&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float pitch(const quat &in)", asFUNCTIONPR(glm::pitch, (const glm::quat&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float yaw(const quat &in)", asFUNCTIONPR(glm::yaw, (const glm::quat&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float roll(const quat &in)", asFUNCTIONPR(glm::roll, (const glm::quat&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float radians(float)", asFUNCTIONPR(glm::radians, (float), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float degrees(float)", asFUNCTIONPR(glm::degrees, (float), float), asCALL_CDECL);

    // Register Entity.
    engine->RegisterObjectType("Entity", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Entity", "string name", asOFFSET(Entity, name));
    engine->RegisterObjectProperty("Entity", "quat rotation", asOFFSET(Entity, rotation));
    engine->RegisterObjectProperty("Entity", "vec3 position", asOFFSET(Entity, position));
    engine->RegisterObjectProperty("Entity", "vec3 scale", asOFFSET(Entity, scale));
    engine->RegisterObjectMethod("Entity", "vec3 GetWorldPosition() const", asMETHOD(Entity, GetWorldPosition), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "void SetWorldPosition(const vec3 &in)", asMETHOD(Entity, SetWorldPosition), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "void Kill()", asMETHOD(Entity, Kill), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "bool IsKilled() const", asMETHOD(Entity, IsKilled), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "void SetEnabled(bool, bool)", asMETHOD(Entity, SetEnabled), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "bool IsEnabled() const", asMETHOD(Entity, IsEnabled), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ GetParent() const", asMETHOD(Entity, GetParent), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ InstantiateScene(const string &in)", asMETHODPR(Entity, InstantiateScene, (const std::string&), Entity*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "bool IsScene() const", asMETHOD(Entity, IsScene), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ GetChild(const string &in) const", asMETHOD(Entity, GetChild), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ GetChildFromIndex(int) const", asMETHOD(Entity, GetChildFromIndex), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "uint GetNumChildren() const", asMETHOD(Entity, GetNumChildren), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "uint GetUniqueIdentifier() const", asMETHOD(Entity, GetUniqueIdentifier), asCALL_THISCALL);

    engine->RegisterGlobalFunction("Entity@ GetEntityByGUID(uint GUID)", asFUNCTIONPR(ActiveHymn::GetEntityByGUID, (unsigned int), Entity*), asCALL_CDECL);

    engine->RegisterObjectMethod("Entity", "void RotateYaw(float angle)", asMETHOD(Entity, RotateYaw), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "void RotatePitch(float angle)", asMETHOD(Entity, RotatePitch), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "void RotateRoll(float angle)", asMETHOD(Entity, RotateRoll), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "void RotateAroundWorldAxis(float, const vec3 &in)", asMETHOD(Entity, RotateAroundWorldAxis), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "quat GetWorldOrientation()", asMETHOD(Entity, GetWorldOrientation), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "void SetWorldOrientation(const quat& in)", asMETHOD(Entity, SetWorldOrientation), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "void SetLocalOrientation(const quat& in)", asMETHOD(Entity, SetLocalOrientation), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ SetParent(Entity@ parent) const", asMETHOD(Entity, SetParent), asCALL_THISCALL);

    // Register components.
    engine->SetDefaultNamespace("Component");

    engine->RegisterObjectType("DirectionalLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("DirectionalLight", "vec3 color", asOFFSET(DirectionalLight, color));
    engine->RegisterObjectProperty("DirectionalLight", "float ambientCoefficient", asOFFSET(DirectionalLight, ambientCoefficient));

    engine->RegisterObjectType("Camera", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Camera", "bool orthographic", asOFFSET(Camera, orthographic));
    engine->RegisterObjectProperty("Camera", "float fieldOfView", asOFFSET(Camera, fieldOfView));
    engine->RegisterObjectProperty("Camera", "float size", asOFFSET(Camera, size));
    engine->RegisterObjectProperty("Camera", "float zNear", asOFFSET(Camera, zNear));
    engine->RegisterObjectProperty("Camera", "float zFar", asOFFSET(Camera, zFar));
    engine->RegisterObjectProperty("Camera", "vec4 viewport", asOFFSET(Camera, viewport));
    engine->RegisterObjectProperty("Camera", "int order", asOFFSET(Camera, order));
    engine->RegisterObjectProperty("Camera", "bool overlay", asOFFSET(Camera, overlay));
    engine->RegisterObjectProperty("Camera", "uint layerMask", asOFFSET(Camera, layerMask));

    engine->RegisterObjectType("Listener", 0, asOBJ_REF | asOBJ_NOCOUNT);

    engine->RegisterObjectType("Mesh", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Mesh", "uint layerMask", asOFFSET(Mesh, layerMask));

    engine->RegisterObjectType("PointLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("PointLight", "vec3 color", asOFFSET(PointLight, color));
    engine->RegisterObjectProperty("PointLight", "float attenuation", asOFFSET(PointLight, attenuation));
    engine->RegisterObjectProperty("PointLight", "float intensity", asOFFSET(PointLight, intensity));

    engine->RegisterObjectType("RigidBody", 0, asOBJ_REF | asOBJ_NOCOUNT);

    engine->RegisterObjectType("SpotLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("SpotLight", "vec3 color", asOFFSET(SpotLight, color));
    engine->RegisterObjectProperty("SpotLight", "float attenuation", asOFFSET(SpotLight, attenuation));
    engine->RegisterObjectProperty("SpotLight", "float intensity", asOFFSET(SpotLight, intensity));
    engine->RegisterObjectProperty("SpotLight", "float coneAngle", asOFFSET(SpotLight, coneAngle));

    engine->RegisterObjectType("SoundSource", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("SoundSource", "float volume", asOFFSET(SoundSource, gain));
    engine->RegisterObjectProperty("SoundSource", "bool loop", asOFFSET(SoundSource, loop));
    engine->RegisterObjectMethod("SoundSource", "void Play()", asMETHOD(SoundSource, Play), asCALL_THISCALL);
    engine->RegisterObjectMethod("SoundSource", "void Pause()", asMETHOD(SoundSource, Pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("SoundSource", "void Stop()", asMETHOD(SoundSource, Stop), asCALL_THISCALL);

    engine->RegisterObjectType("Sprite", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Sprite", "float pixelsPerUnit", asOFFSET(Sprite, pixelsPerUnit));
    engine->RegisterObjectProperty("Sprite", "vec2 pivot", asOFFSET(Sprite, pivot));
    engine->RegisterObjectProperty("Sprite", "vec3 tint", asOFFSET(Sprite, tint));
    engine->RegisterObjectProperty("Sprite", "float alpha", asOFFSET(Sprite, alpha));
    engine->RegisterObjectProperty("Sprite", "uint layerMask", asOFFSET(Sprite, layerMask));

    engine->SetDefaultNamespace("");

    // Register getting components.
    engine->RegisterObjectMethod("Entity", "Component::DirectionalLight@ GetDirectionalLight()", asMETHODPR(Entity, GetComponent, () const, DirectionalLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Camera@ GetCamera()", asMETHODPR(Entity, GetComponent, () const, Camera*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Listener@ GetListener()", asMETHODPR(Entity, GetComponent, () const, Listener*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Mesh@ GetMesh()", asMETHODPR(Entity, GetComponent, () const, Mesh*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::PointLight@ GetPointLight()", asMETHODPR(Entity, GetComponent, () const, PointLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::RigidBody@ GetRigidBody()", asMETHODPR(Entity, GetComponent, () const, RigidBody*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::SpotLight@ GetSpotLight()", asMETHODPR(Entity, GetComponent, () const, SpotLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::SoundSource@ GetSoundSource()", asMETHODPR(Entity, GetComponent, () const, SoundSource*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Sprite@ GetSprite()", asMETHODPR(Entity, GetComponent, () const, Sprite*), asCALL_THISCALL);

    // Register managers.
    engine->RegisterObjectType("DebugDrawingManager", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddPoint(const vec3 &in, const vec3 &in, float, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddPoint), asCALL_THISCALL);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddLine(const vec3 &in, const vec3 &in, const vec3 &in, float = 1.0, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddLine), asCALL_THISCALL);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddCuboid(const vec3 &in, const mat4 &in, const vec3 &in, float = 1.0, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddCuboid), asCALL_THISCALL);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddPlane(const vec3 &in, const vec3 &in, const vec2 &in, const vec3 &in, float = 1.0, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddPlane), asCALL_THISCALL);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddSphere(const vec3 &in, float, const vec3 &in, float = 1.0, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddSphere), asCALL_THISCALL);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddCylinder(float, float, const mat4& in, const vec3 &in, float = 1.0, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddCylinder), asCALL_THISCALL);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddCone(float, float, const mat4& in, const vec3 &in, float = 1.0, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddCone), asCALL_THISCALL);

    engine->RegisterObjectType("PhysicsManager", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectMethod("PhysicsManager", "void MakeKinematic(Component::RigidBody@)", asMETHOD(PhysicsManager, MakeKinematic), asCALL_THISCALL);
    engine->RegisterObjectMethod("PhysicsManager", "void MakeDynamic(Component::RigidBody@)", asMETHOD(PhysicsManager, MakeDynamic), asCALL_THISCALL);
    engine->RegisterObjectMethod("PhysicsManager", "void ForceTransformSync(Component::RigidBody@)", asMETHOD(PhysicsManager, ForceTransformSync), asCALL_THISCALL);
    engine->RegisterObjectMethod("PhysicsManager", "void HaltMovement(Component::RigidBody@)", asMETHOD(PhysicsManager, HaltMovement), asCALL_THISCALL);

    engine->RegisterObjectType("Hub", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Hub", "DebugDrawingManager@ debugDrawingManager", asOFFSET(Hub, debugDrawingManager));
    engine->RegisterObjectProperty("Hub", "PhysicsManager@ physicsManager", asOFFSET(Hub, physicsManager));

    // Register functions.
    engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
    engine->RegisterGlobalFunction("void RestartScene()", asFUNCTION(RestartScene), asCALL_CDECL);
    engine->RegisterGlobalFunction("void RegisterUpdate()", asFUNCTION(::RegisterUpdate), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool Input(input button)", asFUNCTION(ButtonInput), asCALL_CDECL);
    engine->RegisterGlobalFunction("void SendMessage(Entity@, int)", asFUNCTION(::SendMessage), asCALL_CDECL);
    engine->RegisterGlobalFunction("Hub@ Managers()", asFUNCTION(Managers), asCALL_CDECL);
    engine->RegisterGlobalFunction("vec2 GetCursorXY()", asFUNCTION(GetCursorXY), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool IsIntersect(Entity@, Entity@)", asMETHOD(ScriptManager, IsIntersect), asCALL_THISCALL_ASGLOBAL, this);
    engine->RegisterGlobalFunction("void exit(int)", asFUNCTION(exit), asCALL_CDECL);
}

ScriptManager::~ScriptManager() {
    engine->ShutDownAndRelease();
}

int ScriptManager::BuildScript(ScriptFile* script) {
    GetBreakpoints(script);

    std::string filename = Hymn().GetPath() + "/" + script->path + script->name;
    if (!FileSystem::FileExists(filename.c_str())) {
        Log() << "Script file does not exist: " << filename << "\n";
        return -1;
    }

    // Create and build script module.
    CScriptBuilder builder;
    int r = builder.StartNewModule(engine, script->module.c_str());
    if (r < 0) {
        Log() << "Couldn't start new module: " << script->module << ".\n";
        return r;
    }

    r = builder.AddSectionFromFile(filename.c_str());
    if (r < 0) {
        Log() << "File section could not be added: " << filename << ".\n";
        return r;
    }

    r = builder.BuildModule();
    if (r < 0) {
        Log() << "Compile errors.\n";
        return r;
    }

    FillFunctionVector(script);

    return r;
}

void ScriptManager::BuildAllScripts() {
    std::string path = Hymn().GetPath() + "/";

    for (ScriptFile* file : Hymn().scripts) {
        std::string filename = path + file->path + file->name;
        if (!FileSystem::FileExists(filename.c_str())) {
            Log() << "Script file does not exist: " << filename << "\n";
            return;
        }

        // We get the breakpoints.
        GetBreakpoints(file);

        // Create and build script module.
        CScriptBuilder builder;
        asIScriptModule* module = engine->GetModule(file->module.c_str());
        if (module == nullptr) {
            int r = builder.StartNewModule(engine, file->module.c_str());
            if (r < 0)
                Log() << "Couldn't start new module: " << file->module << ".\n";

            r = builder.AddSectionFromFile(filename.c_str());
            if (r < 0)
                Log() << "File section could not be added: " << filename << ".\n";

            r = builder.BuildModule();
            if (r < 0)
                Log() << "Compile errors.\n";
        } else {
            std::string script;

            LoadScriptFile(filename.c_str(), script);
            module->AddScriptSection(filename.c_str(), script.c_str());

            int r = module->Build();
            if (r < 0)
                Log() << file->name.c_str() << "Compile errors.\n";
        }

        FillFunctionVector(file);
    }
}

void ScriptManager::GetBreakpoints(const ScriptFile* scriptFile) {
    // If we already fetched the breakpoints for this file, we clear it.
    auto it = breakpoints.find(scriptFile->name);
    if (it != breakpoints.end())
        breakpoints[scriptFile->name].clear();

    std::string path = Hymn().GetPath() + "/";
    std::string filePath = path + scriptFile->path + scriptFile->name;

    std::string scriptLines;
    LoadScriptFile(filePath.c_str(), scriptLines);

    std::istringstream f(scriptLines);
    std::string line;
    int lineNumber = 1;
    while (std::getline(f, line)) {
        if (line.length() >= 8) {
            std::string end = line.substr(line.length() - 8, 7);
            if (end == "//break" || end == "//Break" || end == "//BREAK")
                breakpoints[scriptFile->name].insert(lineNumber);
        }
        lineNumber++;
    }
}

void ScriptManager::ClearBreakpoints() {
    for (auto pair : breakpoints)
        pair.second.clear();

    breakpoints.clear();
}

void ScriptManager::FillPropertyMap(Script* script) {
    int r = BuildScript(script->scriptFile);
    if (r < 0) {
        Log() << "Couldn't fetch properties"
              << "\n";
    } else {
        if (!script->initialized)
            CreateInstance(script);

        int propertyCount = script->instance->GetPropertyCount();

        for (int n = 0; n < propertyCount; n++) {
            std::string name = script->instance->GetPropertyName(n);
            int typeId = script->instance->GetPropertyTypeId(n);
            void* varPointer = script->instance->GetAddressOfProperty(n);

            if (script->IsInPropertyMap(name, typeId))
                continue;

            if (typeId == asTYPEID_INT32) {
                int size = sizeof(int);
                script->AddToPropertyMap(name, typeId, size, varPointer);
            } else if (typeId == asTYPEID_FLOAT) {
                int size = sizeof(float);
                script->AddToPropertyMap(name, typeId, size, varPointer);
            } else if (typeId == engine->GetTypeIdByDecl("Entity@") && name != "self") {
                int size = sizeof(unsigned int);

                Entity* pointer = *(Entity**)varPointer;

                const std::vector<Entity*> entities = Hymn().world.GetEntities();

                bool initialized = false;
                for (std::size_t i = 0; i < entities.size(); ++i) {
                    if (entities[i] == pointer) {
                        initialized = true;
                        break;
                    }
                }

                if (initialized) {
                    unsigned int GUID = pointer->GetUniqueIdentifier();
                    script->AddToPropertyMap(name, typeId, size, (void*)(&GUID));

                } else {
                    // We start with setting the GUID to 0, which means it's uninitialized.
                    unsigned int GUID = 0;
                    script->AddToPropertyMap(name, typeId, size, (void*)(&GUID));
                }
            }
        }
    }
}

void ScriptManager::FillFunctionVector(ScriptFile* scriptFile) {
    scriptFile->functionList.clear();

    asITypeInfo* scriptClass = GetClass(scriptFile->module, scriptFile->module);
    if (scriptClass != nullptr) {
        int functionCount = scriptClass->GetMethodCount();
        for (int n = 0; n < functionCount; n++) {
            asIScriptFunction* func = scriptClass->GetMethodByIndex(n);
            std::string decl = func->GetDeclaration(false);

            scriptFile->functionList.push_back(decl);
        }
    }
}

void ScriptManager::Update(World& world, float deltaTime) {
    // Init.
    for (Script* script : scripts.GetAll()) {
        if (!script->initialized && !script->IsKilled() && script->entity->IsEnabled()) {
            CreateInstance(script);

            // Skip if not initialized
            if (!script->initialized)
                continue;

            FillPropertyMap(script);

            int propertyCount = script->instance->GetPropertyCount();

            for (int n = 0; n < propertyCount; n++) {
                std::string name = script->instance->GetPropertyName(n);
                int typeId = script->instance->GetPropertyTypeId(n);
                void* varPointer = script->instance->GetAddressOfProperty(n);

                if (script->IsInPropertyMap(name, typeId)) {
                    if (typeId == engine->GetTypeIdByDecl("Entity@")) {
                        unsigned int* GUID = (unsigned int*)script->GetDataFromPropertyMap(name);

                        // We make sure it is initialized.
                        if (*GUID != 0)
                            *reinterpret_cast<Entity**>(varPointer) = Hymn().GetEntityByGUID(*GUID);
                        else
                            Log() << "Property " << name << " of script " << script->scriptFile->name << " on entity " << script->entity->name << " is not initialized"
                                  << "\n";
                    } else
                        script->CopyDataFromPropertyMap(name, varPointer);
                }
            }
        }
    }

    // Update.
    for (Entity* entity : world.GetUpdateEntities()) {
        this->currentEntity = entity;
        if (currentEntity->IsEnabled())
            CallUpdate(entity, deltaTime);
    }

    // Handle messages.
    while (!messages.empty()) {
        std::vector<Message> temp = messages;
        messages.clear();

        for (const Message& message : temp)
            CallMessageReceived(message);
    }

    // Register entities for events.
    for (Entity* entity : updateEntities)
        world.RegisterUpdate(entity);
    updateEntities.clear();
}

void ScriptManager::RegisterUpdate(Entity* entity) {
    updateEntities.push_back(entity);
}

void ScriptManager::RegisterInput() {
    // Get the input enum.
    asUINT enumCount = engine->GetEnumCount();
    asITypeInfo* inputEnum = nullptr;
    for (asUINT i = 0; i < enumCount; ++i) {
        asITypeInfo* asEnum = engine->GetEnumByIndex(i);
        std::string name = asEnum->GetName();
        if (name == "input") {
            inputEnum = asEnum;
            break;
        }
    }

    for (std::size_t i = 0; i < Input::GetInstance().buttons.size(); ++i) {
        Input::Button* button = Input::GetInstance().buttons[i];

        // Check if we've already registered the button.
        bool registered = false;
        asUINT inputCount = inputEnum->GetEnumValueCount();
        for (asUINT j = 0; j < inputCount; ++j) {
            int value;
            std::string registeredButton = inputEnum->GetEnumValueByIndex(j, &value);
            if (registeredButton == button->action) {
                registered = true;
                break;
            }
        }

        if (!registered)
            engine->RegisterEnumValue("input", std::string(button->action).c_str(), static_cast<int>(i));
    }
}

void ScriptManager::SendMessage(Entity* recipient, Entity* sender, int type) {
    Message message;
    message.recipient = recipient;
    message.sender = sender;
    message.type = type;
    messages.push_back(message);
}

Component::Script* ScriptManager::CreateScript() {
    return scripts.Create();
}

int ScriptManager::GetStringDeclarationID() {
    return engine->GetTypeIdByDecl("string");
}

const std::vector<Component::Script*>& ScriptManager::GetScripts() const {
    return scripts.GetAll();
}

void ScriptManager::ClearKilledComponents() {
    scripts.ClearKilled();
}

void ScriptManager::ExecuteScriptMethod(const Entity* entity, const std::string& method) {
    Component::Script* script = entity->GetComponent<Component::Script>();
    if (!script)
        return;
    currentEntity = script->entity;

    ScriptFile* scriptFile = script->scriptFile;

    // Get class.
    asITypeInfo* type = GetClass(scriptFile->module, scriptFile->module);

    // Find method to call.
    std::string methodDecl;
    methodDecl.append(method);
    asIScriptFunction* scriptMethod = type->GetMethodByDecl(methodDecl.c_str());
    if (scriptMethod == nullptr)
        Log() << "Can't find method void " << method << "()\n";

    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(scriptMethod);
    context->SetObject(script->instance);
    ExecuteCall(context, scriptFile->name);

    // Clean up.
    context->Release();
}

void ScriptManager::CreateInstance(Component::Script* script) {
    currentEntity = script->entity;
    ScriptFile* scriptFile = script->scriptFile;

    // Skip if no script file.
    if (!scriptFile)
        return;

    // Find the class to instantiate.
    asITypeInfo* type = GetClass(scriptFile->module, scriptFile->module);

    // Skip if no class is found.
    if (!type)
        return;

    // Find factory function / constructor.
    std::string factoryName = scriptFile->module + "@ " + scriptFile->module + "(Entity@)";
    asIScriptFunction* factoryFunction = type->GetFactoryByDecl(factoryName.c_str());
    if (factoryFunction == nullptr)
        Log() << "Couldn't find the factory function for " << scriptFile->module << ".\n";

    // Create context, prepare it and execute.
    asIScriptContext* context = CreateContext();
    context->Prepare(factoryFunction);
    context->SetArgObject(0, script->entity);
    ExecuteCall(context, scriptFile->name);

    // Get the newly created object.
    script->instance = *(static_cast<asIScriptObject**>(context->GetAddressOfReturnValue()));
    script->instance->AddRef();

    // Clean up.
    context->Release();

    // Set initialized.
    script->initialized = true;
}

asIScriptContext* ScriptManager::CreateContext() {
    asIScriptContext* context = engine->CreateContext();
    context->SetLineCallback(asFUNCTION(AngelScriptDebugLineCallback), &breakpoints, asCALL_CDECL);
    return context;
}

void ScriptManager::CallMessageReceived(const Message& message) {
    currentEntity = message.recipient;
    Component::Script* script = currentEntity->GetComponent<Component::Script>();
    ScriptFile* scriptFile = script->scriptFile;

    // Get class.
    asITypeInfo* type = GetClass(scriptFile->module, scriptFile->module);

    // Find method to call.
    asIScriptFunction* method = type->GetMethodByDecl("void ReceiveMessage(Entity@, int)");
    if (method == nullptr)
        Log() << "Can't find method void ReceiveMessage(Entity@, int)\n";

    // Create context, prepare it and execute.
    asIScriptContext* context = CreateContext();
    context->Prepare(method);
    context->SetObject(script->instance);
    context->SetArgAddress(0, message.sender);
    context->SetArgDWord(1, message.type);
    ExecuteCall(context, scriptFile->name);

    // Clean up.
    context->Release();
}

void ScriptManager::CallUpdate(Entity* entity, float deltaTime) {
    Component::Script* script = entity->GetComponent<Component::Script>();
    ScriptFile* scriptFile = script->scriptFile;

    // Get class.
    asITypeInfo* type = GetClass(scriptFile->module, scriptFile->module);

    // Find method to call.
    asIScriptFunction* method = type->GetMethodByDecl("void Update(float)");
    if (method == nullptr)
        Log() << "Can't find method void Update(float)\n";

    // Create context, prepare it and execute.
    asIScriptContext* context = CreateContext();
    context->Prepare(method);
    context->SetObject(script->instance);
    context->SetArgFloat(0, deltaTime);
    ExecuteCall(context, scriptFile->name);

    // Clean up.
    context->Release();
}

void ScriptManager::LoadScriptFile(const char* fileName, std::string& script) {
    // Open the file in binary mode
    FILE* f = fopen(fileName, "rb");

    // Determine the size of the file
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Load the entire file in one call
    script.resize(len);
    fread(&script[0], len, 1, f);

    fclose(f);
}

void ScriptManager::ExecuteCall(asIScriptContext* context, const std::string& scriptName) {
    int r = context->Execute();
    if (r != asEXECUTION_FINISHED) {
        // The execution didn't complete as expected. Determine what happened.
        if (r == asEXECUTION_EXCEPTION) {
            // An exception occurred, let the script writer know what happened so it can be corrected.
            Log() << "An exception '" << context->GetExceptionString() << "' occurred in " << scriptName << ". Please correct the code and try again.\n";
        }
    }
}

asITypeInfo* ScriptManager::GetClass(const std::string& moduleName, const std::string& className) {
    // Get script module.
    asIScriptModule* module = engine->GetModule(moduleName.c_str(), asGM_ONLY_IF_EXISTS);
    if (module == nullptr) {
        Log() << "Couldn't find \"" << moduleName << "\" module.\n";
        return nullptr;
    }

    // Find the class.
    asUINT typeCount = module->GetObjectTypeCount();
    for (asUINT i = 0; i < typeCount; ++i) {
        asITypeInfo* type = module->GetObjectTypeByIndex(i);
        if (strcmp(type->GetName(), className.c_str()) == 0)
            return type;
    }

    Log() << "Couldn't find class \"" << className << "\".\n";
    return nullptr;
}

bool ScriptManager::IsIntersect(Entity* checker, Entity* camera) const {
    const glm::mat4 projection = camera->GetComponent<Component::Camera>()->GetProjection(glm::vec2(window->GetSize().x, window->GetSize().y));
    RayIntersection rayIntersector;
    float intersectDistance;
    const glm::vec3 rayDirection = MousePicking::GetRayDirection(camera, projection, window);
    if (rayIntersector.RayOBBIntersect(camera->GetWorldPosition(), rayDirection, checker->GetComponent<Component::Mesh>()->model->GetAxisAlignedBoundingBox(), checker->GetModelMatrix(), intersectDistance)) {
        if (intersectDistance < 10.0f)
            return true;
        return false;
    }
    return false;
}

const std::vector<Entity*>& ScriptManager::GetUpdateEntities() {
    return updateEntities;
}
