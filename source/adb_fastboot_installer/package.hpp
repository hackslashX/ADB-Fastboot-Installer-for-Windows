/*
	Copyrights - ADB Fastboot Installer by hackslashX (Muhammad Fahad Baig)
*/

// Package.hpp - Header file containing necessary package definitions

#ifndef  PACKAGE_HPP
#define PACKAGE_HPP

#include <string>
#include <algorithm>
#include <Windows.h>
#include <WinInet.h>
#include <intrin.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <SetupAPI.h>

#include "zlib\unzip.h"

#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "urlmon.lib")

constexpr auto MAX_FILENAME = 512;
constexpr auto READ_SIZE = 8192;

struct Package
{
	std::string packageName;
	std::string adbVersion;
	std::string fastbootVersion;
	std::string downloadURL;
	std::string packageSize;
};

namespace helperX
{
	bool checkSystemInstall(void);
	Package checkExistingVersion(void);
	void deleteExistingInstall(void);
	Package checkVersionFromRepository(void);
	bool downloadLatestVersionFromRepository(Package pkgD);
	bool decompressRecievedPackage(Package pkg);
	bool installationProcedure(void);
	bool installDriver(int,HANDLE);
	void cleaning(Package pkg);
}

namespace ConsoleForeground
{
	enum {
		BLACK = 0,
		DARKBLUE = FOREGROUND_BLUE,
		DARKGREEN = FOREGROUND_GREEN,
		DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
		DARKRED = FOREGROUND_RED,
		DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
		DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
		DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		GRAY = FOREGROUND_INTENSITY,
		BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
		GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
		RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
		MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
		YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	};
}

// Prototypes
void drawHeader(HANDLE h);
bool internetConnectionCheck(HANDLE h);
Package showRepositoryPackageVersion(HANDLE h);
void showSystemWideInstall(HANDLE h);
void showDeleteExistingInstall(HANDLE h);
void showDownloadLatestVersion(HANDLE h, Package pkg);
void showDecompressingProcess(HANDLE h, Package pkg);
void showInstallationProcess(HANDLE h, Package pkg);
void showCleaningProcess(HANDLE h, Package pkg);
void showDriverInstall(HANDLE h);
void showFinish(HANDLE h);

#endif // ! PACKAGE_HPP
