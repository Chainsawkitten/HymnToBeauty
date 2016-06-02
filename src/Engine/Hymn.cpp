#include "Hymn.hpp"

ActiveHymn::ActiveHymn() {
    Clear();
}

ActiveHymn& ActiveHymn::GetInstance() {
    static ActiveHymn ActiveHymn;
    
    return ActiveHymn;
}

void ActiveHymn::Clear() {
    
}

ActiveHymn& Hymn() {
    return ActiveHymn::GetInstance();
}
