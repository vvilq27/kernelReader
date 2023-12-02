#include "util/globals.h"
#include "util/includes.h"
#include "reader/KernelReader.h"
#include "bf.h"

void mainThread();


int myTeamId;

int main()
{
	while (!Globals::processID)
		Globals::processID = Driver::get_process_id("bf1.exe");

	if (NULL == Globals::processID){
		std::cout << "process not found!";
		return 1;
	}

	std::cout << reinterpret_cast<uintptr_t>(Globals::processID) << std::endl;

	
	//HMODULE moduleBase = Driver::GetProcessModuleHandle(L"ac_client.exe", reinterpret_cast<uintptr_t>(Globals::processID));
	
	Globals::modBase = Driver::GetModuleBase(Globals::processID, "r5apex.exe");
	if (NULL == Globals::modBase) {
		std::cout << "modulebase not found!";
		return 1;
	}

	std::cout << std::hex << Globals::modBase << std::endl;

	Player* p = new Player();

	for (int i = 1; i < 25; i++) {
		char* name = reinterpret_cast<char*>(p->getPlayerName(i));		
		const char* result = strstr(name, "ShellK");

		if (result != nullptr) {
			//std::cout << "name: " << name << std::endl;
			unsigned long long client = p->GetClientPlayerById(i);

			myTeamId = Driver::rpm<uint8_t>(client + 0x1C34);
		}
		delete[] name;
	}

	std::cout << myTeamId << std::endl;

	while (1) {
		for (int i = 1; i < 25; i++) {
			unsigned long long client = p->GetClientPlayerById(i);

			uint64_t clientSoldierEntity = Driver::rpm<Long>(client + 0x1D48);
			uint64_t HealthComponent = Driver::rpm<Long>(clientSoldierEntity + 0x1D0);
			float hp = Driver::rpm<float>(HealthComponent + 0x0020);

			//char* name = Driver::readString(client + 0x40);
			
			//std::cout << name << std::endl;

			int playerTeamId = Driver::rpm<uint8_t>(client + 0x1C34);

			if (playerTeamId != myTeamId && hp > 0.1f) {
				std::cout << "client: " << std::hex << client << std::endl;
				char* name = p->getPlayerName(i);
				/*for (int k = 0; k < 5; k++) {
					std::cout << *(name+k);
				}*/
				std::cout << name <<  std::endl;
				delete[] name;
				//std::cout << "teamId: " << playerTeamId << std::endl;


				//BYTE x[] = { 0xf1 };
				//Driver::wpm(clientSoldierEntity + 0x324, x);
			}
		}

		Sleep(5000);
	}
	


	//char* name = reinterpret_cast<char*>(p->getPlayerName(0));

	//std::cout << Driver::rpm<char>(0x5926ddb0+64);


	
	//Entity = Driver::rpm<uintptr_t>(Globals::modBase + OFFSET_LOCAL_ENT);
	//uint8_t hp = Driver::rpm<uint8_t>(Globals::modBase + 0x4828FC);

	//while (1) {
	//	uint8_t hp = Driver::rpm<uint8_t>(Globals::modBase + 0x4828FC);
	//	std::cout << "my hp: ";
	//	std::cout << std::dec << static_cast<int>(hp) << std::endl;
	//	

	//	Sleep(444);
	//}
	
	/*
	DWORD clientDllAddress = GetProcessId(moduleBase);

	std::cout << "clientDllAddress: " << clientDllAddress << std::endl;
	
	DWORD entityList = clientDllAddress + 0x17B0CF0;
	if (NULL == entityList) {
		std::cout << "entityList not found!";
		return 1;
	}

	DWORD64 EntityListEntry = Driver::rpm<DWORD64>(entityList);
	if ( NULL == EntityListEntry) {
		std::cout << "EntityListEntry not found!";
		return 1;
	}

	std::cout << EntityListEntry << std::endl;

	*/


	/*
	//while (!Globals::modBase)
		//Globals::modBase = Driver::GetModuleBase(Globals::processID, "cs2.exe");


	//Main Thread
	std::thread main(mainThread);

	main.join(); //Start Main Thread


	//Kill Key
	if (GetAsyncKeyState(Globals::killKey))
	{
		for (int i = 0; i < 16000; i++)
		{
			BaseEntity pEntity(i);
			pEntity.Glow(0.f, 0.f, 0.f); //Turning off glow
		}

		std::exit(0);
	}

	*/
}

void mainThread()
{
	while (!GetAsyncKeyState(Globals::killKey))
	{
		
	}
}