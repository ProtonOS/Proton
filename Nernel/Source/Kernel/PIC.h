#pragma once

namespace PIC
{
	void Startup();
	void ResetInterrupts(bool pSlave);

	void StartInterrupts();
	void StopInterrupts();
};
