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

Core::COMPortLogger* Kernel::sCOMPortLogger = nullptr;
Core::Driver::Console* Kernel::sConsole = nullptr;
Driver::PIC8259A* Kernel::sPIC = nullptr;

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

    sPIC = new Driver::PIC8259A();
    if (!Core::DeviceManager::RegisterPIC(sPIC)) return false;

    return true;
}

void Kernel::Shutdown()
{
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
