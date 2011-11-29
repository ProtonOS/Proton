#include <Kernel.h>

extern "C" {
void Kernel(uint32_t pMultiBootMagic, void * pMultiBootData);
}

void Kernel(uint32_t pMultiBootMagic,
            void * pMultiBootData)
{
    if (!MultiBoot::Initialize(pMultiBootMagic, pMultiBootData)) return;

    DeviceManager::Initialize();

    COMPortLogger* comPortLogger = new COMPortLogger();
    if (!DeviceManager::RegisterCOMPortLogger(comPortLogger))
    {
        delete comPortLogger;
        return;
    }

    Console* console = new Console();
    if (!DeviceManager::RegisterConsole(console))
    {
        delete console;
        return;
    }
	DeviceManager::GetConsole().Clear(Console::CreateAttributes(Console::Color::LightWhite, Console::Color::DarkBlack));
    DeviceManager::GetConsole().WriteLine("Booting Proton (" BRANCH ")...");

    while (true) ;
}
