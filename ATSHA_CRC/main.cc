#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>

void atCRC(size_t length, const uint8_t *data, uint8_t *crc_le)
{
    size_t counter;
    uint16_t crc_register = 0;
    uint16_t polynom = 0x8005;
    uint8_t shift_register;
    uint8_t data_bit, crc_bit;

    for (counter = 0; counter < length; counter++)
    {
        for (shift_register = 0x01; shift_register > 0x00; shift_register <<= 1)
        {
            data_bit = (data[counter] & shift_register) ? 1 : 0;
            crc_bit = crc_register >> 15;
            crc_register <<= 1;
            if (data_bit != crc_bit)
            {
                crc_register ^= polynom;
            }
        }
    }
    crc_le[0] = (uint8_t)(crc_register & 0x00FF);
    crc_le[1] = (uint8_t)(crc_register >> 8);
}

int main(int argc, char *argv[])
{
  std::cout << "Hello World!" << std::endl;

  // size_t length = 0x21;
  // const uint8_t data[] = {
  //   0x23, 0x01, 0x23, 0x1F, 0x69,
  //   0x00, 0x09, 0x04, 0x00, 0xE2,
  //   0x69, 0x9E, 0xD3, 0xEE, 0x0F,
  //   0x01, 0x00, 0xC8, 0x00, 0x55,
  //   0x00, 0x80, 0x80, 0x80, 0xA1,
  //   0x82, 0xE0, 0xA3, 0x60, 0x94,
  //   0x40, 0xA0, 0x85};  // CRC = 0xD9, 0x60 CRC Ok

  size_t length = 0x05;
  const uint8_t data[] = {0x07, 0x02, 0x00, 0x00, 0x00};

  uint8_t crc[] = {0x00, 0x00};
  printf("crc: 0x%02x 0x%02x\n", crc[0], crc[1]);
  atCRC(length, data, crc);
  printf("crc: 0x%02x 0x%02x\n", crc[0], crc[1]);

  return(0);
}