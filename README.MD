# Libreg

A modern, intuitive and simple C++ library to interact with the Windows registry

## Build from source

Run:
```
$ msbuild libreg.sln
```

The build output can be found in the `lib` folder.

## Usage


```C++

#include <libreg/Key.h>
#include <iostream>

void Example()
{
	// Open or create a key
	auto key = libreg::Key::OpenOrCreate(libreg::Hive::CurrentUser, "Software\\libreg", Access::AllAccess);
	
	// Set a value
	key.SetValue("foo", "bar", ValueType::Sz);
	
	// Create a key
	auto subkey  = key.CreateSubKey("sub-key");
	
	// Create a volatile sub-key*	
	auto volatile_subkey  = key.CreateSubKey("volatile-sub-key", true);


	// Read a value
	auto value = key.GetValue<libreg::MultiString>("foo");
	std::cout << "Value: " << value << std::endl;
	
	// List sub-keys in a key
	for (const auto &e: key.SubKeys())
	{
		std::cout << "Subkey: " << e.Path() << std::endl;
	}
	
	// List values in a key
	for (const auto &e: key.Values())
	{
		std::cout << "Name: " << e.first << ", type: " << e.second << std::endl;
	}
	
	// Delete a value
	key.DeleteValue("foo");
	
	// Delete a key
	key.DeleteSubKey("sub-key");
}