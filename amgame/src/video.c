#include <game.h>

#define SIDE 16
static int w, h;

static void init() {
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;
}

static void draw_tile(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

void splash() {
  init();
  for (int x = 0; x * SIDE <= w; x++) {
    for (int y = 0; y * SIDE <= h; y++) {
      if ((x & 1) ^ (y & 1)) {  // x,y都为奇数或都为偶数时取0，否则取1
        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0x007ACC); // white->0xffffff 我改成了0x007ACC->蓝色
      }
    }
  }
  draw_tile(0, 0, SIDE, SIDE, 0xdd4c35);
}

#define KEYNAME(key) [AM_KEY_##key] = #key,

static const char *key_names[] = {
  AM_KEYS(KEYNAME)
};

static int last_x = 0, last_y = 0;
static int cur_x = 0, cur_y = 0;

void move() {
  /** 用 K J H L 四个键控制上下左右来移动小方块 */
  AM_INPUT_KEYBRD_T event = { .keycode = AM_KEY_NONE };
  ioe_read(AM_INPUT_KEYBRD, &event);
  if (event.keycode != AM_KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
  last_x = cur_x;
  last_y = cur_y;
  if (event.keycode == AM_KEY_K && event.keydown) {
    cur_y = cur_y==0 ? 0 : cur_y-1;
  } else if (event.keycode == AM_KEY_J && event.keydown) {
    cur_y = cur_y*SIDE<h-SIDE ? cur_y+1 : cur_y;
  } else if (event.keycode == AM_KEY_H && event.keydown) {
    cur_x = cur_x==0 ? 0 : cur_x-1;
  } else if (event.keycode == AM_KEY_L && event.keydown) {
    cur_x = cur_x*SIDE<w-SIDE ? cur_x+1 : cur_x;
  } else if (event.keycode == AM_KEY_ESCAPE && event.keydown) {
    puts("Game Over!\n");
    halt(0);
  }
  if ((cur_x^last_x)||(cur_y^last_y)) {
    draw_tile(cur_x * SIDE, cur_y * SIDE, SIDE, SIDE, 0xdd4c35);
    draw_tile(last_x * SIDE, last_y * SIDE, SIDE, SIDE, ((last_x & 1)^(last_y & 1)) ? 0x007ACC : 0x0000);
  }
}




