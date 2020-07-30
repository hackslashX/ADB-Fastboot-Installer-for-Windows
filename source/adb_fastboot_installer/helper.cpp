/*
	Copyrights - ADB Fastboot Installer by hackslashX (Muhammad Fahad Baig)
*/

// Helper.cpp - Helper functions containing various methods to assist 
// TODO :: EASE DEPENDENCIES ON WINAPI AND USE STANDARD VERSIONS

#include "package.hpp"

constexpr auto GOOGLE_DRIVER_URL = "https://raw.githubusercontent.com/hackslashX/ADB-Fastboot-Installer-for-Windows/master/versions/drivers/google-drivers.zip";
constexpr auto HUAWEI_URL = "https://raw.githubusercontent.com/hackslashX/ADB-Fastboot-Installer-for-Windows/master/versions/drivers/huawei-drivers.zip";
constexpr auto LATEST_VERSION = "https://raw.githubusercontent.com/hackslashX/ADB-Fastboot-Installer-for-Windows/master/versions/latestversion";
constexpr auto PACKAGE_URL = "https://github.com/hackslashX/ADB-Fastboot-Installer-for-Windows/raw/master/versions/";

namespace helperX
{
	using namespace std;
	using namespace std::filesystem;

	/*
		bool checkSystemInstall(void)

		Checks whether adb/fastboot (determined by installation of ADB only) is already installed system-wide and returns boolean based on the result
	*/
	bool checkSystemInstall(void)
	{
		system("where adb 1> temp_installStatus.tmp 2>&1");

		fstream logFile;
		logFile.open("temp_installStatus.tmp", ios::in);
		string logData{};
		getline(logFile, logData);
		logFile.close();

		if (logData == "INFO: Could not find files for the given pattern(s).")
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	/*
		Package checkExistingVersion(void)

		Returns package information for currently installed adb/fastboot version. It is a pretty basic implementation which might fail under extreme testing environments.
	*/
	Package checkExistingVersion(void)
	{
		Package retPkg;
		system("adb version 1> temp_adbVersion.tmp 2>&1");
		system("fastboot --version 1> temp_fastbootVersion.tmp 2>&1");

		fstream logFile;
		logFile.open("temp_adbVersion.tmp", ios::in);
		string logData{};
		getline(logFile, logData);
		logData = logData.substr(29);
		retPkg.adbVersion = logData;
		logFile.close();

		logFile.open("temp_fastbootVersion.tmp", ios::in);
		getline(logFile, logData);
		logData = logData.substr(17);
		retPkg.fastbootVersion = logData;
		logFile.close();

		retPkg.packageName = "null";
		retPkg.downloadURL = "local";

		return retPkg;
	}

	/*
		void deleteExistingInstall(void)

		Delete existing system wide adb/fastboot installs to install a newer/fresh version
	*/
	void deleteExistingInstall(void)
	{
		fstream logFile;
		logFile.open("temp_installStatus.tmp", ios::in);
		system("adb kill-server 1> nul 2>&1");

		while (!logFile.eof())
		{
			string deletePath{};
			getline(logFile, deletePath);
			deletePath = deletePath.substr(0, deletePath.length() - 7); // Get directory only

			// Define various delete Paths
			string adbPath = deletePath + "adb.exe";
			string fastbootPath = deletePath + "fastboot.exe";
			string dll1Path = deletePath + "AdbWinUsbApi.dll";
			string dll2Path = deletePath + "AdbWinApi.dll";
			string dll3Path = deletePath + "libwinpthread-1.dll";

			// Commence Delete procedure
			DeleteFile(wstring(adbPath.begin(), adbPath.end()).c_str());
			DeleteFile(wstring(fastbootPath.begin(), fastbootPath.end()).c_str());
			DeleteFile(wstring(dll1Path.begin(), dll1Path.end()).c_str());
			DeleteFile(wstring(dll2Path.begin(), dll2Path.end()).c_str());
			DeleteFile(wstring(dll3Path.begin(), dll3Path.end()).c_str());
		}

		// System wide instances deleted
	}

	/*
		Package checkVersionFromRepository(void)

		Checks latest Package version from Github Repository
	*/
	Package checkVersionFromRepository(void)
	{
		Package retPkg;
		if (InternetCheckConnection(L"http://www.github.com", FLAG_ICC_FORCE_CONNECTION, 0))
		{
			string downloadPath = current_path().string() + "\\latestversion";
			string downloadUrl = LATEST_VERSION;
			HRESULT dHr = URLDownloadToFileA(NULL, downloadUrl.c_str(), downloadPath.c_str(), 0, NULL);

			if (dHr == S_OK)
			{
				fstream versionFile;
				versionFile.open(downloadPath, ios::in);
				string fileData{};

				getline(versionFile, fileData);
				retPkg.adbVersion = fileData;
				getline(versionFile, fileData);
				retPkg.fastbootVersion = fileData;
				getline(versionFile, fileData);
				int sizeTrimmed = stoi(fileData) / 1000;
				retPkg.packageSize = to_string(sizeTrimmed);
				retPkg.packageName = retPkg.adbVersion + ".zip";
				retPkg.downloadURL = PACKAGE_URL + retPkg.packageName;

				return retPkg;
			}
		}
		retPkg.packageName = "NULL";
		return retPkg;
	}

	/*
		bool downloadLatestVersionFromRepository(Package pkgD)

		Downloads latest package from the GitHub repository
	*/

	bool downloadLatestVersionFromRepository(Package pkgD)
	{
		if (InternetCheckConnection(L"http://www.github.com", FLAG_ICC_FORCE_CONNECTION, 0))
		{
			string downloadPath = current_path().string() + "\\";
			downloadPath.append(pkgD.packageName);
			HRESULT dHr = URLDownloadToFileA(NULL, pkgD.downloadURL.c_str(), downloadPath.c_str(), 0, NULL);

			if (dHr == S_OK)
			{
				return true; // Download successfull
			}
		}
		return false;
	}

	/*
		bool decompressRecievedPackage(Package pkg)

		Decompresses the downloaded zip package into current working directory
	*/
	bool decompressRecievedPackage(Package pkg)
	{
		unzFile zipFile = unzOpen(pkg.packageName.c_str());

		// Char buffer to hold read data from zip File
		voidp *rdBuff = new voidp[READ_SIZE];

		// Load Information about Zip File
		unz_global_info zipGlobalInfo;
		if (unzGetGlobalInfo(zipFile, &zipGlobalInfo) != UNZ_OK)
		{
			unzClose(zipFile);
			return false; // Return control to handle decompression error
		}

		// Start Extraction
		for (uLong i = 0; i < zipGlobalInfo.number_entry; ++i)
		{
			unz_file_info zipFileInfo;
			char filename[MAX_FILENAME];
			if (unzGetCurrentFileInfo(zipFile, &zipFileInfo, filename, MAX_FILENAME, NULL, 0, NULL, 0) != UNZ_OK)
			{
				unzClose(zipFile);
				return false; // Return control to handle decompression error
			}

			// If filename is a folder, create it and continue
			if (string(filename).find('//', string(filename).size() - 1) != string::npos) {
				create_directory(current_path().string() + "//" + string(filename) + "//");
				goto HERE;
			}

			// Dealing with file entries only
			FILE* out;
			fopen_s(&out, filename, "wb");
			if (out == NULL)
			{
				unzClose(zipFile);
				return false; // Return control to handle decompression error
			}
			if (unzOpenCurrentFile(zipFile) != UNZ_OK)
			{
				unzCloseCurrentFile(zipFile);
				unzClose(zipFile);
				return false; // Return control to handle decompression error
			}
			int error = UNZ_OK;
			do
			{
				error = unzReadCurrentFile(zipFile, rdBuff, READ_SIZE);
				if (error < 0)
				{
					unzCloseCurrentFile(zipFile);
					unzClose(zipFile);
					return false;
				}

				// Start writing data
				if (error > 0)
				{
					fwrite(rdBuff, error, 1, out);
				}
			} while (error > 0);

			fclose(out);
		HERE:
			unzCloseCurrentFile(zipFile);

			// Go to next entry
			if ((i + 1) < zipGlobalInfo.number_entry)
			{
				if (unzGoToNextFile(zipFile) != UNZ_OK)
				{
					unzClose(zipFile);
					return false;
				}
			}
		}

		unzClose(zipFile);
		return true; // Mark successfull extraction
	}

	/*
		bool installationProcedure(void)

		Copies the extracted files to the correct directories depending on the installed OS architecture
	*/
	bool installationProcedure(void)
	{
		char* pValue;
		_dupenv_s(&pValue, NULL, "SystemDrive");
		string mPath(pValue);
		free(pValue);
		string oPath = mPath + "\\hcX-af\\";
		mPath += "\\Program Files\\hcX-af\\";
		remove_all(mPath);
		create_directory(mPath);
		CopyFileA(string(current_path().string() + "\\adb.exe").c_str(), string(mPath + "adb.exe").c_str(), FALSE);
		CopyFileA(string(current_path().string() + "\\mke2fs.exe").c_str(), string(mPath + "mke2fs.exe").c_str(), FALSE);
		CopyFileA(string(current_path().string() + "\\fastboot.exe").c_str(), string(mPath + "fastboot.exe").c_str(), FALSE);
		CopyFileA(string(current_path().string() + "\\AdbWinApi.dll").c_str(), string(mPath + "AdbWinApi.dll").c_str(), FALSE);
		CopyFileA(string(current_path().string() + "\\AdbWinUsbApi.dll").c_str(), string(mPath + "AdbWinUsbApi.dll").c_str(), FALSE);
		CopyFileA(string(current_path().string() + "\\libwinpthread-1.dll").c_str(), string(mPath + "libwinpthread-1.dll").c_str(), FALSE);
		HKEY key;
		if (RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment\\"), &key) == ERROR_SUCCESS) {
			DWORD size = 1300;
			DWORD type = REG_EXPAND_SZ;
			string curVal = "";
			{
				char *nData = NULL;
				RegQueryValueEx(key, TEXT("Path"), NULL, &type, NULL, &size); // This first iteration gets the correct size
				nData = new char[size + 1];
				RegQueryValueEx(key, TEXT("Path"), NULL, &type, (LPBYTE)(nData), &size);
				// Loop through the entire data to remove redundant null chars
				for (signed int i = 0; i < size; i++) {
					if (nData[i] != '\0')
						curVal += nData[i];
				}
				delete[] nData;
			}
			mPath = mPath + ";";
			// Previous :: Remove old installer footprints
			if (curVal.find(oPath) != string::npos) curVal.replace(curVal.find(oPath + ";"), string(oPath + ";").size(), "");
			remove_all(oPath);

			// If the PATH already exists, don't re add
			if (curVal.find(mPath) == string::npos) {
				curVal += ";" + mPath;
				size = curVal.length();
				// Convert the string back to char*
				char *newVal = new char[size + 1];
				strncpy_s(newVal, size+1, curVal.c_str(), size);
				RegSetValueExA(key, "Path", NULL, type, (LPBYTE)newVal, size);
			}
			RegCloseKey(key);

			SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
				(LPARAM)L"Environment", SMTO_ABORTIFHUNG, 5000, NULL);
		}
		return true;
	}

