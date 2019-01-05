#pragma once

#include <functional>
#include <memory>

namespace Utility {
template <typename T> class LockBox;
template <typename T> void swap(LockBox<T>& first, LockBox<T>& second);

/// Restricts access to data by protecting it with a key. Only those with
/// the correct key may access the data, allowing the user to define the
/// set of owners according to how the key is distributed. Others may keep
/// a reference to the data by holding a lockbox, but without the key they
/// have no way to be granted access to the inner contents, and must turn
/// to an owner to work with it instead.
template <typename T> class LockBox {
  public:
    /// Control object used to only grant access to objects with a key
    /// matching that of the LockBox.
    class Key {};

    friend void swap<T>(LockBox<T>& first, LockBox<T>& second);

    /// Constructor that defaults to referencing no data.
    LockBox() = default;

    /// Constructor.
    /**
     * @param key A key that is allowed to open the box.
     * @param data Data that will be wrapped by the LockBox. This raw
     * pointer is not copied or otherwise managed by LockBox, and it's
     * the responsibility of the owner to make sure the pointer is
     * valid for as long as the box is used.
     */
    LockBox(std::shared_ptr<Key> key, T* data);

    /// Make a new reference to an existing lockbox.
    /**
     * @param other Existing LockBox whose data is referenced.
     */
    LockBox(const LockBox<T>& other);

    /// Move an existing reference into a new LockBox, invalidating
    /// the old one.
    /**
     * @param other Existing LockBox to move reference from.
     */
    LockBox(LockBox<T>&& other);

    /// Replace the currently held reference with a new one.
    /**
     * @param rhs LockBox with the new reference to hold.
     * @return Reference to the LockBox being assigned to.
     */
    LockBox<T>& operator=(LockBox<T> rhs);

    /// Destroys a LockBox, increasing the reference count. Note that
    /// the wrapped object is not deleted since the box is only a thin
    /// wrapper.
    ~LockBox();

    /// Open a box to access underlying contents.
    /**
     * @param key A key that must match the box.
     * @param forValidKey If |key| is valid, this function is called,
     * passing a reference to the box contents.
     * @return True if the key matched, false otherwise.
     */
    bool Open(std::shared_ptr<Key> key, const std::function<void(T&)>& forValidKey);

    /// Number of lockboxes with the same reference.
    /**
     * @return Reference count.
     */
    uint32_t RefCount() const;

  private:
    struct Shared {
        uint32_t refs = 0;
        std::shared_ptr<Key> key = std::shared_ptr<Key>(nullptr);
        T* data = nullptr;
    };

    Shared* shared = nullptr;
};

/// Swap two lockboxes with each other.
/**
 * @param first First lockbox to swap.
 * @param second Second lockbox to swap.
 */
template <typename T> void swap(LockBox<T>& first, LockBox<T>& second) {
    using std::swap;
    swap(first.shared, second.shared);
}

template <typename T> LockBox<T>::LockBox(std::shared_ptr<Key> key, T* data) {
    shared = new Shared;
    shared->refs = 1;
    shared->key = key;
    shared->data = data;
}

template <typename T> LockBox<T>::LockBox(const LockBox<T>& other) {
    shared = other.shared;
    shared->refs++;
}

template <typename T> LockBox<T>::LockBox(LockBox<T>&& other) : LockBox() {
    swap(*this, other);
}

// Why not placement new: https://stackoverflow.com/a/7178022
// Copy-and-swap idiom: https://stackoverflow.com/a/3279550
template <typename T> LockBox<T>& LockBox<T>::operator=(LockBox<T> rhs) {
    swap(*this, rhs);
    return *this;
}

template <typename T> LockBox<T>::~LockBox() {
    if (shared && --shared->refs == 0) {
        delete shared;
    }
}

template <typename T> bool LockBox<T>::Open(std::shared_ptr<Key> key, const std::function<void(T&)>& forValidKey) {
    if (shared && key.get() == shared->key.get()) {
        forValidKey(*shared->data);
        return true;
    }
    return false;
}

template <typename T> uint32_t LockBox<T>::RefCount() const {
    return shared ? shared->refs : 0;
}
} // namespace Utility
