#pragma once

/// A hymn to beauty.
class Hymn {
    public:
        /// Get the instance of the class.
        /**
         * @return The %Hymn instance
         */
        static Hymn& GetInstance();
        
        /// Clear the hymn of all properties.
        void Clear();
        
    private:
        Hymn();
        Hymn(Hymn const&) = delete;
        void operator=(Hymn const&) = delete;
};
