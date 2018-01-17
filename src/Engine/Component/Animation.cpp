#include "Animation.hpp"

#include "../Hymn.hpp"
#include "../Geometry/Model.hpp"

Component::Animation::Animation() {
    
}

Json::Value Component::Animation::Save() const {
    Json::Value component;
    if (riggedModel != nullptr)
        component["riggedModel"] = riggedModel->path + riggedModel->name;
    else
        component["riggedModel"] = "null";
    return component;
}
