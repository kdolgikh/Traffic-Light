#include "signal_manager.h"
#include <stddef.h>

bool sig_top_set_color(COLOR_ENUM color);
bool sig_mid_set_color(COLOR_ENUM color);
bool sig_bot_set_color(COLOR_ENUM color);

bool manage_signal(SIGNAL_STRUCT *const sig_ptr, COLOR_ENUM color) {
  bool result = false;

  do {
    if (sig_ptr == NULL) {
      break;
    }

    if (color < NONE || color > GREEN) {
      break;
    }

    if (sig_ptr->curr_color != color) {
      sig_ptr->curr_color = color;
      if (sig_ptr->set_color(color)) {
        result = true;
      }
    }
  } while (0);

  return result;
}