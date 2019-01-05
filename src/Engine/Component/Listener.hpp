#pragma once

#include "SuperComponent.hpp"

namespace Component {
/// %Component describing a listener that can listen to sounds.
class Listener : public SuperComponent {
  public:
    /// Create new listener.
    Listener();

    Json::Value Save() const override;
};
} // namespace Component
