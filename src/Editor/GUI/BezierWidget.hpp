#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <imgui.h>

class Curve {
    public:
        /// Constructor.
        Curve();
        std::vector<glm::vec2> points;
    private:
};

class BezierWidget {
    public:
        /// Constructor.
        BezierWidget(ImVec2 size, Curve& curve);

        /// Draws a bezier widget.
        void show();
    private:
        Curve* curve;
        ImVec2 size;
};
