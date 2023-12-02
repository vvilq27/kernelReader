#pragma once

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include "util/globals.h"
#include "util/includes.h"
#include "reader/KernelReader.h"

#define Long unsigned long long
#define OFFSET_OBFUSCATIONMGR 0x14351D058
#define OFFSET_CLIENTGAMECONTEXT 0x1437F7758
#define ValidPointer( pointer ) ( pointer != NULL && (DWORD_PTR)pointer >= 0x10000 && (DWORD_PTR)pointer < 0x00007FFFFFFEFFFF /*&& some other checks*/ )

class Player
{
public:
	Player()
	{
	}

	typedef struct
	{
		unsigned long long ClientPlayer, ClientVehicleEntity;
		char* PlayerName, * VehicleID, * VehicleName;
		bool isInVehicle;
	}PlayerInfo;


	Long GetClientPlayerById(int id)
	{
		Long pClientGameContext = Driver::rpm<Long>(OFFSET_CLIENTGAMECONTEXT);
		if (!ValidPointer(pClientGameContext))
		{
			return 0;
		}

		Long pPlayerManager = Driver::rpm<Long>(pClientGameContext + 0x68);
		if (!ValidPointer(pPlayerManager))
		{
			return 0;
		}

		Long pObfuscationMgr = Driver::rpm<Long>(OFFSET_OBFUSCATIONMGR);
		if (!ValidPointer(pObfuscationMgr))
		{
			return 0;
		}

		Long PlayerListXorValue = Driver::rpm<Long>(pPlayerManager + 0xF8);
		Long PlayerListKey = PlayerListXorValue ^ Driver::rpm<Long>(pObfuscationMgr + 0x70);

		Long mpBucketArray = Driver::rpm<Long>(pObfuscationMgr + 0x10);


		int mnBucketCount = Driver::rpm<int>(pObfuscationMgr + 0x18);
		if (mnBucketCount == 0)
		{
			return 0;
		}

		int startCount = (int)PlayerListKey % mnBucketCount;

		Long mpBucketArray_startCount = Driver::rpm<Long>(mpBucketArray + (Long)(startCount * 8));
		Long node_first = Driver::rpm<Long>(mpBucketArray_startCount);
		Long node_second = Driver::rpm<Long>(mpBucketArray_startCount + 0x8);
		Long node_mpNext = Driver::rpm<Long>(mpBucketArray_startCount + 0x10);

		while (PlayerListKey != node_first)
		{
			mpBucketArray_startCount = node_mpNext;

			node_first = Driver::rpm<Long>(mpBucketArray_startCount);
			node_second = Driver::rpm<Long>(mpBucketArray_startCount + 0x8);
			node_mpNext = Driver::rpm<Long>(mpBucketArray_startCount + 0x10);
		}

		Long EncryptedPlayerMgr = node_second;
		return EncryptedPlayerMgr_GetPlayer(EncryptedPlayerMgr, id);

	}
	Long EncryptedPlayerMgr_GetPlayer(Long ptr, int id)
	{
		Long XorValue1 = Driver::rpm<Long>(ptr + 0x20) ^ Driver::rpm<Long>(ptr + 0x8);
		Long XorValue2 = XorValue1 ^ Driver::rpm<Long>(ptr + 0x10);
		if (!ValidPointer(XorValue2))
		{
			return 0;
		}

		return XorValue1 ^ Driver::rpm<Long>(XorValue2 + 0x8 * id);
	}

	char* getPlayerName(int id) {
		unsigned long long client = GetClientPlayerById(id);

		//std::cout << "client: " << std::hex << client << std::endl;

		char* name = Driver::readString(client + 0x40);
		std::cout << name << std::endl;

		//char* name = new char[10]; // Allocate memory (replace with appropriate size)
		//strcpy_s(name, 10, nameDrv);

		return name;
	}

private:
	const int OFFSET_PlayerName = 0x18;
	const int OFFSET_ClientVehicleEntity = 0x1D38;
	const int OFFSET_ClientSoldierEntity = 0x1D48;
	const int OFFSET_VehicleEntityData = 0x30;
	const int OFFSET_VeniceVehicleCustomizationAsset = 0x120;
	const int OFFSET_VehicleID = 0x10;
	const int OFFSET_gameBase = 0x140000000;

	//unsigned long long EncryptedPlayerMgr_GetPlayer(unsigned long long ptr, int id);
	//unsigned long long GetClientPlayerById(int id);
};

