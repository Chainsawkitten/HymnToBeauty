#pragma once

#include <string>
#include "Scene/Scene.hpp"

/// A hymn to beauty.
class ActiveHymn {
    friend ActiveHymn& Hymn();
    
    public:
        /// Clear the hymn of all properties.
        void Clear();
        
        /// Get the path where the hymn is saved.
        /**
         * @return The hymn's path.
         */
        const std::string& GetPath() const;
        
        /// Set the path where the hymn is saved.
        /**
         * @param path New path.
         */
        void SetPath(const std::string& path);
        
        /// Load a hymn.
        /**
         * @param path Path to the saved hymn.
         */
        void Load(const std::string& path);
        
        /// Render the active scene.
        void Render();
        
    private:
        static ActiveHymn& GetInstance();
        
        ActiveHymn();
        ActiveHymn(ActiveHymn const&) = delete;
        void operator=(ActiveHymn const&) = delete;
        
        std::string path;
        
        Scene activeScene;
};

/// Get the active hymn.
/**
 * @return The %ActiveHymn instance.
 */
ActiveHymn& Hymn();
