#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <imgui.h>
///@TODO:   Separate Curve into separate class with a given ease function.
///         Rename BezierWidget to CurveWidget.
///         Make the widget draw the curve using ease function.
class Curve {
    public:
        /// Constructor.
        Curve();
        
        /// Adds a points to the curve.
        /**
         * @param newPoint the new point.
         */
        void AddPoint(glm::vec2 newPoint);
        
        /// Gets the index of the point that is closest to the given input point.
        /**
         * @param point The point we want to check against.
         * @return The index of the closest point.
         */
        int GetClosestPointIndex(glm::vec2 point);
        
        /// Stick the rightmost and leftmost points to the left and right edges of the curve.
        void StickToEdges();
        
        /// Sorts the curves points by x value.
        void Sort();
        
        /// All our curve points.
        std::vector<glm::vec2> points;
};

class BezierWidget {
    public:
        /// Constructor.
        BezierWidget(ImVec2 size, Curve* curve);

        /// Draws a bezier widget.
        void Show();
    private:
        Curve* curve;
        ImVec2 size;
        bool draggingPoint = false;
};
