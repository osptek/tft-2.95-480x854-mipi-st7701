#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2c.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_mipi_dsi.h"
#include "driver/gpio.h"
#include "esp_ldo_regulator.h"
#include "esp_lvgl_port.h"
#include "lv_demos.h"

#include "esp_lcd_st7701.h"

/* LCD size */
#define EXAMPLE_LCD_H_RES   (480)
#define EXAMPLE_LCD_V_RES   (854)

#if LV_COLOR_DEPTH == 16
#define MIPI_DPI_PX_FORMAT (LCD_COLOR_PIXEL_FORMAT_RGB565)
#define BSP_LCD_COLOR_DEPTH (16)
#define LV_COLOR_FORMAT LV_COLOR_FORMAT_RGB565
#elif LV_COLOR_DEPTH == 24
#define MIPI_DPI_PX_FORMAT (LCD_COLOR_PIXEL_FORMAT_RGB888)
#define BSP_LCD_COLOR_DEPTH (24)
#define LV_COLOR_FORMAT LV_COLOR_FORMAT_RGB888
#endif

// “VDD_MIPI_DPHY”应供电 2.5V，可从内部 LDO 稳压器或外部 LDO 芯片获取电源
#define EXAMPLE_MIPI_DSI_PHY_PWR_LDO_CHAN 3 // LDO_VO3 连接至 VDD_MIPI_DPHY
#define EXAMPLE_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV 2500
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL 1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_BK_LIGHT -1
#define EXAMPLE_PIN_NUM_LCD_RST  -1

static const char *TAG = "EXAMPLE";

/* LCD IO and panel */
static esp_lcd_panel_handle_t lcd_panel = NULL;
static esp_lcd_panel_io_handle_t io_handle = NULL;

/* LVGL display and touch */
static lv_display_t *lvgl_disp = NULL;

static void example_bsp_enable_dsi_phy_power(void)
{
    // 打开 MIPI DSI PHY 的电源，使其从“无电”状态进入“关机”状态
    esp_ldo_channel_handle_t ldo_mipi_phy = NULL;
#ifdef EXAMPLE_MIPI_DSI_PHY_PWR_LDO_CHAN
    esp_ldo_channel_config_t ldo_mipi_phy_config = {
        .chan_id = EXAMPLE_MIPI_DSI_PHY_PWR_LDO_CHAN,
        .voltage_mv = EXAMPLE_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV,
    };
    ESP_ERROR_CHECK(esp_ldo_acquire_channel(&ldo_mipi_phy_config, &ldo_mipi_phy));
    ESP_LOGI(TAG, "MIPI DSI PHY Powered on");
#endif
}

static void example_bsp_init_lcd_backlight(void)
{
#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT};
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
#endif
}

static void example_bsp_set_lcd_backlight(uint32_t level)
{
#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, level);
#endif
}

