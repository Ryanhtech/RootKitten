#include "RootKitten.hpp"

bool RK::System::UserManager::GetUserProfiles(RK::System::RegistryManager::RegistryHive systemHive, RK::System::Volume systemVolume, std::vector<RK::System::UserManager::UserProfile>& profiles)
{
	// Get the ProfileList subkey
	HKEY _profileListKey;
	bool _result = systemHive.GetSubKeyPointer(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList", _profileListKey);

	// Check if an error occurred
	if (!_result)
	{
		return false;
	}

	return true;
}