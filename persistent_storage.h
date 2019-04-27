#ifndef PERSISTENT_STORAGE_H
#define PERSISTENT_STORAGE_H
#include <native_gecko.h>


#define PS_KEY_FALL_CONFIGURED 0x4000
#define PS_KEY_TAP_CONFIGURED  0x4001
#define PS_KEY_BUTTON_STATE    0x4002


uint16_t ps_save(uint16_t key, void *pValue, uint8_t size);

uint16_t ps_load(uint16_t key, void *pValue, uint8_t size);

#endif
