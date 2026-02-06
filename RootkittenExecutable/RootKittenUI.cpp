#include "RootKitten.hpp"
#include <iostream>


void RK::UI::PrintString(std::wstring str)
{
	std::wcout << str;
}

std::wstring RK::UI::PromptStr(std::wstring promptMessage)
{
	// Display the prompt message.
	PrintString(promptMessage);

	// Wait for user input
	std::wstring _input;
	std::wcin >> _input;

	// Return user input
	return _input;
}

int RK::UI::PromptInt(std::wstring promptMessage)
{
	// Initialise the variables
	std::wstring _inputStr;
	int _inputInt = 0;

	while (true)
	{
		// Get the user input.
		_inputStr = PromptStr(promptMessage);

		// Check if the user input can be converted to an int.
		try
		{
			_inputInt = std::stoi(_inputStr);
			return _inputInt;
		}
		catch (std::invalid_argument)
		{
			PrintString(L"This number is incorrect, try again.\n");
		}
		catch (std::out_of_range)
		{
			PrintString(L"This number is out of range, try again.\n");
		}
	}
}

void RK::UI::PrintCopyright()
{
	PrintString(
		L"RootKitten Executable\n"
		L"The RootKitten Project\n"
		L"(c) 2026 Ryanhtech Labs\n"
	);
	PrintSeparator();
}

