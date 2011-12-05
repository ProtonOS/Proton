#include <Kernel.h>

extern "C" {
	
    void* GetModuleAddress(int i);

    void* GetModuleAddress(int i)
    {
	    return reinterpret_cast<void*>(Core::MultiBoot::GetLoadedModule(i).Address);
    }

    #include "Runtime/CLIFile.h"
    #include "Runtime/MetaData.h"
    #include "Runtime/Type.h"
    #include "Runtime/Heap.h"
    #include "Runtime/Finalizer.h"

    void Main(uint32_t pMultiBootMagic,
              void * pMultiBootData)
    {
        if (!Kernel::Startup(pMultiBootMagic, pMultiBootData))
        {
            Kernel::Shutdown();
            return;
        }
		
		JIT_Execute_Init();
		MetaData_Init();
		Type_Init();
		Heap_Init();
		Finalizer_Init();

		tCLIFile *pCLIFile;
		char* args = (char*)"";
		int retValue;
		pCLIFile = CLIFile_Load((char*)"mernel.exe");
		
		retValue = CLIFile_Execute(pCLIFile, 0, &args );

		printf("Execution finished, return %i", retValue);

        while (true) ;
    }
}

Core::COMPortLogger* Kernel::sCOMPortLogger = nullptr;
Core::Driver::Console* Kernel::sConsole = nullptr;
Driver::PIC8259A* Kernel::sPIC = nullptr;
Driver::PIT8254* Kernel::sPIT = nullptr;
Driver::RTC146818A* Kernel::sRTC = nullptr;

bool Kernel::Startup(uint32_t pMultiBootMagic,
                     void * pMultiBootData)
{
    if (!Core::MultiBoot::Startup(pMultiBootMagic, pMultiBootData)) return false;

    if (!Core::DeviceManager::Startup()) return false;

    sCOMPortLogger = new Core::COMPortLogger();
    if (!Core::DeviceManager::RegisterCOMPortLogger(sCOMPortLogger)) return false;

    sConsole = new Core::Driver::Console();
    if (!Core::DeviceManager::RegisterConsole(sConsole)) return false;

	Core::DeviceManager::GetConsole().Clear(Core::Driver::Console::CreateAttributes(Core::Driver::ConsoleColor::LightWhite, Core::Driver::ConsoleColor::DarkBlack));
    Core::DeviceManager::GetConsole().WriteLine("Starting Proton (" BRANCH ")...");

    if (!Core::FileSystemManager::Startup()) return false;

    if (!Core::GDT::Startup()) return false;
    if (!Core::IDT::Startup()) return false;

    sRTC = new Driver::RTC146818A();
    if (!Core::DeviceManager::RegisterRTC(sRTC)) return false;

    sPIC = new Driver::PIC8259A();
    if (!Core::DeviceManager::RegisterPIC(sPIC)) return false;

    sPIT = new Driver::PIT8254(100);
    if (!Core::DeviceManager::RegisterPIT(sPIT)) return false;

    if (!Core::SystemClock::Startup()) return false;

	time_t startupTime = time(NULL);
    printf("Startup @ %s", ctime(&startupTime));

    return true;
}

void Kernel::Shutdown()
{
    Core::SystemClock::Shutdown();

    if (sRTC)
    {
        Core::DeviceManager::UnregisterRTC(sRTC);
        delete sRTC;
        sRTC = nullptr;
    }

    if (sPIT)
    {
        Core::DeviceManager::UnregisterPIT(sPIT);
        delete sPIT;
        sPIT = nullptr;
    }

    if (sPIC)
    {
        Core::DeviceManager::UnregisterPIC(sPIC);
        delete sPIC;
        sPIC = nullptr;
    }
    
    Core::IDT::Shutdown();
    Core::GDT::Shutdown();

    Core::FileSystemManager::Shutdown();

    if (sConsole)
    {
        Core::DeviceManager::UnregisterConsole(sConsole);
        delete sConsole;
        sConsole = nullptr;
    }

    if (sCOMPortLogger)
    {
        Core::DeviceManager::UnregisterCOMPortLogger(sCOMPortLogger);
        delete sCOMPortLogger;
        sCOMPortLogger = nullptr;
    }

    Core::DeviceManager::Shutdown();

    Core::MultiBoot::Shutdown();
}
