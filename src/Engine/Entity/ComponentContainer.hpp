#pragma once

#include <functional>
#include <vector>

/// Stores components.
template <class C> class ComponentContainer {
  public:
    /// Create new component container.
    ComponentContainer();

    /// Destructor.
    ~ComponentContainer();

    /// Create a new component.
    /**
     * @return The created component.
     */
    C* Create();

    /// Clear all killed components.
    void ClearKilled();

    /// Clear all killed components, allowing the caller to do work with
    /// the component before it is removed.
    /**
     * @param preRemove A function that will be called for each killed
            component before it is removed, passing the component itself.
     */
    void ClearKilled(const std::function<void(C*)>& preRemove);

    /// Get all components.
    /**
     * @return All of the components in the container.
     */
    const std::vector<C*>& GetAll() const;

  private:
    std::vector<C*> components;
};

template <class C> ComponentContainer<C>::ComponentContainer() {}

template <class C> ComponentContainer<C>::~ComponentContainer() {
    for (C* component : components)
        delete component;
}

template <class C> C* ComponentContainer<C>::Create() {
    C* component = new C();
    components.push_back(component);
    return component;
}

template <class C> void ComponentContainer<C>::ClearKilled() {
    ClearKilled([](C*) {});
}

template <class C> void ComponentContainer<C>::ClearKilled(const std::function<void(C*)>& preRemove) {
    // Clear killed components.
    std::size_t i = 0;
    while (i < components.size()) {
        if (components[i]->IsKilled()) {
            preRemove(components[i]);
            delete components[i];
            components[i] = components[components.size() - 1];
            components.pop_back();
        } else {
            ++i;
        }
    }
}

template <class C> const std::vector<C*>& ComponentContainer<C>::GetAll() const {
    return components;
}
