#include "RootKitten.hpp"
#include <stdexcept>

RK::System::UserManager::UserProfile::UserProfile(std::wstring profilePath, unsigned int profileSid)
{
	// Call the initialisation method with the default path to the user registry hive file
	this->Initialise(profilePath, profileSid, profilePath + L"NTUSER.DAT");
}

RK::System::UserManager::UserProfile::UserProfile(std::wstring profilePath, unsigned int profileSid, std::wstring userHivePath)
{
	this->Initialise(profilePath, profileSid, userHivePath);
}

void RK::System::UserManager::UserProfile::Initialise(std::wstring profilePath, unsigned int profileSid, std::wstring userHivePath)
{
	// Copy the parameters to local attributes
	this->_profilePath = profilePath;
	this->_sid = profileSid;
	this->_userHivePath = userHivePath;
}

RK::System::UserManager::UserProfile::~UserProfile()
{
	// Destroy pointers
	this->UnloadUserHive();
}

bool RK::System::UserManager::UserProfile::LoadUserHive()
{
	// Attempt to initialise a registry hive from the provided hive file. This will throw a runtime_error
	// if it fails for some reason.
	try
	{
		this->_userHive = new RK::System::RegistryManager::RegistryHive(L"user_" + std::to_wstring(this->_sid), this->_userHivePath);
	}
	catch (std::runtime_error &_err)
	{
		// An error occurred.
		return false;
	}

	return true;
}

void RK::System::UserManager::UserProfile::UnloadUserHive()
{
	// Delete the class.. Goodbye!
	if (this->_userHive != nullptr)
	{
		delete this->_userHive;
	}
}

bool RK::System::UserManager::UserProfile::GetUserHive(RK::System::RegistryManager::RegistryHive &key)
{
	// Attempt to get the user's hive instance.
	// If this instance is null, report an error.
	if (this->_userHive != nullptr)
	{
		key = *this->_userHive;
		return true;
	}

	return false;
}
