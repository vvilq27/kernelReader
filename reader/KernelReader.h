#pragma once

#include <Windows.h>
#include <cstdint>
#include <memory>
#include <string_view>
#include <TlHelp32.h>
#include <mutex>
#include <iostream>
#include "..\util\globals.h"

typedef struct _COPY_MEMORY {
	void* buffer;
	ULONG64		address;
	ULONG		size;
	HANDLE		pid;
	bool		get_pid;
	bool		base;
	bool		peb;
	bool		read;
	bool		write;
	const char* module_name;
	const char* process_name;
}COPY_MEMORY;

namespace Driver
{
	static std::once_flag flag;

	template<typename ... A>
	uint64_t call_hook(const A ... arguments)
	{
		std::call_once(flag, [] { LoadLibrary(L"user32.dll"); });
		void* control_function = GetProcAddress(LoadLibrary(L"win32u.dll"), "NtOpenCompositionSurfaceSectionInfo"); //NtOpenCompositionSurfaceSectionInfo
		const auto control = static_cast<uint64_t(__stdcall*)(A...)>(control_function);
		return control(arguments ...);
	}

	static HANDLE get_process_id(const char* process_name) {
		COPY_MEMORY m{};
		m.get_pid = true;
		m.process_name = process_name;
		call_hook(&m);
		return m.pid;
	}

	static uintptr_t GetModuleBase(HANDLE pID, const char* modName)
	{
		COPY_MEMORY m{};
		m.base = true;
		m.pid = pID;
		m.module_name = modName;
		call_hook(&m);

		return (uintptr_t)m.buffer;
	}

	template<typename type>
	type rpm(uintptr_t readAddress)
	{
		type buffer{};

		COPY_MEMORY m{};
		m.read = true;
		m.pid = Globals::processID;
		m.address = readAddress;
		m.buffer = &buffer;
		m.size = sizeof(type);

		call_hook(&m);
		return buffer;
	}

	char* readString(uintptr_t readAddress)
	{
		char buffer[16];

		COPY_MEMORY m{};
		m.read = true;
		m.pid = Globals::processID;
		m.address = readAddress;
		m.buffer = &buffer;
		m.size = sizeof(buffer);

		call_hook(&m);
		return buffer;
	}

	template<typename type>
	void wpm(UINT_PTR writeAddress, type value)
	{
		COPY_MEMORY m{};
		m.write = true;
		m.pid = Globals::processID;
		m.address = writeAddress;
		m.buffer = &value;
		m.size = sizeof(value);
		call_hook(&m);
	}

	HMODULE GetProcessModuleHandle(const wchar_t* ModuleName, DWORD pid)
	{
		std::cout << "PID: " << pid << std::endl;
		MODULEENTRY32 ModuleInfoPE;
		ModuleInfoPE.dwSize = sizeof(MODULEENTRY32);
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
		Module32First(hSnapshot, &ModuleInfoPE);
		std::wcout << ModuleName << std::endl << std::endl;
		do {
			//std::wcout << ModuleInfoPE.szModule << std::endl;
			if (wcscmp(ModuleInfoPE.szModule, ModuleName) == 0)
			{
				CloseHandle(hSnapshot);
				return ModuleInfoPE.hModule;
			}
		} while (Module32Next(hSnapshot, &ModuleInfoPE));
		CloseHandle(hSnapshot);
		return 0;
	}
};