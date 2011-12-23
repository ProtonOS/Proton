#pragma once

#define INTERNAL_CALL_PARAM(offset, type) (*((type*)((uint8_t*)(pParams) + offset)))