	/*
	void cleaning(Package pkg)

	Cleans the local files downloaded and created by the installer
	*/
	void cleaning(Package pkg)
	{
		DeleteFileA(string(current_path().string() + "\\adb.exe").c_str());
		DeleteFileA(string(current_path().string() + "\\mke2fs.exe").c_str());
		DeleteFileA(string(current_path().string() + "\\fastboot.exe").c_str());
		DeleteFileA(string(current_path().string() + "\\AdbWinApi.dll").c_str());
		DeleteFileA(string(current_path().string() + "\\AdbWinUsbApi.dll").c_str());
		DeleteFileA(string(current_path().string() + "\\libwinpthread-1.dll").c_str());

		DeleteFileA(string(current_path().string() + "\\latestversion").c_str());
		DeleteFileA(string(current_path().string() + "\\temp_installStatus.tmp").c_str());
		DeleteFileA(string(current_path().string() + "\\temp_adbVersion.tmp").c_str());
		DeleteFileA(string(current_path().string() + "\\temp_fastbootVersion.tmp").c_str());
		DeleteFileA(string(current_path().string() + "\\RefreshEnv.cmd").c_str());
		string pkgPath = "\\" + pkg.packageName;
		if (pkg.packageName != "NULL") DeleteFileA(string(current_path().string() + pkgPath).c_str());
	}

