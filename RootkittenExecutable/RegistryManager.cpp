#include "RootKitten.hpp"

bool RK::System::RegistryManager::GetDoesSystemHiveExist(std::wstring drivePath, bool &exists)
{
	// Determine if a file exists at the usual system hive file location.
	bool _fileExists;
	bool _status = RK::System::GetFileExists(relativePathToSystemHiveFile, _fileExists);

	if (!_status)
	{
		// An error occurred. We can't determine whether the file exists
		// or not.
		exists = false;
		return false;
	}

	// Check if the file exists.
	exists = _fileExists;
	return true;
}
