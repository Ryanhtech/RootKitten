#include <string>
#include <vector>
#include <optional>
#include <Windows.h>

#ifndef __ROOTKITTEN_HPP__
#define __ROOTKITTEN_HPP__


namespace RK::System
{
	class Volume
	{
	public:
		/// <summary>
		/// Represents a Windows volume.
		/// </summary>
		Volume(std::wstring volumePath);
		~Volume();

		/// <summary>
		/// Returns a wstring containing the volume name.
		/// </summary>
		std::wstring GetVolumeName();

		/// <summary>
		/// Returns the volume FS.
		/// </summary>
		std::wstring GetVolumeFileSystem();

		/// <summary>
		/// Returns the volume path (letter).
		/// </summary>
		std::wstring GetVolumePath();

		/// <summary>
		/// Returns the volume's serial number.
		/// </summary>
		unsigned long GetVolumeSerialNumber();

		/// <summary>
		/// Gets all the volumes in the system.
		/// </summary>
		static bool GetAllVolumes(std::vector<RK::System::Volume>& volumes);

	private:
		std::wstring _volumePath;
		std::wstring _volumeFileSystem;
		std::wstring _volumeName;
		unsigned long _volumeSerialNumber = 0;
	};

	/// <summary>
	/// Gets the hard drive letters that are currently connected to the system.
	/// Returns true when the operation completes successfully, false otherwise.
	/// </summary>
	bool GetHardDriveLetters(std::vector<std::wstring>& driveLetters);

	/// <summary>
	/// Returns info about the volume specified in argument "volumeLetter".
	/// </summary>
	/// <returns>
	/// true if the operation performed successfully, false otherwise. If false is
	/// returned, no data is modified in the arguments.
	/// </returns>
	bool GetVolumeInfo(std::wstring volumeLetter, std::wstring &volumeName, unsigned long &volumeSerialNumber, std::wstring &volumeFileSystem);

	/// <summary>
	/// Gets an AdjustPrivileges token for this process.
	/// </summary>
	bool GetAdjustPrivilegesProcessToken(HANDLE &tokenHandle);

	/// <summary>
	/// Checks whether a spacified directory exists. Returns false if an error
	/// occurred.
	/// </summary>
	bool GetDirectoryExists(std::wstring checkPath, bool &directoryExists);

	/// <summary>
	/// Determines whether a file (or directory...) exists or not. Returns false if an error occurred.
	/// </summary>
	bool GetFileExists(std::wstring filePath, bool &fileExists);

	/// <summary>
	/// Checks if a specified volume is a Windows volume, by considering multiple
	/// clues. Returns false in case of error.
	/// </summary>
	bool GetIsWindowsVolume(std::wstring volumeLetter, bool &isWindowsVolume);

	/// <summary>
	/// Sets the Windows privilege on the object contained in the handle.
	/// </summary>
	bool SetPrivilege(HANDLE handle, const wchar_t *privilegeName, bool enable);

	/// <summary>
	/// Immediatly shuts down the system.
	/// </summary>
	bool ShutDownWindows(bool restart);
	bool ShutDownWindows();

	/// <summary>
	/// Immediately restarts the system.
	/// </summary>
	bool RestartWindows();

	/// <summary>
	/// Initialises RootKitten critical components.
	/// * Grants the required privileges for RootKitten
	/// Returns false if an error occurred, true if everything is OK.
	/// </summary>
	bool RootKittenInit();

	namespace RegistryManager
	{
		class RegistryHive
		{
		public:
			/// <summary>
			/// Loads a Windows registry hive as a HKEY_LOCAL_MACHINE sub-key.
			/// Can throw an exception if an initialisation error occurrs.
			/// </summary>
			/// <param name="subKeyName">
			/// The HKEY_LOCAL_MACHINE sub-key name in which you want to load the hive data.
			/// </param>
			/// <param name="hivePath">
			/// The absolute path to the hive file you want to load.
			/// </param>
			RegistryHive(std::wstring subKeyName, std::wstring hivePath);
			~RegistryHive();