void RK::UI::PrintSeparator()
{
	PrintString(L"+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
}

int RK::UI::ChoiceList(std::vector<std::wstring> *listItems)
{
	// Print all the items in the list. Be sure to add 1 to each index displayed.
	for (int i = 0; i < listItems->size(); i++)
	{
		std::wstring _indexStr = std::to_wstring(i + 1);
		RK::UI::PrintString(L"[" + _indexStr + L"] " + listItems->at(i) + L"\n");
	}

	// Initialise the variables
	int _userInput;

	while (true)
	{
		// Prompt the user for an input as an index
		_userInput = PromptInt(L"Which item do you want to choose?\n?>") - 1;

		// Check if this input actually exists
		if (_userInput >= 0 && _userInput < listItems->size())
		{
			// It does! Return it.
			return _userInput;
		}

		// Print an error message.
		PrintString(L"Error: this number is out of range. Try again.\n");
	}
}

bool RK::UI::PromptForSystemDrive(RK::System::Volume **systemDrive)
{
	// Get the Volumes
	std::vector<RK::System::Volume> _volumes;
	bool _result = RK::System::Volume::GetAllVolumes(_volumes);

	// Check if result is OK.
	if (!_result)
	{
		// Result is not OK.
		systemDrive = nullptr;
		return false;
	}

	// Build a list containing the volume info for the user
	std::vector<std::wstring> _volumeInfo;
	std::wstring _volName;
	std::wstring _volPath;
	std::wstring _volFS;
	std::wstring _volSN;
	std::wstring _volInfoLine;

	for (int i = 0; i < _volumes.size(); i++)
	{
		_volName = _volumes[i].GetVolumeName();
		_volPath = _volumes[i].GetVolumePath();
		_volFS = _volumes[i].GetVolumeFileSystem();
		_volSN = std::to_wstring(_volumes[i].GetVolumeSerialNumber());

		if (_volName.empty())
		{
			_volInfoLine = _volPath + L" (" + _volSN + L", formatted as " + _volFS + L")";
		}
		else
		{
			_volInfoLine = _volPath + L" [" + _volName + L"] (" + _volSN + L", formatted as " + _volFS + L")";
		}

		_volumeInfo.push_back(_volInfoLine);
	}

	// Prompt a drive from the list.
	PrintString(L"Select the Windows drive you want to work on. This drive will be used to make changes to the system.\n");
	int selectedIndex = ChoiceList(&_volumeInfo);
	std::wstring _selectedDriveStr = _volumes.at(selectedIndex).GetVolumePath();
	RK::System::Volume* _selectedDrive = new RK::System::Volume(_selectedDriveStr);

	// Return the user's choice
	*systemDrive = _selectedDrive;
	return true;
}

bool RK::UI::PromptForConsent(std::wstring message)
{
	// Start while true loop.
	std::wstring _answer;

	while (true)
	{
		PrintString(message);
		_answer = PromptStr(L"\nPlease type 'yes' or 'no'\n?>");

		if (_answer == L"yes")
		{
			return true;
		}
		else if (_answer == L"no")
		{
			return false;
		}

		PrintString(L"We didn't quite understand what you said. Try again.\n");
	}
}

std::vector<std::wstring> *RK::UI::GetMainMenuItems()
{
	std::vector<std::wstring> _list =
	{
		L"Exit RootKitten",
		L"Restart the system",
		L"Power off the system",
		L"Install/Uninstall Utilman exploit",
		L"Mount system Registry hive and open in Regedit",
		L"Mount user Registry hive and open in Regedit",
	};

	std::vector<std::wstring>* _listPtr = new std::vector<std::wstring>(_list);
	return _listPtr;
}

void RK::UI::UtilmanLoop(std::wstring drivePath)
{
	// Check if utilman is enabled...
	bool _enableStatus;
	bool _utilmanEnabled;
	bool _status = RK::System::UtilmanExploit::GetIsUtilmanExploitEnabled(drivePath, _utilmanEnabled);

	if (!_status)
	{
		unsigned long _err = GetLastError();
		PrintString(L"Win32 error (UTILMAN-A): " + std::to_wstring(_err) + L"\n");
		return;
	}

	// Depending on whether Utilman is enabled or not, show different messages.
	if (_utilmanEnabled)
	{
		if (!PromptForConsent(L"Utilman is enabled. Disable Utilman?"))
		{
			PrintString(L"Cancelled.\n");
			return;
		}

		_enableStatus = RK::System::UtilmanExploit::DisableUtilmanExploit(drivePath);
	}
	else
	{
		if (!PromptForConsent(L"Utilman is disabled. Enable Utilman on drive " + drivePath + L"?"))
		{
			PrintString(L"Cancelled.\n");
			return;
		}

		_enableStatus = RK::System::UtilmanExploit::EnableUtilmanExploit(drivePath);
	}

	if (!_enableStatus)
	{
		unsigned long _err = GetLastError();
		PrintString(L"Win32 error (UTILMAN-B): " + std::to_wstring(_err) + L"\n");
		return;
	}

	PrintString(L"The requested operation completed successfuly.\n");
}

bool RK::UI::MainLoop()
{
	// Initialise the main menu items.
	std::vector<std::wstring> *_mainMenuItems = GetMainMenuItems();

	// Prompt the drive to use.
	RK::System::Volume *_drive = nullptr;
	bool _result = RK::UI::PromptForSystemDrive(&_drive);

	if (!_result || _drive == nullptr)
	{
		// Something went wrong.
		return false;
	}

	// Load the drive's registry hive
	RK::System::RegistryManager::RegistryHive *_hive = nullptr;
	try
	{
		_hive = new RK::System::RegistryManager::RegistryHive(
			L"SYSTEM_" + std::to_wstring(_drive->GetVolumeSerialNumber()),
			_drive->GetVolumePath() + RK::System::RegistryManager::relativePathToSystemHiveFile
		);
	}
	catch (std::runtime_error &_err)
	{
		// There was an error
		std::cout << _err.what();
		delete _drive;
		return false;
	}

	PrintSeparator();

	// Initialise the loop variables.
	int _choiceIndex = 0;

	// Start the main loop
	while (true)
	{
		// Print main menu title
		PrintString(L"Ryanhtech RootKitten Main Menu\nWorking on drive " + _drive->GetVolumePath() + L"\n\n");
		PrintString(L"Enter a feature ID, and press Enter.\n");

		// Ask the user for their choice
		_choiceIndex = ChoiceList(_mainMenuItems);

		// Perform the requested action
		switch (_choiceIndex)
		{
		case 0:
			// Exit RootKitten
			// Absolutely free the memory
			delete _mainMenuItems;
			delete _drive;
			delete _hive;

			// Say good-bye !
			PrintString(L"Goodbye!\n");
			return true;

		case 1:
			if (!RK::System::RestartWindows())
			{
				delete _mainMenuItems;
				delete _drive;
				delete _hive;
				return false;
			}
			break;

		case 2:
			if (!RK::System::ShutDownWindows())
			{
				delete _mainMenuItems;
				delete _drive;
				delete _hive;
				return false;
			}
			break;

		case 3:
			RK::UI::UtilmanLoop(_drive->GetVolumePath());
			break;

		default:
			// The feature wasn't found.
			PrintString(L"We couldn't find a feature corresponding to your request - Try again.\n");
			break;
		}

		// Print separator
		PrintSeparator();
	}
}

// -- RootKitten Main Function -- //
int main(int argc, char *argv[])
{
	RK::UI::PrintCopyright();

	// Initialise the system
	bool _initResult = RK::System::RootKittenInit();
	if (!_initResult)
	{
		// Something went wrong
		RK::UI::PrintString(L"An error occurred. Win32 error code: " + std::to_wstring(GetLastError()) + L"\n");
		return GetLastError();
	}

	bool _status = RK::UI::MainLoop();

	if (!_status)
	{
		RK::UI::PrintString(L"An error occurred. Win32 error code: " + std::to_wstring(GetLastError()) + L"\n");
		RK::UI::PromptStr(L"fuezezf");
		return GetLastError();
	}

	return 0;
}
