#include "bsp_buffer.h"
#include <stdlib.h>
#include <string.h>
#include "bsp_log.h"

static bool CircularBuffer_Write(Buffer_t *_buffer, uint8_t *data, uint16_t size)
{
    uint16_t clear_area = 0;
    if (_buffer->buffer_body.circularBuffer.head > _buffer->buffer_body.circularBuffer.tail) {
        clear_area = _buffer->buffer_body.circularBuffer.head - _buffer->buffer_body.circularBuffer.tail;
        if (clear_area > size) {
            memcpy(&_buffer->buffer_body.circularBuffer.data[_buffer->buffer_body.circularBuffer.tail], data, size);
            _buffer->buffer_body.circularBuffer.tail += size;
        } else
            return false; // space not enough
    } else {
        clear_area = _buffer->size - _buffer->buffer_body.circularBuffer.tail + _buffer->buffer_body.circularBuffer.head;
        if (clear_area > size) {
            if (_buffer->size - _buffer->buffer_body.circularBuffer.tail < size) {
                uint8_t first_store, second_store;
                first_store = _buffer->size - _buffer->buffer_body.circularBuffer.tail;
                memcpy(&_buffer->buffer_body.circularBuffer.data[_buffer->buffer_body.circularBuffer.tail], data, first_store);
                second_store = size - first_store;
                memcpy(&_buffer->buffer_body.circularBuffer.data[0], data + first_store, second_store);
                _buffer->buffer_body.circularBuffer.tail = 0 + second_store;
            } else {
                memcpy(&_buffer->buffer_body.circularBuffer.data[_buffer->buffer_body.circularBuffer.tail], data, size);
                _buffer->buffer_body.circularBuffer.tail += size;
            }
        } else
            return false; // space not enough
    }
    return true;
}
static bool CircularBuffer_Read(Buffer_t *_buffer, uint8_t *data, uint16_t size)
{
    uint16_t stored_area = 0;
    if (_buffer->buffer_body.circularBuffer.head == _buffer->buffer_body.circularBuffer.tail)
        return false; // Buffer is empty
    else if (_buffer->buffer_body.circularBuffer.head > _buffer->buffer_body.circularBuffer.tail) {
        stored_area = _buffer->size - _buffer->buffer_body.circularBuffer.head + _buffer->buffer_body.circularBuffer.tail;
        if (stored_area >= size) {
            // if need to wrap around
            if (_buffer->size - _buffer->buffer_body.circularBuffer.head < size) {
                uint8_t first_copy, second_copy;
                first_copy = _buffer->size - _buffer->buffer_body.circularBuffer.head;
                memcpy(data, _buffer->buffer_body.circularBuffer.data + _buffer->buffer_body.circularBuffer.head, first_copy);
                second_copy = size - first_copy;
                memcpy(data + first_copy, _buffer->buffer_body.circularBuffer.data, second_copy);
                _buffer->buffer_body.circularBuffer.head = 0 + second_copy;
            } else {
                memcpy(data, _buffer->buffer_body.circularBuffer.data + _buffer->buffer_body.circularBuffer.head, size);
                _buffer->buffer_body.circularBuffer.head += size;
            }
        }
    } else {
        stored_area = _buffer->buffer_body.circularBuffer.tail - _buffer->buffer_body.circularBuffer.head;
        if (stored_area >= size) {
            memcpy(data, _buffer->buffer_body.circularBuffer.data + _buffer->buffer_body.circularBuffer.head, size);
            _buffer->buffer_body.circularBuffer.head += size;
        } else
            return false; // Not enough data
    }
    return true;
}

