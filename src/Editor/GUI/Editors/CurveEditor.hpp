#pragma once

#include <imgui.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <cmath>
#include <vector>
#include <Video/ParticleSystemRenderer.hpp>

struct MyCurve {
    std::string curve_name;
    float maxTime = 1.0f;
    ImVec2 value[10];
    ImGuiID id;
    int item = 0;
    float value_you_care_about = 0.0f;

    // Variables for controlling particle.
    bool editVelocityX = false;
    bool editVelocityY = false;
    bool editVelocityZ = false;
};

/// Graphical editor for curves.
class CurveEditor {
    public:
        /// Constructor.
        CurveEditor();

        /// Destructor.
        ~CurveEditor();

        /// Show the editor.
        void Show();

        /// If editor is visible.
        bool IsVisible() const;

        /// Set visibility.
        /**
         * @param visible setting visibility on editor.
         */
        void SetVisible(bool visible);

        /// Adds a curve.
        /**
         * @param curve_name name of curve.
         * @param uniqueId unique ID.
         * @param item unique ID just set it to 0.
         */
        void AddMyCurve(const std::string& curve_name, ImGuiID uniqueId, int item);

        /// Updates all curves.
        /**
         * @param deltaTime deltatime.
         * @param totalTime total time for the curves.
         */
        void UpdateCurves(float deltaTime, float totalTime);

        /// Render the curve editor.
        void RenderCurveEditor();

        /// Get all curves.
        /**
         * @return All curves.
         */
        const std::vector<MyCurve>& GetAllCurves() const;

    private:
        bool visible = false;
        ImGuiID addedCurve = 0;
        std::string curvename = "Default";
        char curveBuf[10];
        std::string editor_name;
        std::vector<MyCurve> curves;
        float time = 0.0f;
        Video::ParticleSystemRenderer::EmitterSettings emitterSettings;
        bool goBack = false;
        bool play = false;
};