static const st7701_lcd_init_cmd_t lcd_init_cmds[] = {
//  {cmd, { data }, data_size, delay_ms}
    {0xFF, (uint8_t []){0x77,0x01,0x00,0x00,0x13}, 5, 0},
    {0xEF, (uint8_t []){0x08}, 1, 0},
    {0xFF, (uint8_t []){0x77,0x01,0x00,0x00,0x10}, 5, 0},
    {0xC0, (uint8_t []){0xE9,0x03}, 2, 0},
    {0xC1, (uint8_t []){0x10,0x0C}, 2, 0},
    {0xC2, (uint8_t []){0x20,0x0A}, 2, 0},
    {0xCC, (uint8_t []){0x10}, 2, 0},
    {0xB0, (uint8_t []){0x0F,0x1F,0x28,0x1C,0x13,0x07,0x15,0x0A,0x08,0x2F,0x04,0x13,0x0F,0x2D,0x33,0x1F}, 16, 0},
    {0xB1, (uint8_t []){0x00,0x1F,0x25,0x0F,0x0F,0x05,0x0D,0x07,0x08,0x23,0x03,0x0E,0x0F,0x27,0x30,0x1F}, 16, 0},
    {0xFF, (uint8_t []){0x77,0x01,0x00,0x00,0x11}, 5, 0},
    {0xB0, (uint8_t []){0x4D}, 1, 0},
    {0xB1, (uint8_t []){0x66}, 1, 0},
    {0xB2, (uint8_t []){0x84}, 1, 0},
    {0xB3, (uint8_t []){0x80}, 1, 0},
    {0xB5, (uint8_t []){0x4A}, 1, 0},
    {0xB7, (uint8_t []){0x85}, 1, 0},
    {0xB8, (uint8_t []){0x33}, 1, 0},
    {0xB9, (uint8_t []){0x00,0x1F}, 2, 0},
    {0xC1, (uint8_t []){0x78}, 2, 0},
    {0xC2, (uint8_t []){0x78}, 2, 0},
    {0xD0, (uint8_t []){0x88}, 2, 0},
    {0xE0, (uint8_t []){0x00,0x00,0x02}, 3, 0},
    {0xE1, (uint8_t []){0x06,0xA0,0x08,0xA0,0x05,0xA0,0x07,0xA0,0x00,0x44,0x44}, 11, 0},
    {0xE2, (uint8_t []){0x30,0x30,0x44,0x44,0x6E,0xA0,0x00,0x00,0x6E,0xA0,0x00,0x00}, 12, 0},
    {0xE3, (uint8_t []){0x00,0x00,0x33,0x33}, 4, 0},
    {0xE4, (uint8_t []){0x44,0x44}, 2, 0},
    {0xE5, (uint8_t []){0x0D,0x69,0x0A,0xA0,0x0F,0x6B,0x0A,0xA0,0x09,0x65,0x0A,0xA0,0x0B,0x67,0x0A,0xA0}, 16, 0},
    {0xE6, (uint8_t []){0x00,0x00,0x33,0x33}, 4, 0},
    {0xE7, (uint8_t []){0x44,0x44}, 2, 0},
    {0xE8, (uint8_t []){0x0C,0x68,0x0A,0xA0,0x0E,0x6A,0x0A,0xA0,0x08,0x64,0x0A,0xA0,0x0A,0x66,0x0A,0xA0}, 16, 0},
    {0xE9, (uint8_t []){0x36,0x00}, 2, 0},
    {0xEB, (uint8_t []){0x00,0x01,0xE4,0xE4,0x44,0x88,0x40}, 7, 0},
    {0xED, (uint8_t []){0xFF,0x45,0x67,0xFA,0x01,0x2B,0xCF,0xFF,0xFF,0xFC,0xB2,0x10,0xAF,0x76,0x54,0xFF}, 16, 0},
    {0xEF, (uint8_t []){0x10,0x0D,0x04,0x08,0x3F,0x1F}, 6, 0},
    {0xFF, (uint8_t []){0x77,0x01,0x00,0x00,0x13}, 5, 0},
    {0xE8, (uint8_t []){0x00,0x0E}, 2, 0},
    {0xFF, (uint8_t []){0x77,0x01,0x00,0x00,0x00}, 5, 0},
    {0xFF, (uint8_t []){0x77,0x01,0x00,0x00,0x13}, 5, 0},
    {0xE8, (uint8_t []){0x00,0x0C}, 2, 10},
    {0xE8, (uint8_t []){0x00,0x00}, 2, 0},
    {0xFF, (uint8_t []){0x77,0x01,0x00,0x00,0x00}, 5, 0},
    {0x35, (uint8_t []){0x00}, 1, 0},
    {0x11, (uint8_t []){0x00}, 0, 1500},
    {0x29, (uint8_t []){0x00}, 0, 120},
};