	/*
	bool installDriver(char manChoice)

	A nifty utility function to install manufacturer drivers
	*/
	bool installDriver(int manChoice, HANDLE h) {
		string downloadPath{};
		string downloadURL{};
		HRESULT dHr;
		Package drv;
		switch (manChoice) {
		case 1:
			SetConsoleTextAttribute(h, ConsoleForeground::WHITE);
			cout << "[*] Downloading and installing Google USB drivers ... ";
			downloadURL = GOOGLE_DRIVER_URL;
			downloadPath = current_path().string() + "\\google-drivers.zip";
			dHr = URLDownloadToFileA(NULL, downloadURL.c_str(), downloadPath.c_str(), 0, NULL);
			if (dHr != S_OK) return false;
			drv.packageName = "google-drivers.zip";
			if (decompressRecievedPackage(drv)) {
				// Extraction Successful, begin instalation
				STARTUPINFO info = { sizeof(info) };
				PROCESS_INFORMATION processInfo;

				if (CreateProcess(L"C:\\Windows\\sysnative\\cmd.exe", L"C:\\Windows\\sysnnative\\cmd.exe /C C:\\Windows\\System32\\pnputil.exe -i -a .\\usb_driver\\android_winusb.inf", NULL, NULL, 0, CREATE_NO_WINDOW, NULL, NULL, &info, &processInfo))
				{
					WaitForSingleObject(processInfo.hProcess, INFINITE);
					CloseHandle(processInfo.hProcess);
					CloseHandle(processInfo.hThread);
				}

				// Install Done :: Cleaning
				DeleteFileA(string(current_path().string() + "\\google-drivers.zip").c_str());
				remove_all(current_path().string() + "\\usb_driver");
				return true;
			}
			else return false;

		case 2:
			SetConsoleTextAttribute(h, ConsoleForeground::WHITE);
			cout << "[*] Downloading and installing Huawei USB drivers ... ";
			downloadURL = HUAWEI_URL;
			downloadPath = current_path().string() + "\\huawei-drivers.zip";
			dHr = URLDownloadToFileA(NULL, downloadURL.c_str(), downloadPath.c_str(), 0, NULL);
			if (dHr != S_OK) return false;
			drv.packageName = "huawei-drivers.zip";
			if (decompressRecievedPackage(drv)) {
				// Extraction Successful, begin instalation
				STARTUPINFO info = { sizeof(info) };
				PROCESS_INFORMATION processInfo;

				if (CreateProcess(L".\\all\\DriverSetup.exe", L".\\all\\DriverSetup.exe", NULL, NULL, 0, CREATE_NO_WINDOW, NULL, NULL, &info, &processInfo))
				{
					WaitForSingleObject(processInfo.hProcess, INFINITE);
					CloseHandle(processInfo.hProcess);
					CloseHandle(processInfo.hThread);
				}

				// Install Done :: Cleaning
				DeleteFileA(string(current_path().string() + "\\huawei-drivers.zip").c_str());
				remove_all(current_path().string() + "\\all");
				DeleteFileA(string(current_path().string() + "\\driverinfo.xml").c_str());
				return true;
				}
				else return false;

			default:
				return false;
		}
	}
}
