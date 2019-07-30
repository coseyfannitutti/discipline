/****************************************************************************/
/* CRC32 stuff taken and adapted from lib_crc version 1.16 :                */
/*   Library         : lib_crc                                              */
/*   File            : lib_crc.c                                            */
/*   Author          : Lammert Bies  1999-2008                              */
/*   E-mail          : info@lammertbies.nl                                  */
/****************************************************************************/

/* the ATxmega series NVM CRC compatible polynom */
/* on ATxmega you can do this much faster in hardware */
#define                 P_32        0xEDB88320L
#define                 D_32        0xFFFFFFFFL

uint32_t crc_tab32_value(uint8_t address) {
  uint32_t result;
  uint8_t  j;

  result = (uint32_t)address & 0xffL;
  for (j=0; j<8; j++) {
      if (result & 0x00000001L)	result = (result >> 1) ^ P_32;
      else			result = result >> 1;
  }

  return result;
}

uint32_t update_crc_32(uint32_t crc, uint8_t c) {
  uint32_t tmp, long_c;

  long_c = (uint32_t)c & 0xffL;

  tmp = crc ^ long_c;
  crc = (crc >> 8) ^ crc_tab32_value(tmp & 0xffL);

  return crc;
}
