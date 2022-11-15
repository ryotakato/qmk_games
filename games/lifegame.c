#include "lifegame.h"

#include <string.h>

#include "timer.h"


void lifegame_init(Lifegame* lifegame) {
  uint16_t t = timer_read();
  xorshift_init(&lifegame->xorshift, t, (t >> 8), 3, 4);

  lifegame->gamestate = 0;
  lifegame->lasttime = 0;

  lifegame->debugval = sizeof(int);
  lifegame->speed = 1000;
  lifegame->initialized = true;

  // 初期化
  memset(lifegame->blocks, 0, sizeof(lifegame->blocks));

  // 初期はランダム配置
  for (int y = 0; y < LifegameHeight; y++) {
    for (int x = 0; x < LifegameWidth; x++) {
      lifegame->blocks[y][x] = xorshift_next(&lifegame->xorshift) % 2;
    }
  }
  /* 
  // 初期グライダー
  lifegame->blocks[0][1] = 1;
  lifegame->blocks[1][2] = 1;
  lifegame->blocks[2][0] = 1;
  lifegame->blocks[2][1] = 1;
  lifegame->blocks[2][2] = 1;
  */
}

void lifegame_reset(Lifegame* lifegame) {
  lifegame->gamestate = 1;
}

int _lifegame_was_alive(Lifegame* lifegame, int x, int y) {
  // 1の位が1なら前回は生きていたということ
  return lifegame->blocks[y][x] % 10;
}

void lifegame_update(Lifegame* lifegame) {
  if (!lifegame->initialized) {
    lifegame_init(lifegame);
  }

  if (lifegame->gamestate == 0) {
    uint16_t time = timer_read();
    if (time - lifegame->lasttime < lifegame->speed) {
      return;
    }
    lifegame->lasttime = time;



    uint8_t tmp;
    int maxH = LifegameHeight - 1;
    int maxW = LifegameWidth - 1;

    // 初期化
    // 値を10の位に移動させ、前回の値とする
    /*
    for (int y = 0; y < LifegameHeight; y++) {
      for (int x = 0; x < LifegameWidth; x++) {
        lifegame->blocks[y][x] = lifegame->blocks[y][x] * 10;
      }
    }
    */


    // 次の世代を算出
    for (int y = 0; y < LifegameHeight; y++) {
      for (int x = 0; x < LifegameWidth; x++) {

        tmp = _lifegame_was_alive(lifegame, x             , y-1<0?maxH:y-1) + // 下
              _lifegame_was_alive(lifegame, x             , y+1>maxH?0:y+1) + // 上
              _lifegame_was_alive(lifegame, x-1<0?maxW:x-1, y             ) + // 左
              _lifegame_was_alive(lifegame, x+1>maxW?0:x+1, y             ) + // 右
              _lifegame_was_alive(lifegame, x-1<0?maxW:x-1, y-1<0?maxH:y-1) + // 左下
              _lifegame_was_alive(lifegame, x+1>maxW?0:x+1, y-1<0?maxH:y-1) + // 右下
              _lifegame_was_alive(lifegame, x-1<0?maxW:x-1, y+1>maxH?0:y+1) + // 左上
              _lifegame_was_alive(lifegame, x+1>maxW?0:x+1, y+1>maxH?0:y+1);  // 右上

        // 判定
        // 生きてる場合、10を足すこと(10の位に次の結果をいれる)で今回の値とする
        if (_lifegame_was_alive(lifegame, x, y) == 1) {
          lifegame->blocks[y][x] = lifegame->blocks[y][x] + ((tmp==2 || tmp==3) * 10);
        } else {
          lifegame->blocks[y][x] = lifegame->blocks[y][x] + ((tmp==3) * 10);
        }


      }
    }

    // 終わったら値を1の位に戻す
    for (int y = 0; y < LifegameHeight; y++) {
      for (int x = 0; x < LifegameWidth; x++) {
        lifegame->blocks[y][x] = lifegame->blocks[y][x] / 10;
      }
    }

  } else {
    uint16_t time = timer_read();
    if (time - lifegame->lasttime < 2000) {
      return;
    }
    lifegame->lasttime = time;

    // init
    lifegame_init(lifegame);
  }
}

void lifegame_render(Lifegame* lifegame, ScreenMatrix* matrix) {
  for (int y = 0; y < LifegameHeight; y++) {
    for (int x = 0; x < LifegameWidth; x++) {
      if (lifegame->blocks[y][x] != 0) {
        screen_draw_rect(matrix, x * 3, y * 3, 2, 2, 0); // 3マス使う場合、40*10までが限界
        //screen_draw_rect(matrix, x * 2, y * 2, 2, 2, 0); // 2マス使う場合、64*16までいけるが、CPU使いすぎて入力に支障がでる
      }
    }
  }

}
