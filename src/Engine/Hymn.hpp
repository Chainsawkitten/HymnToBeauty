#pragma once

#include <string>

/// A hymn to beauty.
class ActiveHymn {
    public:
        /// Get the instance of the class.
        /**
         * @return The %ActiveHymn instance
         */
        static ActiveHymn& GetInstance();
        
        /// Clear the hymn of all properties.
        void Clear();
        
        /// Get the path where the hymn is saved.
        /**
         * @return The hymn's path.
         */
        const std::string& Path() const;
        
        /// Set the path where the hymn is saved.
        /**
         * @param path New path.
         */
        void SetPath(const std::string& path);
        
    private:
        ActiveHymn();
        ActiveHymn(ActiveHymn const&) = delete;
        void operator=(ActiveHymn const&) = delete;
        
        std::string path;
};

/// Get the active hymn.
/**
 * @return The %ActiveHymn instance.
 */
ActiveHymn& Hymn();
