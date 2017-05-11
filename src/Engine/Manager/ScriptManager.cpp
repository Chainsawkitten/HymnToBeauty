#include "ScriptManager.hpp"

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptmath/scriptmath.h>
#include <scriptstdstring/scriptstdstring.h>
#include "../Util/Log.hpp"
#include "../Util/FileSystem.hpp"
#include "../Hymn.hpp"
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Script.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Listener.hpp"
#include "../Component/Physics.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/SoundSource.hpp"
#include "../Component/SpotLight.hpp"
#include "../Input/Input.hpp"
#include "../Script/ScriptFile.hpp"

#include "Managers.hpp"
#include "DebugDrawingManager.hpp"

using namespace Component;

void print(const std::string& message) {
    Log() << message;
}

Entity* GetEntity() {
    return Managers().scriptManager->currentEntity;
}

void RegisterUpdate() {
    Managers().scriptManager->RegisterUpdate(GetEntity());
}

bool Input(int buttonIndex) {
    return Input::GetInstance().CheckButton(buttonIndex);
}

void SendMessage(Entity* recipient, int type) {
    Managers().scriptManager->SendMessage(recipient, type);
}

void vec2Constructor(float x, float y, void* memory) {
    glm::vec2* vec = static_cast<glm::vec2*>(memory);
    vec->x = x;
    vec->y = y;
}

void vec3Constructor(float x, float y, float z, void* memory) {
    glm::vec3* vec = static_cast<glm::vec3*>(memory);
    vec->x = x;
    vec->y = y;
    vec->z = z;
}

void vec4Constructor(float x, float y, float z, float w, void* memory) {
    glm::vec4* vec = static_cast<glm::vec4*>(memory);
    vec->x = x;
    vec->y = y;
    vec->z = z;
    vec->w = w;
}

template<typename type> void glmConstructor(void* memory) {
    *static_cast<type*>(memory) = type();
}

template<typename type> type glmAdd(const type& a, const void* memory) {
    return *static_cast<const type*>(memory) + a;
}

template<typename type> type glmSub(const type& a, const void* memory) {
    return *static_cast<const type*>(memory) - a;
}

template<typename type> type glmMul(float a, const void* memory) {
    return *static_cast<const type*>(memory) * a;
}

template<typename type> type glmMulR(float a, const void* memory) {
    return a * *static_cast<const type*>(memory);
}

template<typename type> type glmDiv(float a, const void* memory) {
    return *static_cast<const type*>(memory) / a;
}

template<typename type> type glmDivR(float a, const void* memory) {
    return a / *static_cast<const type*>(memory);
}

template<typename type> type glmNeg(const void* memory) {
    return -*static_cast<const type*>(memory);
}

glm::vec3 mat3MulVec3(const glm::vec3& a, const void* memory) {
    return *static_cast<const glm::mat3*>(memory) * a;
}

glm::vec4 mat4MulVec4(const glm::vec4& a, const void* memory) {
    return *static_cast<const glm::mat4*>(memory) * a;
}