		private:
			HKEY hiveSubKey;
		};

		// Constant data
		const std::wstring relativePathToConfigDirectory = L"Windows\\System32\\config\\";
		const std::wstring relativePathToSystemHiveFile = relativePathToConfigDirectory + L"SYSTEM";

		/// <summary>
		/// Determines whether the system hive exists on the specified drive.
		/// Returns false if an error occurred, true otherwise.
		/// </summary>
		bool GetDoesSystemHiveExist(std::wstring drivePath, bool &exists);
	}

	namespace UtilmanExploit
	{
		/// <summary>
		/// Designates a drive-relative path to the Utilman executable file.
		/// To get the Utilman file path, add the drive root directory (e.g. C:\)
		/// before this string.
		/// </summary>
		const std::wstring relativePathToUtilman = L"Windows\\System32\\Utilman.exe";
		const std::wstring relativePathToOldUtilman = relativePathToUtilman + L".old";
		const std::wstring relativePathToCommandLine = L"Windows\\System32\\cmd.exe";

		/// <summary>
		/// Determines if the utilman exploit is enabled on a specific drive.
		/// Returns true if the operation completed with no error, false otherwise.
		/// </summary>
		bool GetIsUtilmanExploitEnabled(std::wstring drivePath, bool &enabled);

		/// <summary>
		/// Immediately enables the Utilman exploit on the specified drive. Returns
		/// false if an error occurred, true otherwise.
		/// </summary>
		bool EnableUtilmanExploit(std::wstring drivePath);

		/// <summary>
		/// Immediately disables the Utilman exploit on the specified drive. Returns
		/// false if an error occurred, true otherwise.
		/// </summary>
		bool DisableUtilmanExploit(std::wstring drivePath);
	}
}

namespace RK::UI
{
	/// <summary>
	/// Prints the specified C++-style string. It doesn't include the \n character.
	/// </summary>
	void PrintString(std::wstring str);

	/// <summary>
	/// Prompts the user for a string value. Prints the prompt message beforehand.
	/// </summary>
	std::wstring PromptStr(std::wstring promptMessage);

	/// <summary>
	/// Like PromptStr, but asks for an int instead.
	/// </summary>
	int PromptInt(std::wstring promptMessage);

	/// <summary>
	/// Displays the copyright associated with the programme.
	/// </summary>
	void PrintCopyright();

	/// <summary>
	/// Displays the separator to separate different parts of the UI on the console.
	/// </summary>
	void PrintSeparator();

	/// <summary>
	/// Displays the list provided in parameter listItems and returns the index of the
	/// choice made by the user in the provided list.
	/// </summary>
	int ChoiceList(std::vector<std::wstring> *listItems);

	/// <summary>
	/// Retrieves the list of the drives, and prompts the user for the system drive. If
	/// an error occurrs when retrieving the hard drives, this function returns false.
	/// It returns true otherwise.
	/// </summary>
	bool PromptForSystemDrive(RK::System::Volume **chosenDrive);

	/// <summary>
	/// This method prompts for the user's consent. If the user gave their consent, returns
	/// true; returns false otherwise.
	/// </summary>
	bool PromptForConsent(std::wstring message);

	/// <summary>
	/// Initialises the list of main menu items and returns a pointer to it.
	/// WARNING! If you modify the main menu items, please also check in the MainLoop
	/// function if the indexes still match.
	/// </summary>
	std::vector<std::wstring> *GetMainMenuItems();

	/// <summary>
	/// Starts the Utilman loop, guiding the user through Utilman installation/uninstallation.
	/// </summary>
	void UtilmanLoop(std::wstring drivePath);

	/// <summary>
	/// The program's main function. Returns false if an error occurred.
	/// </summary>
	bool MainLoop();
}

int main(int argc, char* argv[]);

#endif