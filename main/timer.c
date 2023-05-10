#include "driver/gptimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "tasks.h"
#include "timer.h"

static bool cb_gpt_alarm(gptimer_handle_t hdl, const gptimer_alarm_event_data_t *data_ev, void *data)
{
    BaseType_t hiprio_woken = pdFALSE;
    int value = 10;
    xQueueSendFromISR(hdl_q_timer, &value, &hiprio_woken);
    return hiprio_woken == pdTRUE;
}

void init_timer(void)
{
    gptimer_config_t cfg_gpt = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(gptimer_new_timer(&cfg_gpt, &hdl_gpt));
    gptimer_alarm_config_t cfg_gpt_alarm = {
        .alarm_count = 10 * 1000 * 1000,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(gptimer_set_alarm_action(hdl_gpt, &cfg_gpt_alarm));
    gptimer_event_callbacks_t cfg_cb = {
        .on_alarm = cb_gpt_alarm,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(gptimer_register_event_callbacks(hdl_gpt, &cfg_cb, NULL));
    ESP_ERROR_CHECK_WITHOUT_ABORT(gptimer_enable(hdl_gpt));
    hdl_q_timer = xQueueCreate(2, sizeof(int));
    xTaskCreate(&task_timer, "task_timer", 4 * 1024, NULL, 5, NULL);
}