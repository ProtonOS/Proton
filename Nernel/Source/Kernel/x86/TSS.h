#pragma once

extern "C" {
extern void TSSUpdate(uint32_t pSegmentSelector);
extern uint32_t TSSGetTaskRegister();
}
