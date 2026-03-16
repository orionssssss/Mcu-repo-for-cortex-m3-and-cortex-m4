#ifndef _BSP_BUFFER_H__
#define _BSP_BUFFER_H__

#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

typedef enum {
    CommonBuffer = 0,
    DoubleBuffer,
    CircularBuffer,
} BufferType_t;

typedef struct
{
    uint8_t *data;
    uint16_t head;
    uint16_t tail;
} CircularBuffer_t;

typedef struct
{
    uint8_t *data;
} CommonBuffer_t;

typedef struct
{
    uint8_t *data[2];
    uint16_t idx;
} DoubleBuffer_t;

typedef struct
{
    BufferType_t type;
    union {
        CircularBuffer_t circularBuffer;
        CommonBuffer_t commonBuffer;
        DoubleBuffer_t doubleBuffer;
    }buffer_body;
    uint16_t size;
    uint32_t superclass;
} Buffer_t;

typedef struct
{
    BufferType_t type;
    uint16_t size;
    uint32_t superclass;
} BufferConfig_t;

Buffer_t* Buffer_Init(BufferConfig_t *buffer_config);
bool Buffer_Write(Buffer_t *buffer, uint8_t *data, uint16_t size);
bool Buffer_Read(Buffer_t *buffer, uint8_t *data, uint16_t size);

#endif /* _BSP_BUFFER_H__ */