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
}

const string& ActiveHymn::Path() const {
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

ActiveHymn& Hymn() {
    return ActiveHymn::GetInstance();
}
