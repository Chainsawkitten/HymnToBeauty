#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "Widget.hpp"

/** @ingroup Core
 * @{
 */

namespace GUI {
    /// A container that contains Widgets.
    class Container : public Widget {
        public:
            /// Create a new container.
            /**
             * @param parent Parent widget.
             */
            Container(Widget* parent);
            
            /// Destructor.
            virtual ~Container();
    };
}

/** @} */

#endif
