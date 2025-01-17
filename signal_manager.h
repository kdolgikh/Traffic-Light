#include <stdbool.h>

typedef enum {
  NONE,
  RED,
  YELLOW,
  GREEN,
} COLOR_ENUM;

typedef struct {
  COLOR_ENUM curr_color;
  bool (*set_color)(COLOR_ENUM color);
} SIGNAL_STRUCT;

bool manage_signal(SIGNAL_STRUCT *const sig_ptr, COLOR_ENUM color);
bool sig_top_set_color(COLOR_ENUM color);
bool sig_mid_set_color(COLOR_ENUM color);
bool sig_bot_set_color(COLOR_ENUM color);
