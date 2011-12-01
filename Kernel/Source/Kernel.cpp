#include <Kernel.h>

extern "C" {
    void Main(uint32_t pMultiBootMagic,
              void * pMultiBootData)
    {
        if (!Kernel::Startup(pMultiBootMagic, pMultiBootData))
        {
            Kernel::Shutdown();
            return;
        }
        while (true) ;
    }
}

COMPortLogger* Kernel::sCOMPortLogger = nullptr;
Console* Kernel::sConsole = nullptr;

bool Kernel::Startup(uint32_t pMultiBootMagic,
                     void * pMultiBootData)
{
    if (!MultiBoot::Startup(pMultiBootMagic, pMultiBootData)) return false;

    if (!DeviceManager::Startup()) return false;

    sCOMPortLogger = new COMPortLogger();
    if (!DeviceManager::RegisterCOMPortLogger(sCOMPortLogger)) return false;

    sConsole = new Console();
    if (!DeviceManager::RegisterConsole(sConsole)) return false;

	DeviceManager::GetConsole().Clear(Console::CreateAttributes(ConsoleColor::LightWhite, ConsoleColor::DarkBlack));
    DeviceManager::GetConsole().WriteLine("Starting Proton (" BRANCH ")...");

    if (!FileSystemManager::Startup()) return false;

    printf("Debug: printf works at this point\n");

    if (!GDT::Startup()) return false;
    if (!IDT::Startup()) return false;

    return true;
}

void Kernel::Shutdown()
{
    IDT::Shutdown();
    GDT::Shutdown();

    FileSystemManager::Shutdown();

    if (sConsole)
    {
        DeviceManager::UnregisterConsole(sConsole);
        delete sConsole;
        sConsole = nullptr;
    }

    if (sCOMPortLogger)
    {
        DeviceManager::UnregisterCOMPortLogger(sCOMPortLogger);
        delete sCOMPortLogger;
        sCOMPortLogger = nullptr;
    }

    DeviceManager::Shutdown();

    MultiBoot::Shutdown();
}
