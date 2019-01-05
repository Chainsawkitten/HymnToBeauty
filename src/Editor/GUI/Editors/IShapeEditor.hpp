#pragma once

namespace Component {
class Shape;
}

namespace Physics {
class Shape;
}

namespace GUI {

/// Interface to editors of physics shapes.
class IShapeEditor {
  public:
    /// Destructor.
    virtual ~IShapeEditor() {}

    /// Get a string with the kind of shape that's being edited. This
    /// is intended to be used as a label in the GUI.
    /**
     * @return The type of shape as a string.
     */
    virtual const char* Label() const = 0;

    /// Displays the editor.
    /**
     * @param comp The shape component to edit.
     */
    virtual void Show(Component::Shape* comp) = 0;

    /// Set the shape of a shape component.
    /**
     * @param comp The shape component on which to set shape.
     */
    virtual void Apply(Component::Shape* comp) = 0;

    /// Set internal data according to an existing shape. If the shape
    /// does not correspond to the actual editor, nothing happens.
    /**
     * @param shape The %Shape from which to initialize data.
     * @return True if shape matched editor type, false otherwise.
     */
    virtual bool SetFromShape(const Physics::Shape& shape) = 0;
};

} // namespace GUI
