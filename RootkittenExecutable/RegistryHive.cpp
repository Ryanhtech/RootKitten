#include "RootKitten.hpp"
#include <stdexcept>

RK::System::RegistryManager::RegistryHive::RegistryHive(std::wstring subKeyName, std::wstring hivePath)
{
	// Check if the specified hive file actually exists
	bool _exists;
	bool _existsStatus = RK::System::GetFileExists(hivePath, _exists);

	if (!_exists || !_existsStatus)
	{
		// An error occurred, or the file doesn't esist.
		throw std::runtime_error("The hive file doesn't exist or it cannot be accessed.");
	}

	// Load the hive.
	long _loadKeyStatus = RegLoadKeyW(
		HKEY_USERS,
		subKeyName.c_str(),
		hivePath.c_str()
	);
	if (_loadKeyStatus != ERROR_SUCCESS)
	{
		throw std::runtime_error("The hive file could not be loaded.");
	}
	
	// The hive was loaded successfully. Get a handle to it.
	HKEY _key = nullptr;
	long _openKeyStatus = RegOpenKeyW(
		HKEY_USERS,
		subKeyName.c_str(),
		&_key
	);

	// Check for errors
	if (_openKeyStatus != ERROR_SUCCESS || _key == nullptr)
	{
		throw std::runtime_error("Failed to get handle to registry key.");
	}

	this->hiveSubKey = _key;
	this->hiveSubKeyName = subKeyName;
}

RK::System::RegistryManager::RegistryHive::~RegistryHive()
{
	// Close the key
	long _closeKeyStatus = RegCloseKey(
		this->hiveSubKey
	);

	if (_closeKeyStatus != ERROR_SUCCESS)
	{
		// Well... sorry, memory leaks.
	}

	// Unload the key
	long _unloadKeyStatus = RegUnLoadKeyW(
		HKEY_USERS,
		this->hiveSubKeyName.c_str()
	);

	if (_unloadKeyStatus != ERROR_SUCCESS)
	{
		// Hmm.
	}
}