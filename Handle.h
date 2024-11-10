#pragma once

// RAII compliant wrapper of Win32 handles

namespace libreg
{
    template <typename T>
    class Handle
    {
    public:
        Handle(T val);
        ~Handle();

        Handle(Handle<T>&& other);

        Handle(const Handle<T>&) = delete;
        const Handle<T>& operator=(Handle<T>&& other);
        const Handle<T>& operator=(const Handle&) = delete;

        T Get() const;
        T* Addr();
        void Detach();
        void Close();
        operator T() const;
        bool Good() const;

    private:
        T _value;
    };
}
#include "handle.hxx"