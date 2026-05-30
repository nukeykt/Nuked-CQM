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
		int wave;

		const uint8_t* access = regaccess[idx];
		cqmslot_t* slot = &chip->slotz[idx];

		wf = chip->regs[access[0]];

		phase = slot->phase >> 3;

		phase_in = phase;

		{
			int16_t mulb = 0;
			int16_t mula = 0;
			switch (wf)
			{
				case 0:
					mulb = phase_in;
					mula = mulb & 0x7fff;
					if (!(phase_in & 0x8000))
					{
						mula ^= 0x7fff;
					}
					break;
				case 1:
					mulb = phase_in;
					mula = mulb ^ 0x8000;
					if (!(phase_in & 0x8000))
					{
						mula = 0;
					}
					break;
				case 2:
					mulb = phase_in;
					mula = mulb ^ 0x8000;
					break;
				case 3:
					mulb = phase_in;
					mula = mulb ^ 0x8000;
					if (phase_in & 0x4000)
					{
						mula = 0;
					}
					break;
				case 4:
					mulb = (phase_in << 1) & 0x7fff;
					if (!(phase_in & 0x4000))
					{
						mulb |= 0x8000;
					}
					mula = mulb & 0x7fff;
					if (phase_in & 0x4000)
					{
						mula ^= 0x7fff;
					}
					if (!(phase_in & 0x8000))
					{
						mula = 0;
					}
					break;
				case 5:
					mulb = (phase_in << 1) & 0x7fff;
					if (!(phase_in & 0x4000))
					{
						mulb |= 0x8000;
					}
					mula = mulb ^ 0x8000;
					if (!(phase_in & 0x8000))
					{
						mula = 0;
					}
					break;
				case 6:
					mulb = 0x3fff;
					mula = mulb ^ 0x8000;
					if (!(phase_in & 0x8000))
					{
						mula &= 0x7fff;
					}
					break;
				case 7:
					mulb = phase_in & 0x3fff;
					if (!(phase_in & 0x4000))
					{
						mulb |= 0xc000;
					}
					mula = mulb & 0x7fff;
					if (!(phase_in & 0x4000))
					{
						mula ^= 0x7fff;
					}
					if (!((phase_in ^ (phase_in >> 1)) & 0x4000))
					{
						mula = 0;
					}
					break;
			}

			mulb >>= 2;
			if (mulb < 0)
				mulb++;

			wave = (mula * mulb) >> 12;
		}

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

uint8_t CQM_Read(cqm_t* chip, uint32_t address)
{
	return 0;
}
