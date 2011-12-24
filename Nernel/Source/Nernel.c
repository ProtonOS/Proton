#include <Nernel.h>
#include <CLR/GC.h>
#include <CLR/AppDomain.h>
#include <CLR/ILReader.h>
#include <CLR/Log.h>

//static AppDomain* global_baseMernelDomain;

void Main(uint32_t pMultiBootMagic,
            void* pMultiBootData)
{
    if (!Nernel_Startup(pMultiBootMagic, pMultiBootData))
    {
        Nernel_Shutdown();
        return;
    }

    ReferenceTypeObject* root = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
    GC* gc = GC_Create(root);
    ReferenceTypeObject* objA = GC_Allocate(gc, 2048); // Allocate, data should be from Stack 0
    ReferenceTypeObject* objB = GC_Allocate(gc, 1024); // Allocate, data should be from Stack 0
    ReferenceTypeObject* objC = GC_Allocate(gc, 1024); // Allocate, data should be from Stack 0
    ReferenceTypeObject* objD = GC_Allocate(gc, 1024); // Allocate, data should be from Stack 1
    ReferenceTypeObject* objE = GC_Allocate(gc, 4096); // Allocate, data should be from Stack 2
    objB->Flags &= ~ReferenceTypeObject_Flags_Active; // Set inactive, ready for collection
    GC_Collect(gc); // Collect, to reduce reference count on inactive refcount 1 objects
    objA->Flags &= ~ReferenceTypeObject_Flags_Active; // Set inactive, ready for collection
    objC->Flags &= ~ReferenceTypeObject_Flags_Active; // Set inactive, ready for collection
    GC_Collect(gc); // Collect, to reduce reference count on inactive refcount 1 objects
    objD->Flags &= ~ReferenceTypeObject_Flags_Active; // Set inactive, ready for collection
    objE->Flags &= ~ReferenceTypeObject_Flags_Active; // Set inactive, ready for collection
    GC_Collect(gc); // Collect, to reduce reference count on inactive refcount 1 objects

    //global_baseMernelDomain = AppDomain_CreateDomain();

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

    SerialWriter_Startup();
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
    SerialWriter_Shutdown();

    MultiBoot_Shutdown();
}
