#pragma once
namespace Core {
    //Provides a simple pointer wrapper that does null checks
    template <typename T>
    class Ptr {
    public:
        Ptr() : ptr {nullptr} {}
        Ptr(Ptr && ) = default;
        Ptr(Ptr const &) = default;
        Ptr(T * o) : ptr(o) {}
        Ptr & operator=(Ptr const &)= default;
        T * operator->() const {
            Check();
            return ptr;
        }
        Ptr & operator++() {
            return ++ptr, *this;
        }
        T & operator[](size_t o) const {
            Check();
            return ptr[o];
        }
        operator T *() const {
            return Get();
        }
        T * Get() const {
            Check();
            return ptr;
        }
        void Check() const {
            if (!ptr) {
                /*Panic("Null Pointer Access");*/
            }
        }
    private:
        T * ptr;
    };
}