#include "driver/gptimer.h"

extern gptimer_handle_t hdl_gpt;
extern xQueueHandle hdl_q_timer;

void init_timer(void);