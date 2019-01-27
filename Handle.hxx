#pragma once

#include <Windows.h>
#include <type_traits>
#include "Handle.h"

namespace libreg
{
    template <typename T>
    inline Handle<T>::Handle(T hVal) : _value(hVal)
    {
    }

    template <typename T>
    inline Handle<T>::Handle(Handle<T>&& other) :
        _value(other._value)
    {
        other.Detach();
    }

    template <typename T>
    inline const Handle<T>& Handle<T>::operator=(Handle<T>&& other)
    {
      _value = other._value;
      other.Detach();
    }

        template <typename T>
    inline void Handle<T>::Close()
    {
        static_assert(false, "Incorrect specialization of Handle::Close");
    }

    template <>
    inline void Handle<HKEY>::Close()
    {
        RegCloseKey(_value);
        Detach();
    }

    template <>
    inline void Handle<HANDLE>::Close()
    {
        CloseHandle(_value);
        Detach();
    }

    template <typename T>
    inline Handle<T>::~Handle()
    {
        if (Good())
        {
            Close();
        }
    }

    template <typename T>
    inline Handle<T>::operator T() const
    {
        return _value;
    }

    template <typename T>
    inline T Handle<T>::Get() const
    {
        return _value;
    }

    template <typename T>
    inline void Handle<T>::Detach()
    {
        _value = nullptr;
    }

    template <typename T>
    inline bool Handle<T>::Good() const
    {
        return _value != nullptr;
    }
}