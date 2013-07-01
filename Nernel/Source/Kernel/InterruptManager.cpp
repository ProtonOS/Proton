#include "InterruptManager.hpp"

namespace InterruptManager
{

    IDT::InterruptHandler sHandlers[IDT::IDTDescriptorMax];


    void RegisterHandler(UInt8 pInterrupt, IDT::InterruptHandler pHandler)
    {
        sHandlers[pInterrupt] = pHandler;
    }

    IDT::InterruptHandler GetHandler(UInt8 pInterrupt)
    {
        return sHandlers[pInterrupt];
    }

}