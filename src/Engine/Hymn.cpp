#include "Hymn.hpp"

#include "Util/FileSystem.hpp"

using namespace std;

ActiveHymn::ActiveHymn() {
    Clear();
}

ActiveHymn& ActiveHymn::GetInstance() {
    static ActiveHymn ActiveHymn;
    
    return ActiveHymn;
}

void ActiveHymn::Clear() {
    path = "";
    activeScene.Clear();
}

const string& ActiveHymn::GetPath() const {
    return path;
}

void ActiveHymn::SetPath(const string& path) {
    this->path = path;
    FileSystem::CreateDirectory(path.c_str());
}

void ActiveHymn::Load(const string& path) {
    Clear();
    this->path = path;
}

void ActiveHymn::Render() {
    
}

ActiveHymn& Hymn() {
    return ActiveHymn::GetInstance();
}
