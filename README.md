# ADB and Fastboot Installer for Windows

A tiny ADB and Fastboot installer for Windows that fetches the latest version automatically from this github repository. This repository contains the source code, Win32 binary and the ADB / Fastboot binaries as zipped package.

## Features

 * Support for all Windows versions later than Windows XP
 * Small size (334 kB)
 * Fetches the latest version automatically
 * Installs it system wide for ease

## Compatibility Notes

>NOTE: All previous instances of ADB and Fastboot are removed if the user continues to install regardless if already installed (the installer does warns)`

**1. Requirements**
* Microsoft Windows XP or Later 
* Active Internet Connection
* Administrative Privileges required

**2. Notes for x86 version**
* All files are copied to System32 directory.

**3. Notes for x64 version**
* adb.exe and fastboot.exe are copied to Windows directory and the rest are copied to SysWOW64 directory.

**4. Installation Steps**
* Simply download ``adb_fastboot_installer.exe`` and run the executable.
* Follow the onscreen instructions.

## Source Code

All source code is provided AS IS with the repository with no guarantees whatsoever. The program relies on zLib library for decompressing zipped files.