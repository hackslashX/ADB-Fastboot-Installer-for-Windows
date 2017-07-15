/*
	Copyrights - ADB Fastboot Installer by hackslashX (Muhammad Fahad Baig)
*/

// Helper.cpp - Helper functions containing various methods to assist

#include "package.hpp"

namespace helperX
{
	using namespace std;
	using namespace std::experimental::filesystem::v1;

	/*
		bool checkSystemArch(void)

		A Windows only function that tells whether installed Windows OS is 32-bit or 64-bit.
		http://blogs.msdn.com/oldnewthing/archive/2005/02/01/364563.aspx
	*/
	bool checkArch64(void)
	{
		#if defined(_WIN64)
			return TRUE;  // 64-bit programs run only on Win64
		#elif defined(_WIN32)
		// 32-bit programs run on both 32-bit and 64-bit Windows so must sniff more
		BOOL f64 = FALSE;
			return IsWow64Process(GetCurrentProcess(), &f64) && f64;
		#else
			return FALSE; // Win64 does not support Win16
		#endif
	}

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
			string downloadUrl = "https://raw.githubusercontent.com/hackslashX/ADB-Fastboot-Installer-for-Windows/master/versions/latestversion";
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
				retPkg.downloadURL = "https://github.com/hackslashX/ADB-Fastboot-Installer-for-Windows/raw/master/versions/" + retPkg.packageName;

				return retPkg;
			}
		}
		else
		{
			retPkg.packageName = "NULL";
			return retPkg;
		}
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
		else
		{
			return false;
		}
	}

	/*
		bool decompressRecievedPackage(Package pkg)

		Decompresses the downloaded zip package into current working directory
	*/
	bool decompressRecievedPackage(Package pkg)
	{
		unzFile zipFile = unzOpen(pkg.packageName.c_str());

		// Char buffer to hold read data from zip File
		voidp rdBuff[READ_SIZE];

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

			// Dealing with file entries only
			FILE *out = fopen(filename, "wb");
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
		char windowsPathA[MAX_PATH];
		GetWindowsDirectoryA(windowsPathA, MAX_PATH);
		string windowsPath(windowsPathA);

		if (checkArch64() == TRUE) // 64-bit Operating System
		{
			CopyFileA(string(current_path().string() + "\\adb.exe").c_str(), string(windowsPath + "\\adb.exe").c_str(), FALSE);
			CopyFileA(string(current_path().string() + "\\fastboot.exe").c_str(), string(windowsPath + "\\fastboot.exe").c_str(), FALSE);
			CopyFileA(string(current_path().string() + "\\AdbWinApi.dll").c_str(), string(windowsPath + "\\SysWOW64\\AdbWinApi.dll").c_str(), FALSE);
			CopyFileA(string(current_path().string() + "\\AdbWinUsbApi.dll").c_str(), string(windowsPath + "\\SysWOW64\\AdbWinUsbApi.dll").c_str(), FALSE);
			CopyFileA(string(current_path().string() + "\\libwinpthread-1.dll").c_str(), string(windowsPath + "\\SysWOW64\\libwinpthread-1.dll").c_str(), FALSE);
			return true;
		}
		else // 32-bit Operating System
		{
			CopyFileA(string(current_path().string() + "\\adb.exe").c_str(), string(windowsPath + "\\System32\\adb.exe").c_str(), FALSE);
			CopyFileA(string(current_path().string() + "\\fastboot.exe").c_str(), string(windowsPath + "\\System32\\fastboot.exe").c_str(), FALSE);
			CopyFileA(string(current_path().string() + "\\AdbWinApi.dll").c_str(), string(windowsPath + "\\System32\\AdbWinApi.dll").c_str(), FALSE);
			CopyFileA(string(current_path().string() + "\\AdbWinUsbApi.dll").c_str(), string(windowsPath + "\\System32\\AdbWinUsbApi.dll").c_str(), FALSE);
			CopyFileA(string(current_path().string() + "\\libwinpthread-1.dll").c_str(), string(windowsPath + "\\System32\\libwinpthread-1.dll").c_str(), FALSE);
			return true;
		}
		return false; // Fail-safe
	}

	void cleaning(Package pkg)
	{
		DeleteFileA(string(current_path().string() + "\\adb.exe").c_str());
		DeleteFileA(string(current_path().string() + "\\fastboot.exe").c_str());
		DeleteFileA(string(current_path().string()+ "\\AdbWinApi.dll").c_str());
		DeleteFileA(string(current_path().string() + "\\AdbWinUsbApi.dll").c_str());
		DeleteFileA(string(current_path().string() + "\\libwinpthread-1.dll").c_str());

		DeleteFileA(string(current_path().string() + "\\latestversion").c_str());
		DeleteFileA(string(current_path().string() + "\\temp_installStatus.tmp").c_str());
		DeleteFileA(string(current_path().string() + "\\temp_adbVersion.tmp").c_str());
		DeleteFileA(string(current_path().string() + "\\temp_fastbootVersion.tmp").c_str());
		string pkgPath = "\\" + pkg.packageName;
		if (pkg.packageName != "NULL") DeleteFileA(string(current_path().string() + pkgPath).c_str());
	}
}