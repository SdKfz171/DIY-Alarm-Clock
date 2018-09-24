#include "stm32f1xx_hal.h"
#include <stdbool.h>

void mp3_play (uint16_t num);
void mp3_stop (void);
void mp3_single_loop (bool state);
void mp3_single_play (uint16_t num);
void mp3_set_volume (uint16_t volume);
