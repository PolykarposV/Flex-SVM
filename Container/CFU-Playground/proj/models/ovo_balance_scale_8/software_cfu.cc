#include <stdint.h>
#include "software_cfu.h"

// In this function, place C code to emulate your CFU. You can switch between
// hardware and emulated CFU by setting the CFU_SOFTWARE_DEFINED DEFINE in
// the Makefile.
uint32_t software_cfu(int funct3, int funct7, uint32_t rs1, uint32_t rs2)
{
    return 0;
}
