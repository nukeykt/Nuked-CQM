#include "cqm.h"

static const uint8_t regaccess[48][9] = {
	0
};

static const uint8_t regmap[512] = {
	0
};

static inline int doshifter(int x, int shift)
{
	if (shift > 12)
		return x << (shift - 12);
	return x >> (12 - shift);
}

void CQM_Generate(cqm_t* chip, int16_t* sample)
{
	int idx;
	int multi_l = 0;
	int con = 0;
	int con_prev = 0;
	int isrhy = (chip->rhy & 0x20) != 0;

	chip->okeyl = chip->keyl;
	chip->keyl = (chip->key[0] << 0);
	chip->keyl |= (chip->key[1] << 1);
	chip->keyl |= (chip->key[2] << 2);
	chip->keyl |= (chip->key[3] << 3);
	chip->keyl |= (chip->key[4] << 4);
	chip->keyl |= (chip->key[5] << 5);
	chip->keyl |= (chip->key[6] << 6);
	chip->keyl |= (chip->key[7] << 7);
	chip->keyl |= (chip->key[7] << 8);
	chip->keyl |= (chip->key[8] << 9);
	chip->keyl |= (chip->key[8] << 10);
	chip->keyl |= (chip->key[9] << 11);
	chip->keyl |= (chip->key[10] << 12);
	chip->keyl |= (chip->key[11] << 13);
	chip->keyl |= (chip->key[12] << 14);
	chip->keyl |= (chip->key[13] << 15);
	chip->keyl |= (chip->key[14] << 16);
	chip->keyl |= (chip->key[15] << 17);
	chip->keyl |= (chip->key[16] << 18);
	chip->keyl |= (chip->key[17] << 19);
	if (isrhy)
	{
		if (chip->rhy & 1)
			chip->keyl |= 1 << 7;
		if (chip->rhy & 2)
			chip->keyl |= 1 << 10;
		if (chip->rhy & 4)
			chip->keyl |= 1 << 9;
		if (chip->rhy & 8)
			chip->keyl |= 1 << 8;
		if (chip->rhy & 16)
			chip->keyl |= 1 << 6;
	}

	for (idx = 0; idx < 48; idx++)
	{
		int wave;

		int ch = 0;
		int op = 0;
		int is4op = 0;

		const uint8_t* access = regaccess[idx];
		cqmslot_t* slot = &chip->slotz[idx];

		int32_t *modptr;

		int keybit = 0;

		{
			int idx2 = idx;
			if (idx2 >= 24)
			{
				idx2 -= 24;
				ch += 9;
			}

			if (idx2 < 12)
			{
				ch += idx2 >> 2;
				op = idx2 & 3;

				if (ch < 9)
					is4op = (chip->mode >> ch) & 1;
				else
					is4op = (chip->mode >> (ch - 6)) & 1;
			}
			else
			{
				ch += (idx2 >> 1) - 3;
				op = idx2 & 1;
			}
		}

		if (op == 0)
			modptr = chip->slotz[idx + 1].mod;
		else
			modptr = chip->slotz[idx].mod;

		if (ch < 6)
			keybit = ch;
		else if (ch < 9)
			keybit = ch * 2 + op - 6;
		else
			keybit = ch + 2;

		int key = (chip->keyl >> keybit) & 1;
		int kon = key && !(chip->okeyl & (1 << keybit));

		{
			int wf;
			int phase_in;
			int phase;
			wf = chip->regs[access[0]];
			phase = slot->phase >> 3;

			phase_in = phase;
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

		if (kon)
		{
			slot->mod[0] = slot->mod[1] = 0;
		}

		int pmulti = chip->regs[access[4]];

		int multi = (isrhy && ch == 7 && op == 1) ? multi_l : pmulti & 15;

		multi_l = pmulti & 15;

		if (op == 2)
		{
			con_prev = con;
		}
		int fb_con = chip->regs[access[1]];
		con = fb_con & 1;

		int phase = slot->phase;

		int modsub = slot->mod[chip->oddeven ^ 1];
		int modadd = slot->mod[chip->oddeven];

		int fb = (fb_con >> 1) & 7;
		if ((op == 0 && fb == 0) || x) // no modulation
		{
			modsub = 0;
			modadd = 0;
		}
		int modshift = 0;
		if (op == 0)
		{
			modshift = fb + 6;
		}
		else
		{
			modshift = 15;
		}

		modsub = doshifter(modsub, modshift);
		modadd = doshifter(modadd, modshift);
		if (op == 0)
		{
			phase += modsub;
			phase -= modadd;
		}
		else
		{
			phase -= modsub;
			phase += modadd;
		}

		int fnum_lo = chip->regs[access[2]];
		int fnum_hi = chip->regs[access[3]];

		int fnum = fnum_lo | ((fnum_hi & 3) << 8);
		int block = (fnum_hi >> 2) & 7;

		// fnum
		int fnum_ps = fnum << 8;
		if (block == 0)
			fnum_ps &= ~256; // mask fnum0

		static const multi_shift_add1[] = {
			0, 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5
		};

		static const multi_shift_add2[] = {
			0, 0, 0, 1, 0, 1, 2, -1, 0, 1, 2, 2, 3, 3, -1, -1
		};

		int shift_l = block + 2 + multi_shift_add1[multi];

		phase += doshifter(fnum_ps, shift_l);

		if (multi_shift_add2[multi] != 0)
		{
			int mshift = multi_shift_add2[multi];
			int neg = 0;
			if (mshift < 0)
			{
				mshift = -mshift;
				neg = 1;
			}
			int shift_h = block + 2 + mshift;

			if (neg)
				phase -= doshifter(fnum_ps, shift_h);
			else
				phase += doshifter(fnum_ps, shift_h);
		}

		// vibrato
		if (pmulti & 0x40)
		{
			int fnum_h = (fnum & 0x380) << 1;

			int shift_add = 0;
			if ((chip->counter & 0x600) == 0x400)
				shift_add++;
			if (chip->rhy & 0x80)
				shift_add++;

			int shift_l = block + shift_add + multi_shift_add1[multi];
			int vibneg = (chip->counter >> 11) & 1;
			if (vibneg)
				phase -= doshifter(fnum_h, shift_l);
			else
				phase += doshifter(fnum_h, shift_l);

			if (multi_shift_add2[multi] != 0)
			{
				int mshift = multi_shift_add2[multi];
				int neg = 0;
				if (mshift < 0)
				{
					mshift = -mshift;
					neg = 1;
				}
				int shift_h = block + shift_add + mshift;

				if (neg ^ vibneg)
					phase -= doshifter(fnum_h, shift_h);
				else
					phase += doshifter(fnum_h, shift_h);
			}

		}

		if (kon)
		{
			slot->phase = 0x40000;
		}
		else
		{
			slot->phase = phase;
		}
	}

	chip->oddeven ^= 1;
	if (chip->oddeven == 0)
	{
		chip->counter++;
	}
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

void CQM_Reset(cqm_t* chip)
{
	memset(chip, 0, sizeof(cqm_t));

	chip->counter = 0xfff;
}