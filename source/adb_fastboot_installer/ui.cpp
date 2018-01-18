/*
	Copyrights - ADB Fastboot Installer by hackslashX (Muhammad Fahad Baig)
*/

// ui.cpp - Implementation of classic user interface

#include "package.hpp"

using namespace helperX;
using namespace ConsoleForeground;
using namespace std;

void drawHeader(HANDLE h)
{
	system("cls");
	SetConsoleTextAttribute(h, DARKGRAY);
	printf("──────────────────────────────────────────────────────────────────────────────\n");
	printf("╔═╗╔╦╗╔╗   ┌─┐┌┐┌┌┬┐  ╔═╗╔═╗╔═╗╔╦╗╔╗ ╔═╗╔═╗╔╦╗  ┬┌┐┌┌─┐┌┬┐┌─┐┬  ┬  ┌─┐┬─┐\n");
	printf("╠═╣ ║║╠╩╗  ├─┤│││ ││  ╠╣ ╠═╣╚═╗ ║ ╠╩╗║ ║║ ║ ║   ││││└─┐ │ ├─┤│  │  ├┤ ├┬┘\n");
	printf("╩ ╩═╩╝╚═╝  ┴ ┴┘└┘─┴┘  ╚  ╩ ╩╚═╝ ╩ ╚═╝╚═╝╚═╝ ╩   ┴┘└┘└─┘ ┴ ┴ ┴┴─┘┴─┘└─┘┴└─\n");
	printf("──────────────────────────────────────────────────────────────────────────────\n");
	SetConsoleTextAttribute(h, CYAN);
	cout << "Developer: Muhammad Fahad Baig (hackslashX @github)" << endl;
	cout << "Email Support: muhammad.fb.79@gmail.com" << endl;
	cout << "Program Version: 0.3-beta" << endl;
	printf("──────────────────────────────────────────────────────────────────────────────\n");
}

bool internetConnectionCheck(HANDLE h)
{
	system("cls");
	SetConsoleTextAttribute(h, WHITE);
	cout << "[*] Checking Internet Connectivity before proceeding ... ";
	if (InternetCheckConnection(L"http://www.github.com", FLAG_ICC_FORCE_CONNECTION, 0))
	{
		SetConsoleTextAttribute(h, GREEN);
		cout << "[CONNECTED]";
		Sleep(100);
		return true;
	}
	else
	{
		SetConsoleTextAttribute(h, RED);
		cout << "[DISCONNECTED]\n";
		SetConsoleTextAttribute(h, YELLOW);
		cout << "-- An active internet connection was not found. Do you want to retry? (y/n): ";
		char userOpt{};
		cin >> userOpt;
		SetConsoleTextAttribute(h, BLUE);
		switch (userOpt)
		{
		case 'y':
			return false;
		default:
			exit(1); // Exit the program if not y
		}
	}
}

Package showRepositoryPackageVersion(HANDLE h)
{
	Package retPkg = checkVersionFromRepository();
	if (retPkg.packageName == "NULL")
	{
		SetConsoleTextAttribute(h, YELLOW);
		cout << "\n-- Unable to retrieve latest ADB / Fastboot version from Repository.\n-- Please restart the program.";
		system("pause");
		cleaning(retPkg);
		exit(1);
	}
	else
	{
		SetConsoleTextAttribute(h, CYAN);
		cout << "ADB Version: " << retPkg.adbVersion << endl;
		cout << "Fastboot Version: " << retPkg.fastbootVersion << endl;
		printf("──────────────────────────────────────────────────────────────────────────────\n\n");
		return retPkg;
	}
}

void showSystemWideInstall(HANDLE h)
{
	SetConsoleTextAttribute(h, WHITE);
	cout << "[*] Checking if ADB already installed system wide ... ";
	if (checkSystemInstall())
	{
		SetConsoleTextAttribute(h, RED);
		cout << "[INSTALLED]\n";
		SetConsoleTextAttribute(h, MAGENTA);
		Package exPkg = checkExistingVersion();
		cout << "! ADB Installed Version: " << exPkg.adbVersion << endl;
		cout << "! Fastboot Installed Version: " << exPkg.fastbootVersion << endl;
		SetConsoleTextAttribute(h, YELLOW);
		cout << "-- A system wide install was found. It has to be deleted. Continue (y/n): ";
		SetConsoleTextAttribute(h, BLUE);
		char userOpt{};
		cin >> userOpt;
		switch (userOpt)
		{
		case 'y':
			showDeleteExistingInstall(h);
			break;
		default:
			Package emptPkg;
			emptPkg.packageName = "NULL";
			cleaning(emptPkg);
			exit(1); // Exit the program if not y
		}
	}
	else
	{
		SetConsoleTextAttribute(h, GREEN);
		cout << "[NOT INSTALLED]\n";
	}
}

void showDeleteExistingInstall(HANDLE h)
{
	SetConsoleTextAttribute(h, WHITE);
	cout << "[*] Deleting existing system wide install ... ";
	deleteExistingInstall();
	SetConsoleTextAttribute(h, GREEN);
	cout << "[DONE]\n";
}

void showDownloadLatestVersion(HANDLE h, Package pkg)
{
	SetConsoleTextAttribute(h, WHITE);
	cout << "[*] Downloading version " << pkg.adbVersion << " from Github Repository (" << pkg.packageSize << " kB)" << " ... ";
	if (downloadLatestVersionFromRepository(pkg))
	{
		SetConsoleTextAttribute(h, GREEN);
		cout << "[DONE]\n";
	}
	else
	{
		SetConsoleTextAttribute(h, YELLOW);
		cout << "\n-- Unable to download the file.\n-- Please restart the program. ";
		cleaning(pkg);
		system("pause");
		exit(1);
	}
}

void showDecompressingProcess(HANDLE h, Package pkg)
{
	SetConsoleTextAttribute(h, WHITE);
	cout << "[*] Decompressing downloaded package " << pkg.packageName << " ... ";
	if (decompressRecievedPackage(pkg))
	{
		SetConsoleTextAttribute(h, GREEN);
		cout << "[DONE]\n";
	}
	else
	{
		SetConsoleTextAttribute(h, YELLOW);
		cout << "\n-- Unable to extract the downloaded package (probably corrupt).\n-- Please restart the program. ";
		system("pause");
		cleaning(pkg);
		exit(1);
	}
}

void showInstallationProcess(HANDLE h, Package pkg)
{
	SetConsoleTextAttribute(h, WHITE);
	cout << "[*] Copying files and updating environment ... ";
	if (installationProcedure())
	{
		SetConsoleTextAttribute(h, GREEN);
		cout << "[DONE]\n";
	}
	else
	{
		SetConsoleTextAttribute(h, YELLOW);
		cout << "\n-- Unable to copy the extracted files.\n-- Please restart the program. ";
		system("pause");
		cleaning(pkg);
		exit(1);
	}
}

void showCleaningProcess(HANDLE h, Package pkg)
{
	SetConsoleTextAttribute(h, WHITE);
	cout << "[*] Cleaning all downloaded packages and created temp files ... ";
	cleaning(pkg);
	SetConsoleTextAttribute(h, GREEN);
	cout << "[DONE]\n";
}

void showFinish(HANDLE h)
{
	SetConsoleTextAttribute(h, CYAN);
	cout << "\nSUCCESSFULLY INSTALLED!\nThanks for using this program!\n";
	system("pause");
}