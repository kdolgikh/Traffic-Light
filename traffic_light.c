#include "traffic_light.h"
#include "signal_manager.h"
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

typedef enum {
  TL_STATE_INIT,      // read config
  TL_STATE_OPERATION, // normal mode
  TL_STATE_ERROR,     // error mode if loop sensor(s) down
} TL_STATES_ENUM;

typedef enum {
  TL_MAIN_SIGNAL_STATE_RED,
  TL_MAIN_SIGNAL_STATE_YELLOW,
  TL_MAIN_SIGNAL_STATE_GREEN,
} TL_MAIN_SIGNAL_STATES_ENUM;

static SIGNAL_STRUCT sig_red = {
    .curr_color = NONE,
    .set_color = sig_top_set_color,
};
static SIGNAL_STRUCT sig_yellow = {
    .curr_color = NONE,
    .set_color = sig_mid_set_color,
};
static SIGNAL_STRUCT sig_green = {
    .curr_color = NONE,
    .set_color = sig_bot_set_color,
};

static void tl_normal_op(void);

static void tl_normal_op(void) {
  static TL_MAIN_SIGNAL_STATES_ENUM tl_main_sig_state =
      TL_MAIN_SIGNAL_STATE_RED;

  switch (tl_main_sig_state) {
  case (TL_MAIN_SIGNAL_STATE_RED):
    break;
  case (TL_MAIN_SIGNAL_STATE_YELLOW):
    break;
  case (TL_MAIN_SIGNAL_STATE_GREEN):
    break;
  }
}

/* Interface Functions */

void tl_state_machine(void) {
  static TL_STATES_ENUM tl_state = TL_STATE_INIT;
  static TL_STATES_ENUM tl_state_was;

  tl_state_was = tl_state;

  switch (tl_state) {
  case (TL_STATE_INIT):
    break;
  case (TL_STATE_OPERATION):
    tl_normal_op();
    break;
  case (TL_STATE_ERROR):
  default:
    break;
  }

  if (tl_state_was != tl_state) {
    // print new state
  }
}
