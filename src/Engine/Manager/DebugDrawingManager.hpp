#pragma once

/// Debug drawing facilities.
class DebugDrawingManager {
    friend class Hub;
    
    private:
        DebugDrawingManager();
        ~DebugDrawingManager();
        DebugDrawingManager(DebugDrawingManager const&) = delete;
        void operator=(DebugDrawingManager const&) = delete;
};
