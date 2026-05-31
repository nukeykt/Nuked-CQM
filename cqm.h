#pragma once

#include <stdint.h>

typedef struct
{
	int32_t env;
	uint32_t phase;
	int32_t mod[2];
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
	uint32_t keyl;
	uint32_t okeyl;
	uint16_t counter;
	uint8_t trem_cnt1;
	uint8_t trem_cnt2;
	uint8_t trem_cnt3;
	uint8_t trem_cnt;
} cqm_t;

