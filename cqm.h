#pragma once

#include <stdint.h>

typedef struct
{
	uint32_t env;
	uint32_t phase;
	uint32_t fb[2];
} cqmslot_t;

typedef struct
{
	uint8_t regs[256];
	cqmslot_t slotz[48];
	uint8_t oddeven;
	uint8_t reg;
	uint8_t regh;
	uint8_t newm;
	uint8_t rhy;
	uint8_t mode;
	uint8_t key[18];
} cqm_t;

