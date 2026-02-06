#include "RootKitten.hpp"


bool RK::System::GetHardDriveLetters(std::vector<std::wstring> &driveLetters)
{
	// Initialise a new buffer.
	wchar_t _buf[MAX_PATH];

	// Initialise a new string vector.
	std::vector<std::wstring> _list;

	// Query the device letters.
	int _errorCode = GetLogicalDriveStringsW(
		MAX_PATH,
		_buf
	);

	// Check the error code.
	if (!_errorCode)
	{
		return false;
	}

	// Get the drives one by one.
	std::wstring _currentDrive = L"";
	for (int i = 0; i < MAX_PATH; i++)
	{
		const wchar_t _chr = _buf[i];

		if (_chr != NULL)
		{
			_currentDrive += _chr;
		}
		else
		{
			// We encountered a null terminator. Check if the previous one was a null terminator too.
			if (_currentDrive.empty())
			{
				break;
			}

			// Go to the next drive.
			_list.push_back(_currentDrive);
			_currentDrive = L"";
		}
	}

	driveLetters = _list;
	return true;
}

bool RK::System::GetVolumeInfo(std::wstring volumeLetter, std::wstring &volumeName, unsigned long &volumeSerialNumber, std::wstring &volumeFileSystem)
{
	// Initialise buffers
	wchar_t _volumeName[MAX_PATH + 1];
	unsigned long _volumeSerialNumber;
	wchar_t _fileSystem[MAX_PATH + 1];

	// Call the Windows API to get volume label
	bool _result = GetVolumeInformationW(
		volumeLetter.c_str(),   // Root path to drive
		_volumeName,            // Volume name buffer
		MAX_PATH + 1,           // Buffer size
		&_volumeSerialNumber,   // Volume SN
		NULL,                   // Max component length (unused)
		NULL,                   // File system flags (unused)
		_fileSystem,            // File system name buffer
		MAX_PATH + 1            // Buffer size
	);

	if (!_result)
	{
		// An error occurred, abort
		return false;
	}

	// Modify pointer data and return
	volumeSerialNumber = _volumeSerialNumber;
	volumeName.assign(_volumeName);
	volumeFileSystem.assign(_fileSystem);

	return true;
}

bool RK::System::GetAdjustPrivilegesProcessToken(HANDLE &tokenHandle)
{
	// Adjust process privileges
	HANDLE _processToken;

	// Get a token for this process.
	bool _openTokenResult = OpenProcessToken(
		GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&_processToken
	);

	if (!_openTokenResult)
	{
		return false;
	}

	tokenHandle = _processToken;
	return true;
}

bool RK::System::GetDirectoryExists(std::wstring checkPath, bool &directoryExists)
{
	// Check if the length of the path to check is not too long.
	if (checkPath.size() > MAX_PATH - 1)
	{
		directoryExists = false;
		SetLastError(INVALID_FILE_ATTRIBUTES);
		return false;
	}

	// Get the attributes of the directory
	unsigned long _attributes = GetFileAttributesW(
		checkPath.c_str()
	);

	// Check if there was an error
	if (_attributes == INVALID_FILE_ATTRIBUTES)
	{
		directoryExists = false;
		return false;
	}

	// Check if the file is a directory.
	if (_attributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		directoryExists = true;
	}
	else
	{
		directoryExists = false;
	}

	return true;
}

bool RK::System::GetFileExists(std::wstring filePath, bool &fileExists)
{
	// Check the length of the file.
	if (filePath.size() > MAX_PATH - 1)
	{
		// The file path is too long.
		fileExists = false;
		SetLastError(INVALID_FILE_ATTRIBUTES);
		return false;
	}

	// Get the file's attributes
	unsigned long _attributes = GetFileAttributesW(
		filePath.c_str()
	);

	// Check if there was an error
	if (_attributes == INVALID_FILE_ATTRIBUTES)
	{
		// Check if the error is file not found or path not found.
		unsigned long _err = GetLastError();
		if (_err == ERROR_FILE_NOT_FOUND
			|| _err == ERROR_PATH_NOT_FOUND)
		{
			// The file just doesn't exist.
			fileExists = false;
			return true;
		}

		// There was an error.
		fileExists = false;
		return false;
	}

	// The file exists.
	fileExists = true;
	return true;
}

