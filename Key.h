#pragma once

#include <vector>
#include <memory>
#include "MultiString.h"
#include "Handle.h"
#include "Hive.h"
#include "Access.h"
#include "ValueType.h"

namespace libreg
{
    class Key
    {
    public:
        static Key Open(libreg::Hive hive, const MultiString& path, Access access);
        static Key Create(libreg::Hive hive, const MultiString& path, Access access, bool volatile_key = false);
        static Key OpenOrCreate(libreg::Hive hive, const MultiString& path, Access access, bool volatile_key = false);
        static void QueryInfo(const Handle<HKEY>& key, size_t* max_subkey_chars, size_t* max_value_name_chars, size_t* max_data_bytes);

        const Handle<HKEY>& NativeHandle() const;
        const MultiString& Path() const;
        Hive Hive() const;

        std::vector<Key> SubKeys(Access access = Access::read) const;
        std::vector<std::pair<MultiString, ValueType>> Values() const;
        Key OpenSubKey(const MultiString& name, Access access) const;

        void SetValue(const MultiString& name, DWORD value, ValueType type);
        void SetValue(const MultiString& name, const std::vector<std::uint8_t>& value, ValueType type);
        void SetValue(const MultiString& name, const MultiString& value, ValueType type);

        template <typename T>
        T GetValue(const MultiString& name, ValueType expected_type = ValueType::None);

        void Delete();
        void DeleteValue(const MultiString& name);

    private:
        Key(Handle<HKEY>&& handle, libreg::Hive hive, const MultiString& path);

        void SetValueImpl(const MultiString& name, const void* data, size_t size, ValueType type);
        void GetValueImpl(const MultiString& name, void* data, size_t& size, ValueType type);

        std::shared_ptr<Handle<HKEY>> _handle;
        libreg::Hive _hive;
        MultiString _path;
    };
}

#include "Key.hxx"