#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
	int32_t dooutput;
	int32_t wave_prev;
	int32_t wavesample;
	int32_t waveshift;
	uint8_t wavepan;
	uint8_t is4op2;
	uint32_t noise;
	uint8_t hh_bit1;
	uint8_t hh_bit2;
	uint8_t rhy_bit;
} cqm_t;


void CQM_Reset(cqm_t* chip);
void CQM_Write(cqm_t* chip, uint32_t address, uint8_t data);
void CQM_Generate(cqm_t* chip, int16_t* sample);

#ifdef __cplusplus
}
#endif