bool RK::System::GetIsWindowsVolume(std::wstring volumeLetter, bool &isWindowsVolume)
{
	// Initialise the list of directories to check
	std::vector<std::wstring> _directoriesToCheck;
	_directoriesToCheck.push_back(L"Windows");
	_directoriesToCheck.push_back(L"Windows\\System32");
	_directoriesToCheck.push_back(L"Windows\\System32\\config");
	_directoriesToCheck.push_back(L"Users");

	// Loop through these directories and make sure they all exist.
	bool _exists = false;
	bool _status = false;

	for (int i = 0; i < _directoriesToCheck.size(); i++)
	{
		// Call our beautiful API
		_status = GetDirectoryExists(volumeLetter + _directoriesToCheck[i], _exists);

		// Check if there was an error
		if (!_status || !_exists)
		{
			// Determine the cause of the error. If the directory doesn't exist,
			// there was no real error. Otherwise, another problem is happening
			// and we should report it.
			unsigned long _err = GetLastError();

			if (_err == ERROR_PATH_NOT_FOUND
				|| _err == ERROR_FILE_NOT_FOUND)
			{
				// The directory doesn't exist.
				isWindowsVolume = false;
				return true;
			}

			// There was another error. We cannot determine whether this is a
			// Windows volume or not.
			isWindowsVolume = false;
			return false;
		}
	}

	// The test passed: this is a windows volume.
	isWindowsVolume = true;
	return true;
}

bool RK::System::GetIsElevated(bool &elevated)
{
	// Get a token to this process
	HANDLE _tokHandle;
	bool _openTokenStatus = OpenProcessToken(
		GetCurrentProcess(),
		TOKEN_QUERY,
		&_tokHandle
	);

	if (!_openTokenStatus)
	{
		elevated = false;
		return false;
	}

	// Retrieve the elevation information
	TOKEN_ELEVATION _elevation;
	unsigned long _dataLength = sizeof(TOKEN_ELEVATION);
	bool _getInformationStatus = GetTokenInformation(
		_tokHandle,
		TokenElevation,
		&_elevation,
		sizeof(_elevation),
		&_dataLength
	);

	if (!_getInformationStatus)
	{
		elevated = false;
		return false;
	}

	// Close the token
	CloseHandle(_tokHandle);

	// Determine if the process is elevated.
	elevated = _elevation.TokenIsElevated;
	return true;
}

bool RK::System::SetPrivilege(HANDLE handle, const wchar_t *privilegeName, bool enable)
{
	TOKEN_PRIVILEGES _privileges; // Privilege info
	LUID             _luid;       // Privilege ID

	bool _lookupStatus = LookupPrivilegeValueW(
		NULL,             // Computer name (NULL: this computer)
		privilegeName,    // Privilege name
		&_luid            // Reference to LUID
	);

	if (!_lookupStatus)
	{
		// Lookup has failed.
		return false;
	}

	// Zero privileges memory.
	ZeroMemory(&_privileges, sizeof(TOKEN_PRIVILEGES));

	// Set the privilege properties
	_privileges.PrivilegeCount = 1;
	_privileges.Privileges[0].Luid = _luid;

	if (enable)
	{
		_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else
	{
		_privileges.Privileges[0].Attributes = 0;
	}

	// Request privilege modification
	bool _adjustStatus = AdjustTokenPrivileges(
		handle,
		FALSE,
		&_privileges,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL
	);

	// Check if there was an error
	if (!_adjustStatus || GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		return false;
	}

	// Privileges were adjusted successfully.
	return true;
}

bool RK::System::ShutDownWindows(bool restart)
{
	// Shut down Windows now
	bool _status = InitiateSystemShutdownExW(
		NULL,     // Machine name
		NULL,     // Shutdown message
		0,        // Timeout
		TRUE,     // Force apps to be closed
		restart,  // Restart after shutdown
		SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_OTHER // Shutdown reason
	);

	return _status;
}

bool RK::System::ShutDownWindows()
{
	return ShutDownWindows(false);
}

bool RK::System::RestartWindows()
{
	// Restart Windows immediately
	return ShutDownWindows(true);
}

bool RK::System::RootKittenInit()
{
	// Check if RootKitten is elevated
	bool _isElevated = false;
	if (!GetIsElevated(_isElevated))
	{
		return false;
	}
	if (!_isElevated)
	{
		RK::UI::PrintString(L"RootKitten is not elevated. Please run RootKitten as Administrator.\n");
		SetLastError(ERROR_ACCESS_DENIED);
		return false;
	}

	// Get handle to a token for this process
	HANDLE _adjustPrivilegesTokenHandle;
	bool _tokResult = GetAdjustPrivilegesProcessToken(
		_adjustPrivilegesTokenHandle
	);

	if (!_tokResult)
	{
		return false;
	}

	// Set required privileges
	bool _privilegeResult = RK::System::SetPrivilege(
		_adjustPrivilegesTokenHandle,
		SE_SHUTDOWN_NAME,
		true
	);
	bool _privilege2Result = RK::System::SetPrivilege(
		_adjustPrivilegesTokenHandle,
		SE_BACKUP_NAME,
		true
	);
	bool _privilege3Result = RK::System::SetPrivilege(
		_adjustPrivilegesTokenHandle,
		SE_RESTORE_NAME,
		true
	);

	if (!_privilegeResult || !_privilege2Result || !_privilege3Result)
	{
		// Release handles
		CloseHandle(_adjustPrivilegesTokenHandle);

		return false;
	}

	return true;
}
