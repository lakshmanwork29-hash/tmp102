/*
 * test_tmp102.c - checks tmp102_raw_to_celsius() against Table 6-2
 */
#include <stdint.h>
#include <stdio.h>
#include "test_tmp102.h"

float tmp102_raw_to_celsius(uint16_t reg){
	
   int16_t raw = reg;

   // Check whether extended mode is enabled
    if (raw & TMP102_MODE_BIT) {
        raw = (reg >> TMP102_EXTENDED_SHIFT) & TMP102_EXTENDED_MASK;   // Extract the temperature bits in extended mode.
   
		// Check the sign bit to determine whether the temperature is negative
        if (reg & TMP102_SIGN_BIT) {
            raw = ~raw;
            raw = (raw & TMP102_EXTENDED_MASK) + 1;  // 2s complement 
            return -(float)raw * TMP102_TEMP_RESOLUTION;
        }

        return (float)raw * TMP102_TEMP_RESOLUTION;
    }
	// Decode temperature in normal mode 
    else {
        raw = (reg >> TMP102_NORMAL_SHIFT) & TMP102_NORMAL_MASK;   // Extract the temperature bits in normal mode.

        if (reg & TMP102_SIGN_BIT) {
            raw = ~raw;
            raw = (raw & TMP102_NORMAL_MASK) + 1;
            return -(float)raw * TMP102_TEMP_RESOLUTION;
        }

        return (float)raw * TMP102_TEMP_RESOLUTION;
    }


}

struct tmp102_case {
	uint16_t reg;	/* register value = 12-bit value << 4 */
	float expect;
};

static const struct tmp102_case cases[] = {
	{ 0x7FF0u, 127.9375f },
	{ 0x1900u, 25.0f },
	{ 0x0000u, 0.0f },
	{ 0xFFC0u, -0.25f },
	{ 0xE700u, -25.0f },
	{ 0xC900u, -55.0f },
};

int main(void)
{
	unsigned int i;
	int fails = 0;

	for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
		float got = tmp102_raw_to_celsius(cases[i].reg);

		if (got != cases[i].expect) {
			printf("FAIL 0x%04X: got %f, expected %f\n",
			       cases[i].reg, got, cases[i].expect);
			fails++;
		} else {
			printf("ok   0x%04X -> %8.4f C\n", cases[i].reg, got);
		}
	}
	return (fails == 0) ? 0 : 1;
}
