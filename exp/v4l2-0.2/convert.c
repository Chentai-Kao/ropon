#include <stdio.h>

unsigned char sat(int i) {
  return (unsigned char)(i >= 255 ? 255 : (i < 0 ? 0 : i));
}

#define IYUYV2BGR_2(pyuv, pbgr) { \
    int b = (29049 * ((pyuv)[1] - 128)) >> 14; \
    int g = (-5636 * ((pyuv)[1] - 128) - 11698 * ((pyuv)[3] - 128)) >> 14; \
    int r = (22987 * ((pyuv)[3] - 128)) >> 14; \
    (pbgr)[0] = sat(*(pyuv) + b); \
    (pbgr)[1] = sat(*(pyuv) + g); \
    (pbgr)[2] = sat(*(pyuv) + r); \
    (pbgr)[3] = sat((pyuv)[2] + b); \
    (pbgr)[4] = sat((pyuv)[2] + g); \
    (pbgr)[5] = sat((pyuv)[2] + r); \
}
#define IYUYV2BGR_16(pyuv, pbgr) IYUYV2BGR_8(pyuv, pbgr); IYUYV2BGR_8(pyuv + 16, pbgr + 24);
#define IYUYV2BGR_8(pyuv, pbgr) IYUYV2BGR_4(pyuv, pbgr); IYUYV2BGR_4(pyuv + 8, pbgr + 12);
#define IYUYV2BGR_4(pyuv, pbgr) IYUYV2BGR_2(pyuv, pbgr); IYUYV2BGR_2(pyuv + 4, pbgr + 6);

void convert(unsigned char *pyuv, unsigned char *pbgr, int length) {
  unsigned char *pbgr_end = pbgr + length;
  while (pbgr < pbgr_end) {
    IYUYV2BGR_8(pyuv, pbgr);
    pbgr += 3 * 8;
    pyuv += 2 * 8;
  }
}
