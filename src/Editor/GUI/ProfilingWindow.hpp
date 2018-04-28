#pragma once

#include <Engine/Manager/ProfilingManager.hpp>
#include <GUI/LogView.hpp>

namespace GUI {
    class ProfilingWindow {
        public:
            /// Show profiling results.
            void Show();

        private:
            LogView logView;
            void ShowResult(ProfilingManager::Result* result);
    };
}