static esp_err_t app_lcd_init(void)
{
    esp_err_t ret = ESP_OK;

    example_bsp_enable_dsi_phy_power();
    example_bsp_init_lcd_backlight();
    example_bsp_set_lcd_backlight(EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL);

    // 首先创建 MIPI DSI 总线，它还将初始化 DSI PHY
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_dsi_bus_config_t bus_config = {                    \
        .bus_id = 0,                                           \
        .num_data_lanes = 2,                                   \
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,           \
        .lane_bit_rate_mbps = 940,                             \
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus), err, TAG, "LCD init failed");

    ESP_LOGI(TAG, "Install MIPI DSI LCD control panel");
    // 我们使用DBI接口发送LCD命令和参数
    esp_lcd_dbi_io_config_t dbi_config = ST7701_PANEL_IO_DBI_CONFIG();

    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &io_handle), err, TAG, "LCD init failed");

    // 创建ST7701控制面板
    esp_lcd_dpi_panel_config_t dpi_config = {                 \
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,          \
        .dpi_clock_freq_mhz = 15,                             \
        .virtual_channel = 0,                                 \
        .pixel_format = MIPI_DPI_PX_FORMAT,                   \
        .num_fbs = 1,                                         \
        .video_timing = {                                     \
            .h_size = EXAMPLE_LCD_H_RES,                      \
            .v_size = EXAMPLE_LCD_V_RES,                      \
            .hsync_back_porch = 30,                           \
            .hsync_pulse_width = 2,                           \
            .hsync_front_porch = 50,                          \
            .vsync_back_porch = 10,                           \
            .vsync_pulse_width = 8,                           \
            .vsync_front_porch = 20,                          \
        },                                                    \
        .flags.use_dma2d = true,                              \
    };

    st7701_vendor_config_t vendor_config = {
        .init_cmds = lcd_init_cmds,      // Uncomment these line if use custom initialization commands
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(st7701_lcd_init_cmd_t),
        .flags.use_mipi_interface = 1,
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
        },
    };
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = BSP_LCD_COLOR_DEPTH,
        .vendor_config = &vendor_config,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_st7701(io_handle, &panel_config, &lcd_panel), err, TAG, "LCD init failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_reset(lcd_panel), err, TAG, "LCD init failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_init(lcd_panel), err, TAG, "LCD init failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_disp_on_off(lcd_panel, true), err, TAG, "LCD init failed");

    // 打开背光
    example_bsp_set_lcd_backlight(EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);

    return ret;

err:
    if (lcd_panel) {
        esp_lcd_panel_del(lcd_panel);
    }
    return ret;
}

static esp_err_t app_lvgl_init(void)
{
    /* Initialize LVGL */
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,         /* LVGL task priority */
        .task_stack = 4096*2,         /* LVGL 任务堆栈大小*/
        .task_affinity = -1,        /* LVGL task pinned to core (-1 is no affinity) */
        .task_max_sleep_ms = 500,   /* Maximum sleep in LVGL task */
        .timer_period_ms = 5        /* LVGL timer tick period in ms */
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port initialization failed");

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = lcd_panel,
        .buffer_size = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES,
        .double_buffer = true,
        .hres = EXAMPLE_LCD_H_RES,
        .vres = EXAMPLE_LCD_V_RES,
        .monochrome = false,
        .color_format = LV_COLOR_FORMAT,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = false,
            .buff_spiram = true,
            .sw_rotate = false,
            .swap_bytes = false,
            .full_refresh = false,
            .direct_mode = false,
        }
    };

    const lvgl_port_display_dsi_cfg_t dpi_cfg = {
        .flags = {
#if CONFIG_BSP_DISPLAY_LVGL_AVOID_TEAR
            .avoid_tearing = true,
#else
            .avoid_tearing = false,
#endif
        }
    };

    lvgl_disp = lvgl_port_add_disp_dsi(&disp_cfg, &dpi_cfg);

    return ESP_OK;
}

void app_main(void)
{
    /* LCD HW initialization */
    ESP_ERROR_CHECK(app_lcd_init());

    /* LVGL initialization */
    ESP_ERROR_CHECK(app_lvgl_init());

    /* Show LVGL objects */
    lvgl_port_lock(0);

    // lv_demo_music();
    lv_demo_widgets();

    lvgl_port_unlock();
}