Buffer_t *Buffer_Init(BufferConfig_t *buffer_config)
{
    Buffer_t *buffer = (Buffer_t *)malloc(sizeof(Buffer_t));
    if (buffer == NULL || buffer_config == NULL) {
        free(buffer);
        return NULL; // Memory allocation failed
    }
    buffer->type = buffer_config->type;
    switch (buffer->type) {
        case CommonBuffer:
            // Initialize common buffer
            buffer->buffer_body.commonBuffer      = (CommonBuffer_t){0};
            buffer->buffer_body.commonBuffer.data = (uint8_t *)malloc(buffer_config->size * sizeof(uint8_t));
            if (buffer->buffer_body.commonBuffer.data == NULL) {
                free(buffer);
                return NULL; // Memory allocation failed
            }
            memset(buffer->buffer_body.commonBuffer.data, 0, buffer_config->size * sizeof(uint8_t));
            break;
        case DoubleBuffer:
            // Initialize double buffer
            buffer->buffer_body.doubleBuffer         = (DoubleBuffer_t){0};
            buffer->buffer_body.doubleBuffer.data[0] = (uint8_t *)malloc(buffer_config->size * sizeof(uint8_t));
            buffer->buffer_body.doubleBuffer.data[1] = (uint8_t *)malloc(buffer_config->size * sizeof(uint8_t));
            if (buffer->buffer_body.doubleBuffer.data[0] == NULL || buffer->buffer_body.doubleBuffer.data[1] == NULL) {
                free(buffer->buffer_body.doubleBuffer.data[0]);
                free(buffer->buffer_body.doubleBuffer.data[1]);
                free(buffer);
                return NULL; // Memory allocation failed
            }
            memset(buffer->buffer_body.doubleBuffer.data[0], 0, buffer_config->size * sizeof(uint8_t));
            memset(buffer->buffer_body.doubleBuffer.data[1], 0, buffer_config->size * sizeof(uint8_t));
            break;
        case CircularBuffer:
            // Initialize circular buffer
            buffer->buffer_body.circularBuffer      = (CircularBuffer_t){0};
            buffer->buffer_body.circularBuffer.data = (uint8_t *)malloc(buffer_config->size * sizeof(uint8_t));
            if (buffer->buffer_body.circularBuffer.data == NULL) {
                free(buffer);
                return NULL;
            }
            memset(buffer->buffer_body.circularBuffer.data, 0, buffer_config->size * sizeof(uint8_t));
            break;
    }
    buffer->size = buffer_config->size;
    return buffer;
}

bool Buffer_Write(Buffer_t *_buffer, uint8_t *data, uint16_t size)
{
    if (_buffer == NULL || data == NULL || size == 0 || size > _buffer->size) {
        return false;
    }
    switch (_buffer->type) {
        case CommonBuffer:
            memset(_buffer->buffer_body.commonBuffer.data, 0, _buffer->size * sizeof(uint8_t));
            memcpy(_buffer->buffer_body.commonBuffer.data, data, size);
            break;
        case DoubleBuffer:
            _buffer->buffer_body.doubleBuffer.idx = !_buffer->buffer_body.doubleBuffer.idx;
            memset(_buffer->buffer_body.doubleBuffer.data[_buffer->buffer_body.doubleBuffer.idx], 0, _buffer->size * sizeof(uint8_t));
            memcpy(_buffer->buffer_body.doubleBuffer.data[_buffer->buffer_body.doubleBuffer.idx], data, size);
            break;
        case CircularBuffer:
            if (CircularBuffer_Write(_buffer, (uint8_t *)data, size) == false)
                return false;
            break;
    }
    return true;
}

bool Buffer_Read(Buffer_t *_buffer, uint8_t *data, uint16_t size)
{
    if (_buffer == NULL || data == NULL || size == 0 || size > _buffer->size) {
        return false;
    }
    switch (_buffer->type) {
        case CommonBuffer:
            memcpy(data, _buffer->buffer_body.commonBuffer.data, size);
            break;
        case DoubleBuffer:
            memcpy(data, _buffer->buffer_body.doubleBuffer.data[_buffer->buffer_body.doubleBuffer.idx], size);
            break;
        case CircularBuffer:
            if (CircularBuffer_Read(_buffer, data, size) == false)
                return false;
            break;
    }

    return true;
}

void Buffer_Free(Buffer_t *buffer)
{
    if (buffer == NULL) {
        return;
    }
    switch (buffer->type) {
        case CommonBuffer:
            free(buffer->buffer_body.commonBuffer.data);
            break;
        case DoubleBuffer:
            free(buffer->buffer_body.doubleBuffer.data[0]);
            free(buffer->buffer_body.doubleBuffer.data[1]);
            break;
        case CircularBuffer:
            free(buffer->buffer_body.circularBuffer.data);
            break;
    }
    free(buffer);
}
