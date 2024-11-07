#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>

// TFT 实例
TFT_eSPI tft = TFT_eSPI();

#define DRAW_BUF_SIZE (TFT_WIDTH * TFT_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

static uint32_t my_tick(void)
{
  return millis();
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)px_map, w * h, true);
  tft.endWrite();

  lv_display_flush_ready(disp);
}

void lvgl_setup()
{
  tft.init();
  tft.setRotation(1);

  lv_init();

  lv_tick_set_cb(my_tick);

  lv_display_t *disp;
  disp = lv_display_create(TFT_HEIGHT, TFT_WIDTH);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
}

// 在setup()之前添加函数声明

static void anim_x_cb(void *var, int32_t v)
{
  lv_obj_set_x((lv_obj_t *)var, v); // 添加类型转换
}

static void anim_size_cb(void *var, int32_t v)
{
  lv_obj_set_size((lv_obj_t *)var, v, v); // 添加类型转换
}

/**
 * Create a playback animation
 */
void lv_example_anim_2(void)
{

  lv_obj_t *obj = lv_obj_create(lv_screen_active());
  lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_RED), 0);
  lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);

  lv_obj_align(obj, LV_ALIGN_LEFT_MID, 10, 0);

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, obj);
  lv_anim_set_values(&a, 10, 50);
  lv_anim_set_duration(&a, 1000);
  lv_anim_set_playback_delay(&a, 100);
  lv_anim_set_playback_duration(&a, 300);
  lv_anim_set_repeat_delay(&a, 500);
  lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

  lv_anim_set_exec_cb(&a, anim_size_cb);
  lv_anim_start(&a);
  lv_anim_set_exec_cb(&a, anim_x_cb);
  lv_anim_set_values(&a, 10, 240);
  lv_anim_start(&a);
}

void setup()
{
  // 初始化串口
  Serial.begin(115200);

  lvgl_setup();

  lv_example_anim_2();

  lv_obj_t *label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "Hello Cardputer, I'm LVGL!");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

}

void loop()
{
  lv_timer_handler(); // LVGL任务处理
  delay(5);
}