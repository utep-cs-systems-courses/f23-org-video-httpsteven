#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>

#define LED BIT6
#define SW1 1
#define SW2 2
#define SW3 4
#define SW4 8
#define SWITCHES 15
#define SW0 BIT3

// ...

void sw_init(char port, int switches, int* paddle_dir) {
  P1REN |= switches;
  P1IE |= switches;
  P1OUT |= switches;
  P1DIR &= ~switches;
  P1IES |= (P1IN & switches);
  P1IES &= (P1IN | ~switches);
  if (port == '1')
    sw0_update_interrupt_sense();
  else if (port == '2')
    switch_update_interrupt_sense();
}

void init_paddle(int* paddle_pos, int x, int y) {
  paddle_pos[0] = x;
  paddle_pos[1] = y;
}

void draw_game_objects() {
  drawRect(t_paddle_pos, paddle_dims, COLOR_WHITE);
  drawRect(b_paddle_pos, paddle_dims, COLOR_WHITE);
  drawRect(ball_pos, ball_dims, COLOR_WHITE);
}

void update_game_objects(int* pos, int* dims, int* dir, int bg_color, int obj_color) {
  drawRect(pos, dims, bg_color);
  pos[0] += dir[0];
  pos[1] += dir[1];
  drawRect(pos, dims, obj_color);
}

int ranges_overlap(int r1[], int r2[]) {
  return (r1[0] <= r2[1] && r1[0] >= r2[0]) || (r2[0] <= r1[1] && r2[0] >= r1[0]);
}

int boxes_collide(int b1pos[], int b1dims[], int b2pos[], int b2dims[]) {
  int b1_bounds_horiz[] = {b1pos[0], b1pos[0] + b1dims[0]};
  int b2_bounds_horiz[] = {b2pos[0], b2pos[0] + b2dims[0]};
  int horiz_overlap = ranges_overlap(b1_bounds_horiz, b2_bounds_horiz);

  int b1_bounds_vert[] = {b1pos[1], b1pos[1] + b1dims[1]};
  int b2_bounds_vert[] = {b2pos[1], b2pos[1] + b2dims[1]};
  int vert_overlap = ranges_overlap(b1_bounds_vert, b2_bounds_vert);

  return horiz_overlap && vert_overlap;
}

// ...

int main() {
  configureClocks();
  lcd_init();
  switch_init();
  sw_init('1', SW0, t_paddle_dir);
  sw_init('2', SWITCHES, b_paddle_dir);
  enableWDTInterrupts();
  or_sr(0x8);

  int bg_clr = COLOR_BLACK;
  int obj_clr = COLOR_WHITE;
  clearScreen(bg_clr);
  init_paddle(t_paddle_pos, 98, 10);
  init_paddle(b_paddle_pos, 0, 145);
  init_paddle(ball_pos, 20, 20);
  draw_game_objects();

  while (1) {
    update_game_objects(t_paddle_pos, paddle_dims, t_paddle_dir, bg_clr, obj_clr);
    update_game_objects(b_paddle_pos, paddle_dims, b_paddle_dir, bg_clr, obj_clr);
    update_game_objects(ball_pos, ball_dims, ball_dir, bg_clr, obj_clr);

    if ((ball_pos[0] <= 0) || (ball_pos[0] + ball_dims[0] >= display_dims[0])) {
      ball_dir[0] = -ball_dir[0];
      // TODO: beep
    }

    if (boxes_collide(b_paddle_pos, paddle_dims, ball_pos, ball_dims)) {
      ball_dir[1] = -1;
    }

    if (boxes_collide(t_paddle_pos, paddle_dims, ball_pos, ball_dims)) {
      ball_dir[1] = 1;
    }

    if (ball_pos[1] <= 0 || ball_pos[1] + ball_dims[1] >= display_dims[1]) {
      clearScreen(COLOR_RED);
      break;
    }

    if (t_paddle_pos[0] <= 0 || t_paddle_pos[0] + paddle_dims[0] >= display_dims[0])
      t_paddle_dir[0] = 0;

    if (b_paddle_pos[0] <= 0 || b_paddle_pos[0] + paddle_dims[0] >= display_dims[0])
      b_paddle_dir[0] = 0;
  }
}

void sw_interrupt_handler(char port, int* paddle_dir) {
  char p_val = (port == '1') ? sw0_update_interrupt_sense() : switch_update_interrupt_sense();
  switches = ~p_val & ((port == '1') ? SW0 : SWITCHES);

  if (switches & SW0)
    paddle_dir[0] = -2;
  // ...
}

void __interrupt_vec(PORT1_VECTOR) port_1() {
  if (P1IFG & SW0) {
    P1IFG &= ~SW0;
    sw_interrupt_handler('1', t_paddle_dir);
  }
}

void __interrupt_vec(PORT2_VECTOR) Port_2() {
  if (P2IFG & SWITCHES) {
    P2IFG &= ~SWITCHES;
    sw_interrupt_handler('2', b_paddle_dir);
  }
}

// ...
