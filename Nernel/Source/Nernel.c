#include <Nernel.h>

void Main(uint32_t pMultiBootMagic,
            void* pMultiBootData)
{
    if (!Nernel_Startup(pMultiBootMagic, pMultiBootData))
    {
        Nernel_Shutdown();
        return;
    }
    MultiBoot_LoadedModule* loadedModule = MultiBoot_GetLoadedModuleByFileName("corlib.dll");
    PEFile* peFile = PEFile_Create((uint8_t*)loadedModule->Address, loadedModule->Length);
    if (peFile)
    {
        CLIFile* cliFile = CLIFile_Create(peFile);
        if (cliFile)
        {
            uint16_t streamCount = MetaData_GetStreamCount(cliFile->MetaDataHeader);
            for (uint16_t streamIndex = 0; streamIndex < streamCount; ++streamIndex)
            {
                //uint32_t streamRVA = MetaData_GetStreamRVA(cliFile->MetaDataHeader, streamIndex);
                //uint32_t streamSize = MetaData_GetStreamSize(cliFile->MetaDataHeader, streamIndex);
                const char* streamName = MetaData_GetStreamName(cliFile->MetaDataHeader, streamIndex);
                uint8_t* streamData = MetaData_GetStreamData(cliFile->MetaDataHeader, streamIndex);

                if (!strcmp(streamName, "#~")) cliFile->Tables = streamData;
                else if (!strcmp(streamName, "#Strings")) cliFile->StringsHeap = streamData;
                else if (!strcmp(streamName, "#US")) cliFile->UserStringsHeap = streamData;
                else if (!strcmp(streamName, "#Blob")) cliFile->BlobsHeap = streamData;
                else if (!strcmp(streamName, "#GUID")) cliFile->GUIDsHeap = streamData;

                printf("Loaded Stream, Name %s\n", CLIFile_GetString(cliFile, 10));
            }
        }
    }
    /*
	tCLIFile* cliFile;
	const char* args = "";
	cliFile = CLIFile_Load("mernel.exe");
	CLIFile_Execute(cliFile, 0, &args);
    */
    while (TRUE) ;
}

bool_t Nernel_Startup(uint32_t pMultiBootMagic,
                      void* pMultiBootData)
{
    if (!MultiBoot_Startup(pMultiBootMagic, pMultiBootData)) return FALSE;

    Console_Startup();
    Console_WriteLine("Nernel: Starting Proton (" BRANCH ")...");

    GDT_Startup();
    IDT_Startup();

    RTC_Startup();
    PIC_Startup();
    PIT_Startup(100);
    SystemClock_Startup();

	time_t startupTime = time(NULL);
    printf("Nernel: Startup @ %s", ctime(&startupTime));

    /*
	JIT_Execute_Init();
	MetaData_Init();
	Type_Init();
	Heap_Init();
	Finalizer_Init();
    */
    return TRUE;
}

void Nernel_Shutdown()
{
    SystemClock_Shutdown();
    PIT_Shutdown();
    PIC_Shutdown();
    RTC_Shutdown();

    IDT_Shutdown();
    GDT_Shutdown();

    Console_Shutdown();

    MultiBoot_Shutdown();
}
