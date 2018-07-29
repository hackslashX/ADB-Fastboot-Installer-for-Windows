# ADB and Fastboot Installer for Windows

A tiny ADB and Fastboot installer for Windows that fetches the latest version automatically from this github repository. This repository contains the source code, Win32 binary, the ADB / Fastboot binaries and device drivers as zipped package.

## Features

 * Supports Windows Vista and above
 * Small size (292 kB)
 * Fetches the latest version automatically
 * Installs it system wide for ease
 * Ablity to install device USB drivers

## Compatibility Notes

>NOTE: Any previous system-wide installations are removed. The installer warns when it does so.

**1. Requirements**
* Microsoft Windows Vista or Later 
* Active Internet Connection
* Administrative Privileges required

**2. Installation Directory**
* All files are installed in C:\Program Files\hcx-af directory regardless of system architecture

**3. Installation Steps**
* Simply download ``versions\adb_fastboot_installer-x.xs.exe`` and run the executable.
* Follow the onscreen instructions.

## Source Code

All source code is provided AS IS with the repository with no guarantees whatsoever. The program relies on zLib library for decompressing zipped files, RefreshEnv cmd from choco Package Manager to refresh environment path.