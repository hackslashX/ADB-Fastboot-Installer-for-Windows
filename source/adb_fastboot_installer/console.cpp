/*
	Copyrights - ADB Fastboot Installer by hackslashX (Muhammad Fahad Baig)
*/

// Console.cpp - Heart of the program. Basic Console.

#include "package.hpp"

using namespace helperX;

int main(void)
{
	SetConsoleTitle(L"ADB and Fastboot Installer 0.1-alpha for Windows");
	HANDLE cmdH = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleOutputCP(CP_UTF8);

	// Check Internet Connectivity
	bool bValue = TRUE;
	while (bValue)
	{
		bValue = !(internetConnectionCheck(cmdH));
	}

	// Draw Header
	drawHeader(cmdH);

	// Querry ADB and Fastboot Version from Github Repository
	Package netPkg;
	netPkg = showRepositoryPackageVersion(cmdH);

	// System Wide Installation Check
	showSystemWideInstall(cmdH);

	// Start Downloading Latest Version
	showDownloadLatestVersion(cmdH, netPkg);

	// Decompression
	showDecompressingProcess(cmdH, netPkg);

	// Installation
	showInstallationProcess(cmdH, netPkg);

	// Cleaning
	showCleaningProcess(cmdH, netPkg);

	// Finish
	showFinish(cmdH);
}

