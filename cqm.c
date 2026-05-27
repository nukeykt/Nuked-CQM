#include "cqm.h"

static const uint8_t regaccess[48][9] = {
	0
};

static const uint8_t regmap[512] = {
	0
};

void CQM_Generate(cqm_t* chip, int16_t* sample)
{
	int idx;
	for (idx = 0; idx < 48; idx++)
	{
		int wf;
		int phase_in;
		int phase;

		const uint8_t* access = regaccess[idx];
		cqmslot_t* slot = &chip->slotz[idx];

		wf = chip->regs[access[0]];

		phase = slot->phase >> 3;

		phase_in = phase;

	}

	chip->oddeven ^= 1;
}

void CQM_Write(cqm_t* chip, uint32_t address, uint8_t data)
{
	if (address & 1)
	{
		int regh = chip->regh;
		int reg = chip->reg;
		int index;
		if (regh && reg == 5)
		{
			chip->newm = data & 1;
			return;
		}
		regh &= chip->newm;

		index = regmap[(regh << 8) + reg];
		if (index != 0xff)
		{
			if (!chip->newm)
			{
				if ((reg & 0xf0) == 0xc0)
				{
					data |= 0x30;
				}
				if ((reg & 0xe0) == 0xe0)
				{
					data &= 0xfb;
				}
			}
			if ((reg & 0xf0) == 0xb0)
			{
				chip->key[regh * 9 + (reg & 0xf)] = (data >> 5) & 1;
			}
			chip->regs[index] = data;
			return;
		}
		if (!regh && reg == 0xbd)
		{
			chip->rhy = data;
			return;
		}
		if (regh && reg == 0x4)
		{
			chip->mode = data & 0x3f;
			return;
		}
	}
	else
	{
		chip->reg = data;
		chip->regh = (address >> 1) & 1;
	}
}

uint8_t CQM_Write(cqm_t* chip, uint32_t address)
{
	return 0;
}
