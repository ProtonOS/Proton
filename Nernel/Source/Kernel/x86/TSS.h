#pragma once

extern "C" {
extern uint32_t TSSGetRegister(void);
extern void TSSUpdateRegister(uint32_t pDescriptorOffset);
}
