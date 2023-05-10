#include "driver/ledc.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "lvgl.h"
#include "sdkconfig.h"

#include "shared.h"
#include "timer.h"

#ifdef CONFIG_SALLOW_DEBUG_RUNTIME_STATS
void task_debug_runtime_stats(void *data)
{
    char buf[2048];
    while(true) {
        vTaskGetRunTimeStats(buf);
        printf("%s\n", buf);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
#endif

void task_timer(void *data)
{
    int value;
    if (hdl_q_timer == NULL) {
        ESP_LOGE(TAG, "task_timer: hdl_q_timer is NULL");
        goto delete;
    }
    while(true) {
        xQueueReceive(hdl_q_timer, &value, portMAX_DELAY);
        ESP_LOGI(TAG, "received event in timer queue");
        // sleep value seconds
        vTaskDelay(value * 1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Wake LCD timeout, turning off LCD");
        ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0, 0);
        gptimer_stop(hdl_gpt);
        gptimer_set_raw_count(hdl_gpt, 0);
    }
delete:
    vTaskDelete(NULL);
}