#pragma once

#include <Video/Geometry/Geometry3D.hpp>
#include <json/json.h>

namespace Geometry {
/// Interface of a model loaded from a file.
class Model : public Video::Geometry::Geometry3D {
  public:
    /// Create new empty model.
    /**
     * The created model has to be loaded later using Load.
     *
     * @param lowLevelRenderer The low-level renderer to use
     */
    explicit Model(Video::LowLevelRenderer* lowLevelRenderer);

    /// Destructor.
    virtual ~Model();

    /// Save the model.
    /**
     * @return JSON value to be stored on disk.
     */
    Json::Value Save() const;

    /// Load model from file.
    /**
     * @param name Name of the model.
     */
    void Load(const std::string& name);

    /// The name of the model.
    std::string name;

    /// The folder containing the model file.
    std::string path;

  private:
    void LoadGeometry(const char* filename);
};
} // namespace Geometry
