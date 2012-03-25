#pragma once

extern void Halt() __attribute__((noreturn));
void Panic(const char* pMessage) __attribute__((noreturn));
