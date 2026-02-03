#include "RootKitten.hpp"
#include <stdexcept>

RK::System::Volume::Volume(std::wstring volumePath)
{
	// Copy volume path to heap and store the pointer for it.
	this->_volumePath.assign(volumePath);

	// Get volume info
	bool _result = GetVolumeInfo(
		volumePath,
		this->_volumeName,
		this->_volumeSerialNumber,
		this->_volumeFileSystem
	);

	if (!_result)
	{
		// There was an error. No reference data was modified.
		throw std::runtime_error("GetVolumeInfo failed");
	}
}

RK::System::Volume::~Volume()
{
	// There is nothing to do.
}

std::wstring RK::System::Volume::GetVolumeName()
{
	return this->_volumeName;
}

std::wstring RK::System::Volume::GetVolumeFileSystem()
{
	return this->_volumeFileSystem;
}

std::wstring RK::System::Volume::GetVolumePath()
{
	return this->_volumePath;
}

unsigned long RK::System::Volume::GetVolumeSerialNumber()
{
	return this->_volumeSerialNumber;
}

bool RK::System::Volume::GetAllVolumes(std::vector<RK::System::Volume> &volumes)
{
	// Initialise the volume list
	std::vector<RK::System::Volume> _volumes;

	// Get all the volume paths
	std::vector<std::wstring> _volumePaths;
	bool _result = GetHardDriveLetters(_volumePaths);

	if (!_result)
	{
		return false;
	}

	// For each volume, build a new Volume object
	for (int i = 0; i < _volumePaths.size(); i++)
	{
		try
		{
			// Check if the volume is a Windows volume.
			bool _windowsVolume = false;
			bool _status = GetIsWindowsVolume(_volumePaths[i], _windowsVolume);

			if (!_status || !_windowsVolume)
			{
				// An error occurred or this volume isn't a windows volume.
				// Next!
				continue;
			}

			// Create the Volume object
			RK::System::Volume _volume(_volumePaths[i]);

			// TODO: check if the volume is a Windows volume!
			// Add the volume to the list
			_volumes.push_back(_volume);
		}
		catch (const std::runtime_error& _err)
		{
			// Something went wrong. Ignore this volume
			continue;
		}
	}

	// Provide the data to the caller
	volumes = _volumes;
	return true;
}