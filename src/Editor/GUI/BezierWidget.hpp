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
        
        /// Curve types.
        enum CurveType{
            LINEAR,
            QUADIN,          // t^2
            QUADOUT,
            QUADINOUT,
            CUBICIN,         // t^3
            CUBICOUT,
            CUBICINOUT,
            QUARTIN,         // t^4
            QUARTOUT,
            QUARTINOUT,
            QUINTIN,         // t^5
            QUINTOUT,
            QUINTINOUT,
            SINEIN,          // sin(t)
            SINEOUT,
            SINEINOUT,
            EXPOIN,          // 2^t
            EXPOOUT,
            EXPOINOUT,
            CIRCIN,          // sqrt(1-t^2)
            CIRCOUT,
            CIRCINOUT,
            ELASTICIN,       // exponentially decaying sine wave
            ELASTICOUT,
            ELASTICINOUT,
            BACKIN,          // overshooting cubic easing: (s+1)*t^3 - s*t^2
            BACKOUT,
            BACKINOUT,
            BOUNCEIN,        // exponentially decaying parabolic bounce
            BOUNCEOUT,
            BOUNCEINOUT,
            
            SINESQUARE,      // gapjumper's
            EXPONENTIAL,     // gapjumper's
            SCHUBRING1,      // terry schubring's formula 1
            SCHUBRING2,      // terry schubring's formula 2
            SCHUBRING3,      // terry schubring's formula 3
            
            SINPI2,          // tomas cepeda's
            SWING,           // tomas cepeda's & lquery's
        } curveType = CurveType::QUADINOUT;
        
        /// Adds a points to the curve.
        /**
         * @param newPoint the new point.
         */
        void AddPoint(glm::vec2 newPoint);
        
        /// Samples a value in the curve.
        /**
         * @param x The point in the curve which will be sampled.
         * @return The sampled value.
         */
        float Sample(float x);
        
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
        /// Increase this to increase the resolution of the graph.
        static int maximumNumberOfCurveLines;
        
        Curve* curve;
        
        ImVec2 size;
        
        bool draggingPoint = false;
};
