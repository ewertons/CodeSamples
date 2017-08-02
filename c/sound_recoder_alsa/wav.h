#include <stdint.h>

typedef struct
{
    char RIFF_marker[4];
    uint32_t file_size;
    char filetype_header[4];
    char format_marker[4];
    uint32_t data_header_length;
    uint16_t format_type;
    uint16_t number_of_channels;
    uint32_t sample_rate;
    uint32_t bytes_per_second;
    uint16_t bytes_per_frame;
    uint16_t bits_per_sample;
} WaveHeader;

WaveHeader *genericWAVHeader(uint32_t sample_rate, uint16_t bit_depth, uint16_t channels);
WaveHeader *retrieveWAVHeader(const void *ptr);
int writeWAVHeader(int fd, WaveHeader *hdr);
int recordWAV(const char *fileName, WaveHeader *hdr, uint32_t duration);
