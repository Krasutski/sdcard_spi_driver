/* Includes ------------------------------------------------------------------*/
#include "spi_sdcard_driver_config.h"
#include "spi_sdcard_crc7.h"

#if SD_CRC7_MODE == SD_CRC7_MODE_RAM_TABLE || SD_CRC7_MODE == SD_CRC7_MODE_ROM_TABLE

#  if SD_CRC7_MODE == CRC7_MODE_RAM_TABLE
uint8_t crc7_table[256];

void crc7_generate_table(void) {
    int i, j;
    uint8_t poly = 0x89;  // the value of our CRC-7 polynomial

    for (i = 0; i < 256; ++i) {
        crc7_table[i] = (i & 0x80) ? i ^ poly : i;

        for (j = 1; j < 8; ++j) {
            crc7_table[i] <<= 1;

            if (crc7_table[i] & 0x80) {
                crc7_table[i] ^= poly;
            }
        }
    }
}
#  else// SD_CRC7_MODE == SD_CRC7_MODE_ROM_TABLE
const uint8_t crc7_table[256] = {
    0x00, 0x09, 0x12, 0x1B, 0x24, 0x2D, 0x36, 0x3F, 0x48, 0x41, 0x5A, 0x53, 0x6C, 0x65, 0x7E, 0x77,
    0x19, 0x10, 0x0B, 0x02, 0x3D, 0x34, 0x2F, 0x26, 0x51, 0x58, 0x43, 0x4A, 0x75, 0x7C, 0x67, 0x6E,
    0x32, 0x3B, 0x20, 0x29, 0x16, 0x1F, 0x04, 0x0D, 0x7A, 0x73, 0x68, 0x61, 0x5E, 0x57, 0x4C, 0x45,
    0x2B, 0x22, 0x39, 0x30, 0x0F, 0x06, 0x1D, 0x14, 0x63, 0x6A, 0x71, 0x78, 0x47, 0x4E, 0x55, 0x5C,
    0x64, 0x6D, 0x76, 0x7F, 0x40, 0x49, 0x52, 0x5B, 0x2C, 0x25, 0x3E, 0x37, 0x08, 0x01, 0x1A, 0x13,
    0x7D, 0x74, 0x6F, 0x66, 0x59, 0x50, 0x4B, 0x42, 0x35, 0x3C, 0x27, 0x2E, 0x11, 0x18, 0x03, 0x0A,
    0x56, 0x5F, 0x44, 0x4D, 0x72, 0x7B, 0x60, 0x69, 0x1E, 0x17, 0x0C, 0x05, 0x3A, 0x33, 0x28, 0x21,
    0x4F, 0x46, 0x5D, 0x54, 0x6B, 0x62, 0x79, 0x70, 0x07, 0x0E, 0x15, 0x1C, 0x23, 0x2A, 0x31, 0x38,
    0x41, 0x48, 0x53, 0x5A, 0x65, 0x6C, 0x77, 0x7E, 0x09, 0x00, 0x1B, 0x12, 0x2D, 0x24, 0x3F, 0x36,
    0x58, 0x51, 0x4A, 0x43, 0x7C, 0x75, 0x6E, 0x67, 0x10, 0x19, 0x02, 0x0B, 0x34, 0x3D, 0x26, 0x2F,
    0x73, 0x7A, 0x61, 0x68, 0x57, 0x5E, 0x45, 0x4C, 0x3B, 0x32, 0x29, 0x20, 0x1F, 0x16, 0x0D, 0x04,
    0x6A, 0x63, 0x78, 0x71, 0x4E, 0x47, 0x5C, 0x55, 0x22, 0x2B, 0x30, 0x39, 0x06, 0x0F, 0x14, 0x1D,
    0x25, 0x2C, 0x37, 0x3E, 0x01, 0x08, 0x13, 0x1A, 0x6D, 0x64, 0x7F, 0x76, 0x49, 0x40, 0x5B, 0x52,
    0x3C, 0x35, 0x2E, 0x27, 0x18, 0x11, 0x0A, 0x03, 0x74, 0x7D, 0x66, 0x6F, 0x50, 0x59, 0x42, 0x4B,
    0x17, 0x1E, 0x05, 0x0C, 0x33, 0x3A, 0x21, 0x28, 0x5F, 0x56, 0x4D, 0x44, 0x7B, 0x72, 0x69, 0x60,
    0x0E, 0x07, 0x1C, 0x15, 0x2A, 0x23, 0x38, 0x31, 0x46, 0x4F, 0x54, 0x5D, 0x62, 0x6B, 0x70, 0x79
};
#  endif //SD_CRC7_MODE == SD_CRC7_MODE_RAM_TABLE

static uint8_t _crc7_get_item(uint8_t init_crc, uint8_t message_byte) {
    return crc7_table[(init_crc << 1) ^ message_byte];
}

uint8_t crc7(const uint8_t message[], size_t length) {

    uint8_t crc = 0;

    for (size_t i = 0; i < length; ++i) {
        crc = _crc7_get_item(crc, message[i]);
    }

    return crc;
}
#elif SD_CRC7_MODE == SD_CRC7_MODE_RUNTIME

uint8_t crc7(const uint8_t  message[], size_t length) {
    const uint8_t poly = 0x89;
    uint8_t crc = 0;

    for (size_t i = 0; i < length; i++) {
        crc ^= message[i];

        for (size_t j = 0; j < 8; j++) {
            crc = (crc & 0x80u) ? ((crc << 1) ^ (poly << 1)) : (crc << 1);
        }
    }

    return crc >> 1;
}

#else
#error set corret SD_CRC7_MODE
#endif

