#include "display.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define I2C_SDA      8
#define I2C_SCL      9
#define I2C_PORT     I2C_NUM_0
#define I2C_FREQ_HZ  400000

static const char *TAG = "DISPLAY";
static u8g2_t u8g2;

// ─── HAL ─────────────────────────────────────
static uint8_t hal_i2c_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    static i2c_cmd_handle_t handle;
    switch (msg) {
        case U8X8_MSG_BYTE_SEND:
            i2c_master_write(handle, (uint8_t *)arg_ptr, arg_int, true); break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            handle = i2c_cmd_link_create();
            i2c_master_start(handle);
            i2c_master_write_byte(handle, u8x8_GetI2CAddress(u8x8), true); break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            i2c_master_stop(handle);
            i2c_master_cmd_begin(I2C_PORT, handle, pdMS_TO_TICKS(1000));
            i2c_cmd_link_delete(handle); break;
    }
    return 1;
}

static uint8_t hal_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    if (msg == U8X8_MSG_DELAY_MILLI)
        vTaskDelay(pdMS_TO_TICKS(arg_int) + 1);
    return 1;
}

// ─── INIT ─────────────────────────────────────
static void i2c_init(void) {
    i2c_config_t conf = {
        .mode             = I2C_MODE_MASTER,
        .sda_io_num       = I2C_SDA,
        .sda_pullup_en    = GPIO_PULLUP_ENABLE,
        .scl_io_num       = I2C_SCL,
        .scl_pullup_en    = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ,
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
}

void display_init(void) {
    i2c_init();

    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, hal_i2c_cb, hal_delay_cb);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    ESP_LOGI(TAG, "Pantalla lista");
}

u8g2_t* display_get(void) {
    return &u8g2;
}