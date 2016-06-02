#pragma once

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
        
    private:
        ActiveHymn();
        ActiveHymn(ActiveHymn const&) = delete;
        void operator=(ActiveHymn const&) = delete;
};

/// Get the active hymn.
/**
 * @return The %ActiveHymn instance.
 */
ActiveHymn& Hymn();
