#include "Material.hpp"

#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Texture/Texture2D.hpp"
#include "DefaultDiffuse.png.hpp"
#include "DefaultNormal.png.hpp"
#include "DefaultSpecular.png.hpp"
#include "DefaultGlow.png.hpp"

using namespace Component;

Material::Material(Entity* entity) : SuperComponent(entity) {
    diffuse = Managers().resourceManager->CreateTexture2D(DEFAULTDIFFUSE_PNG, DEFAULTDIFFUSE_PNG_LENGTH, true);
    normal = Managers().resourceManager->CreateTexture2D(DEFAULTNORMAL_PNG, DEFAULTNORMAL_PNG_LENGTH);
    specular = Managers().resourceManager->CreateTexture2D(DEFAULTSPECULAR_PNG, DEFAULTSPECULAR_PNG_LENGTH);
    glow = Managers().resourceManager->CreateTexture2D(DEFAULTGLOW_PNG, DEFAULTGLOW_PNG_LENGTH);
}

Material::~Material() {
    Managers().resourceManager->FreeTexture2D(diffuse);
    Managers().resourceManager->FreeTexture2D(normal);
    Managers().resourceManager->FreeTexture2D(specular);
    Managers().resourceManager->FreeTexture2D(glow);
}

void Material::SetDiffuse(const char* filename) {
    if (diffuse != nullptr)
        Managers().resourceManager->FreeTexture2D(diffuse);
    
    diffuse = Managers().resourceManager->CreateTexture2DFromFile(filename, true);
}

void Material::SetNormal(const char* filename) {
    if (normal != nullptr)
        Managers().resourceManager->FreeTexture2D(normal);
    
    normal = Managers().resourceManager->CreateTexture2DFromFile(filename);
}

void Material::SetSpecular(const char* filename) {
    if (specular != nullptr)
        Managers().resourceManager->FreeTexture2D(specular);
    
    specular = Managers().resourceManager->CreateTexture2DFromFile(filename);
}

void Material::SetGlow(const char* filename) {
    if (glow != nullptr)
        Managers().resourceManager->FreeTexture2D(glow);
    
    glow = Managers().resourceManager->CreateTexture2DFromFile(filename);
}
