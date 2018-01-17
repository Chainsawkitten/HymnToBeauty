#pragma once

#include "../Resources.hpp"

namespace GUI {
    /// Used to select a resource from the resource list.
    class ResourceSelector {
        public:
            /// Information about the selected resource.
            struct SelectedResource {
                /// Get path (including name).
                /**
                 * @return The path to the resource.
                 */
                std::string GetPath() const;
                
                /// The selected resource.
                const ResourceList::Resource* resource;
                
                /// The path to the resource.
                std::string path;
            };
            
            /// Default constructor.
            ResourceSelector() = default;
            
            /// Show the resource selector and let the user select a resource.
            /**
             * @param type The type of resource to select.
             */
            bool Show(ResourceList::Resource::Type type);
            
            /// Get the selected resource.
            /**
             * @return The selected resource.
             */
            const SelectedResource& GetSelectedResource() const;
            
        private:
            bool ShowHelper(ResourceList::Resource::Type type, const ResourceList::ResourceFolder& folder, const std::string& path);
            
            SelectedResource selectedResource;
    };
}