ScriptManager::ScriptManager() {
    // Create the script engine
    engine = asCreateScriptEngine();
    
    // Set the message callback to receive information on errors in human readable form.
    engine->SetMessageCallback(asFUNCTION(AngelScriptMessageCallback), 0, asCALL_CDECL);
    
    // Register add-ons.
    RegisterStdString(engine);
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
    engine->RegisterObjectMethod("vec2", "vec2& opAddAssign(const vec2 &in) const", asMETHODPR(glm::vec2, operator+=, (const glm::vec2&), glm::vec2&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec2", "vec2& opSubAssign(const vec2 &in) const", asMETHODPR(glm::vec2, operator-=, (const glm::vec2&), glm::vec2&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec2", "vec2& opMulAssign(float) const", asMETHODPR(glm::vec2, operator*=, (float), glm::vec2&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec2", "vec2& opDivAssign(float) const", asMETHODPR(glm::vec2, operator/=, (float), glm::vec2&), asCALL_THISCALL);
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
    engine->RegisterObjectMethod("vec3", "vec3& opAddAssign(const vec3 &in) const", asMETHODPR(glm::vec3, operator+=, (const glm::vec3&), glm::vec3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec3", "vec3& opSubAssign(const vec3 &in) const", asMETHODPR(glm::vec3, operator-=, (const glm::vec3&), glm::vec3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec3", "vec3& opMulAssign(float) const", asMETHODPR(glm::vec3, operator*=, (float), glm::vec3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec3", "vec3& opDivAssign(float) const", asMETHODPR(glm::vec3, operator/=, (float), glm::vec3&), asCALL_THISCALL);
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
    engine->RegisterObjectMethod("vec4", "vec4& opAddAssign(const vec4 &in) const", asMETHODPR(glm::vec4, operator+=, (const glm::vec4&), glm::vec4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec4", "vec4& opSubAssign(const vec4 &in) const", asMETHODPR(glm::vec4, operator-=, (const glm::vec4&), glm::vec4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec4", "vec4& opMulAssign(float) const", asMETHODPR(glm::vec4, operator*=, (float), glm::vec4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec4", "vec4& opDivAssign(float) const", asMETHODPR(glm::vec4, operator/=, (float), glm::vec4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec4", "vec4 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    
    engine->RegisterObjectType("mat3", sizeof(glm::mat3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::mat3>());
    engine->RegisterObjectBehaviour("mat3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(glmConstructor<glm::mat3>, (void*), void), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opAdd(const mat3 &in) const", asFUNCTIONPR(glmAdd, (const glm::mat3&, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opSub(const mat3 &in) const", asFUNCTIONPR(glmSub, (const glm::mat3&, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opDiv(float) const", asFUNCTIONPR(glmDiv, (float, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opDiv_r(float) const", asFUNCTIONPR(glmDivR, (float, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3& opAddAssign(const mat3 &in) const", asMETHODPR(glm::mat3, operator+=, (const glm::mat3&), glm::mat3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat3", "mat3& opSubAssign(const mat3 &in) const", asMETHODPR(glm::mat3, operator-=, (const glm::mat3&), glm::mat3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat3", "mat3& opMulAssign(float) const", asMETHODPR(glm::mat3, operator*=, (float), glm::mat3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat3", "mat3& opDivAssign(float) const", asMETHODPR(glm::mat3, operator/=, (float), glm::mat3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat3", "mat3 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "vec3 opMul(const vec3 &in) const", asFUNCTION(mat3MulVec3), asCALL_CDECL_OBJLAST);
    
    engine->RegisterObjectType("mat4", sizeof(glm::mat4), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::mat4>());
    engine->RegisterObjectBehaviour("mat4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(glmConstructor<glm::mat4>, (void*), void), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opAdd(const mat4 &in) const", asFUNCTIONPR(glmAdd, (const glm::mat4&, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opSub(const mat4 &in) const", asFUNCTIONPR(glmSub, (const glm::mat4&, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opDiv(float) const", asFUNCTIONPR(glmDiv, (float, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opDiv_r(float) const", asFUNCTIONPR(glmDivR, (float, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4& opAddAssign(const mat4 &in) const", asMETHODPR(glm::mat4, operator+=, (const glm::mat4&), glm::mat4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat4", "mat4& opSubAssign(const mat4 &in) const", asMETHODPR(glm::mat4, operator-=, (const glm::mat4&), glm::mat4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat4", "mat4& opMulAssign(float) const", asMETHODPR(glm::mat4, operator*=, (float), glm::mat4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat4", "mat4& opDivAssign(float) const", asMETHODPR(glm::mat4, operator/=, (float), glm::mat4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat4", "mat4 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "vec4 opMul(const vec4 &in) const", asFUNCTION(mat4MulVec4), asCALL_CDECL_OBJLAST);
    
    // Register GLM functions.
    engine->RegisterGlobalFunction("vec2 normalize(const vec2 &in)", asFUNCTIONPR(glm::normalize, (const glm::vec2&), glm::vec2), asCALL_CDECL);
    engine->RegisterGlobalFunction("vec3 normalize(const vec3 &in)", asFUNCTIONPR(glm::normalize, (const glm::vec3&), glm::vec3), asCALL_CDECL);
    engine->RegisterGlobalFunction("vec4 normalize(const vec4 &in)", asFUNCTIONPR(glm::normalize, (const glm::vec4&), glm::vec4), asCALL_CDECL);
    engine->RegisterGlobalFunction("float length(const vec2 &in)", asFUNCTIONPR(glm::length, (const glm::vec2&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float length(const vec3 &in)", asFUNCTIONPR(glm::length, (const glm::vec3&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float length(const vec4 &in)", asFUNCTIONPR(glm::length, (const glm::vec4&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("vec3 cross(const vec3 &in, const vec3 &in)", asFUNCTIONPR(glm::cross, (const glm::vec3&, const glm::vec3&), glm::vec3), asCALL_CDECL);
    engine->RegisterGlobalFunction("float dot(const vec2 &in, const vec2 &in)", asFUNCTIONPR(glm::dot, (const glm::vec2&, const glm::vec2&), float), asCALL_CDECL);
    
    // Register Entity.
    engine->RegisterObjectType("Entity", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Entity", "string name", asOFFSET(Entity, name));
    engine->RegisterObjectProperty("Entity", "vec3 position", asOFFSET(Entity, position));
    engine->RegisterObjectProperty("Entity", "vec3 scale", asOFFSET(Entity, scale));
    engine->RegisterObjectProperty("Entity", "vec3 rotation", asOFFSET(Entity, rotation));
    engine->RegisterObjectMethod("Entity", "void Kill()", asMETHOD(Entity, Kill), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "bool IsKilled() const", asMETHOD(Entity, IsKilled), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ GetParent() const", asMETHOD(Entity, GetParent), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ AddChild(const string &in)", asMETHOD(Entity, AddChild), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ InstantiateScene(const string &in)", asMETHOD(Entity, InstantiateScene), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "bool IsScene() const", asMETHOD(Entity, IsScene), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ GetChild(const string &in) const", asMETHOD(Entity, GetChild), asCALL_THISCALL);
    
    // Register components.
    engine->SetDefaultNamespace("Component");
    
    engine->RegisterObjectType("DirectionalLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("DirectionalLight", "vec3 color", asOFFSET(DirectionalLight, color));
    engine->RegisterObjectProperty("DirectionalLight", "float ambientCoefficient", asOFFSET(DirectionalLight, ambientCoefficient));
    
    engine->RegisterObjectType("Lens", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Lens", "float fieldOfView", asOFFSET(Lens, fieldOfView));
    engine->RegisterObjectProperty("Lens", "float zNear", asOFFSET(Lens, zNear));
    engine->RegisterObjectProperty("Lens", "float zFar", asOFFSET(Lens, zFar));
    
    engine->RegisterObjectType("Listener", 0, asOBJ_REF | asOBJ_NOCOUNT);
    
    engine->RegisterObjectType("Physics", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Physics", "vec3 velocity", asOFFSET(Physics, velocity));
    engine->RegisterObjectProperty("Physics", "float maxVelocity", asOFFSET(Physics, maxVelocity));
    engine->RegisterObjectProperty("Physics", "vec3 angularVelocity", asOFFSET(Physics, angularVelocity));
    engine->RegisterObjectProperty("Physics", "float maxAngularVelocity", asOFFSET(Physics, maxAngularVelocity));
    engine->RegisterObjectProperty("Physics", "vec3 acceleration", asOFFSET(Physics, acceleration));
    engine->RegisterObjectProperty("Physics", "vec3 angularAcceleration", asOFFSET(Physics, angularAcceleration));
    engine->RegisterObjectProperty("Physics", "float velocityDragFactor", asOFFSET(Physics, velocityDragFactor));
    engine->RegisterObjectProperty("Physics", "float angularDragFactor", asOFFSET(Physics, angularDragFactor));
    engine->RegisterObjectProperty("Physics", "float gravityFactor", asOFFSET(Physics, gravityFactor));
    engine->RegisterObjectProperty("Physics", "vec3 momentOfInertia", asOFFSET(Physics, momentOfInertia));
    
    engine->RegisterObjectType("PointLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("PointLight", "vec3 color", asOFFSET(PointLight, color));
    engine->RegisterObjectProperty("PointLight", "float ambientCoefficient", asOFFSET(PointLight, ambientCoefficient));
    engine->RegisterObjectProperty("PointLight", "float attenuation", asOFFSET(PointLight, attenuation));
    engine->RegisterObjectProperty("PointLight", "float intensity", asOFFSET(PointLight, intensity));
    
    engine->RegisterObjectType("SpotLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("SpotLight", "vec3 color", asOFFSET(SpotLight, color));
    engine->RegisterObjectProperty("SpotLight", "float ambientCoefficient", asOFFSET(SpotLight, ambientCoefficient));
    engine->RegisterObjectProperty("SpotLight", "float attenuation", asOFFSET(SpotLight, attenuation));
    engine->RegisterObjectProperty("SpotLight", "float intensity", asOFFSET(SpotLight, intensity));
    engine->RegisterObjectProperty("SpotLight", "float coneAngle", asOFFSET(SpotLight, coneAngle));
    
    engine->RegisterObjectType("SoundSource", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("SoundSource", "float pitch", asOFFSET(SoundSource, pitch));
    engine->RegisterObjectProperty("SoundSource", "float gain", asOFFSET(SoundSource, gain));
    engine->RegisterObjectProperty("SoundSource", "bool loop", asOFFSET(SoundSource, loop));
    engine->RegisterObjectMethod("SoundSource", "void Play()", asMETHOD(SoundSource, Play), asCALL_THISCALL);
    engine->RegisterObjectMethod("SoundSource", "void Pause()", asMETHOD(SoundSource, Pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("SoundSource", "void Stop()", asMETHOD(SoundSource, Stop), asCALL_THISCALL);
    
    engine->SetDefaultNamespace("");
    
    // Register adding and getting components..
    engine->RegisterObjectMethod("Entity", "Component::DirectionalLight@ AddDirectionalLight()", asMETHODPR(Entity, AddComponent, (), DirectionalLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::DirectionalLight@ GetDirectionalLight()", asMETHODPR(Entity, GetComponent, (), DirectionalLight*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::Lens@ AddLens()", asMETHODPR(Entity, AddComponent, (), Lens*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Lens@ GetLens()", asMETHODPR(Entity, GetComponent, (), Lens*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::Listener@ AddListener()", asMETHODPR(Entity, AddComponent, (), Listener*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Listener@ GetListener()", asMETHODPR(Entity, GetComponent, (), Listener*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::Physics@ AddPhysics()", asMETHODPR(Entity, AddComponent, (), Physics*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Physics@ GetPhysics()", asMETHODPR(Entity, GetComponent, (), Physics*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::PointLight@ AddPointLight()", asMETHODPR(Entity, AddComponent, (), PointLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::PointLight@ GetPointLight()", asMETHODPR(Entity, GetComponent, (), PointLight*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::SpotLight@ AddSpotLight()", asMETHODPR(Entity, AddComponent, (), SpotLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::SpotLight@ GetSpotLight()", asMETHODPR(Entity, GetComponent, (), SpotLight*), asCALL_THISCALL);
    
    engine->RegisterObjectMethod("Entity", "Component::SoundSource@ AddSoundSource()", asMETHODPR(Entity, AddComponent, (), SoundSource*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::SoundSource@ GetSoundSource()", asMETHODPR(Entity, GetComponent, (), SoundSource*), asCALL_THISCALL);
    
    // Register managers.
    engine->RegisterObjectType("DebugDrawingManager", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddPoint(const vec3 &in, const vec3 &in, float, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddPoint), asCALL_THISCALL);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddLine(const vec3 &in, const vec3 &in, const vec3 &in, float = 1.0, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddLine), asCALL_THISCALL);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddAxisAlignedBoundingBox(const vec3 &in, const vec3 &in, const vec3 &in, float = 1.0, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddAxisAlignedBoundingBox), asCALL_THISCALL);
    
    engine->RegisterObjectType("Hub", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Hub", "DebugDrawingManager@ debugDrawingManager", asOFFSET(Hub, debugDrawingManager));
    
    // Register functions.
    engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
    engine->RegisterGlobalFunction("Entity@ GetEntity()", asFUNCTION(GetEntity), asCALL_CDECL);
    engine->RegisterGlobalFunction("void RegisterUpdate()", asFUNCTION(::RegisterUpdate), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool Input(input button)", asFUNCTION(Input), asCALL_CDECL);
    engine->RegisterGlobalFunction("void SendMessage(Entity@, int)", asFUNCTION(::SendMessage), asCALL_CDECL);
    engine->RegisterGlobalFunction("Hub@ Managers()", asFUNCTION(Managers), asCALL_CDECL);
}

ScriptManager::~ScriptManager() {
    engine->ShutDownAndRelease();
}

void ScriptManager::BuildScript(const std::string& name) {
    std::string filename = Hymn().GetPath() + FileSystem::DELIMITER + "Scripts" + FileSystem::DELIMITER + name + ".as";
    if (!FileSystem::FileExists(filename.c_str())) {
        Log() << "Script file does not exist: " << filename << "\n";
        return;
    }
    
    // Create and build script module.
    CScriptBuilder builder;
    int r = builder.StartNewModule(engine, name.c_str());
    if (r < 0)
        Log() << "Couldn't start new module: " << name << ".\n";
    
    r = builder.AddSectionFromFile(filename.c_str());
    if (r < 0)
        Log() << "File section could not be added: " << filename << ".\n";
    
    r = builder.BuildModule();
    if (r < 0)
        Log() << "Compile errors.\n";
}

void ScriptManager::BuildAllScripts() {
    std::string path = Hymn().GetPath() + FileSystem::DELIMITER + "Scripts" + FileSystem::DELIMITER;
    
    for (ScriptFile* file : Hymn().scripts) {
        std::string filename = path + file->name + ".as";
        if (!FileSystem::FileExists(filename.c_str())) {
            Log() << "Script file does not exist: " << filename << "\n";
            return;
        }
        
        // Create and build script module.
        CScriptBuilder builder;
        asIScriptModule* module = engine->GetModule(file->name.c_str());
        if (module == nullptr) {
            int r = builder.StartNewModule(engine, file->name.c_str());
            if (r < 0)
                Log() << "Couldn't start new module: " << file->name << ".\n";
            
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
    }
}

void ScriptManager::Update(World& world) {
    // Init.
    for (Script* script : world.GetComponents<Script>()) {
        if (!script->initialized) {
            CreateInstance(script);
            script->initialized = true;
        }
    }
    
    // Update.
    for (Entity* entity : world.GetUpdateEntities())
        CallScript(entity->GetComponent<Script>(), "void Update()");
    
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
            engine->RegisterEnumValue("input", std::string(button->action).c_str(), i);
    }
}

void ScriptManager::SendMessage(Entity* recipient, int type) {
    Message message;
    message.recipient = recipient;
    message.type = type;
    messages.push_back(message);
}

void ScriptManager::CreateInstance(Component::Script* script) {
    currentEntity = script->entity;
    ScriptFile* scriptFile = script->scriptFile;
    
    // Find the class to instantiate.
    asITypeInfo* type = GetClass(scriptFile->name, scriptFile->name);
    
    // Find factory function / constructor.
    std::string factoryName = scriptFile->name + "@ " + scriptFile->name + "(Entity@)";
    asIScriptFunction* factoryFunction = type->GetFactoryByDecl(factoryName.c_str());
    if (factoryFunction == nullptr)
        Log() << "Couldn't find the factory function for " << scriptFile->name << ".\n";
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(factoryFunction);
    context->SetArgObject(0, script->entity);
    ExecuteCall(context);
    
    // Get the newly created object.
    script->instance = *(static_cast<asIScriptObject**>(context->GetAddressOfReturnValue()));
    script->instance->AddRef();
    
    // Clean up.
    context->Release();
}

void ScriptManager::CallScript(Component::Script* script, const std::string& functionName) {
    currentEntity = script->entity;
    ScriptFile* scriptFile = script->scriptFile;
    
    // Get class.
    asITypeInfo* type = GetClass(scriptFile->name, scriptFile->name);
    
    // Find method to call.
    asIScriptFunction* method = type->GetMethodByDecl(functionName.c_str());
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(method);
    context->SetObject(script->instance);
    ExecuteCall(context);
    
    // Clean up.
    context->Release();
}

void ScriptManager::CallMessageReceived(const Message& message) {
    currentEntity = message.recipient;
    Component::Script* script = currentEntity->GetComponent<Component::Script>();
    ScriptFile* scriptFile = script->scriptFile;
    
    // Get class.
    asITypeInfo* type = GetClass(scriptFile->name, scriptFile->name);
    
    // Find method to call.
    asIScriptFunction* method = type->GetMethodByDecl("void ReceiveMessage(int)");
    if (method == nullptr)
        Log() << "Can't find method void ReceiveMessage(int)\n";
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(method);
    context->SetObject(script->instance);
    context->SetArgDWord(0, message.type);
    ExecuteCall(context);
    
    // Clean up.
    context->Release();
}

void ScriptManager::LoadScriptFile(const char* fileName, std::string& script){
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

void ScriptManager::ExecuteCall(asIScriptContext* context) {
    int r = context->Execute();
    if (r != asEXECUTION_FINISHED) {
        // The execution didn't complete as expected. Determine what happened.
        if (r == asEXECUTION_EXCEPTION) {
            // An exception occurred, let the script writer know what happened so it can be corrected.
            Log() << "An exception '" << context->GetExceptionString() << "' occurred. Please correct the code and try again.\n";
        }
    }
}

asITypeInfo* ScriptManager::GetClass(const std::string& moduleName, const std::string& className) {
    // Get script module.
    asIScriptModule* module = engine->GetModule(moduleName.c_str(), asGM_ONLY_IF_EXISTS);
    if (module == nullptr)
        Log() << "Couldn't find \"" << moduleName << "\" module.\n";
    
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
