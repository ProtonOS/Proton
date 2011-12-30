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
    uint8_t data[] = { 0x7B };
    const uint8_t* ptr = data;
    int32_t value = 0;
    ptr = MetaData_GetCompressedSigned(ptr, &value);
    printf("value = %d\n", (int)value);

    ReferenceTypeObject* root = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
    GC* gc = GC_Create(root);
    ReferenceTypeObject* objA = GC_Allocate(gc, root, 2048); // Allocate, data should be from Stack 0
    ReferenceTypeObject* objB = GC_Allocate(gc, objA, 1024); // Allocate, data should be from Stack 0
    ReferenceTypeObject* objC = GC_Allocate(gc, objB, 1024); // Allocate, data should be from Stack 0
    ReferenceTypeObject* objD = GC_Allocate(gc, objC, 1024); // Allocate, data should be from Stack 1
    ReferenceTypeObject* objE = GC_Allocate(gc, objD, 4096); // Allocate, data should be from Stack 2
    ReferenceTypeObject_RemoveReference(objA, objB); // A and B should be unlinked, B, C, D, and E should get disposed
    GC_Collect(gc);
    // At this point B, C, D, and E should all be disposed, they each used at least 25% of their stack
    // And A should still be allocated, with one reference to the root, note that when you DO remove the
    // last reference to the root object, it will be flagged Disposing, but it is never handled by the
    // GC, so it should not matter as the GC only really looks at dependancies, but needs the root so
    // real static and local objects have at least 1 reference to prevent being collected
    if (objA || objB || objC || objD || objE) { }

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
