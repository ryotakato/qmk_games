# QMK Games

[QMK Firmware](https://github.com/qmk/qmk_firmware) の OLED 上で動くゲームです。

## Tetris

某ゲーム。以下のような感じで動作します（リンク先に動画があります）。

<a href="https://twitter.com/melponn/status/1115191783405703168">
  <img src="https://pbs.twimg.com/ext_tw_video_thumb/1115191702740803584/pu/img/S0jQr3V8khObnq8n.jpg" width="320">
</a>

128x32 の OLED しか想定していないので、それ以外の環境ではまず動きません。

確認した動作環境は、 Helix に付いてくる 128x32 の SSD1306 の OLED だけです。

### 遊び方

- h - 左へ移動
- l - 右へ移動
- j - 反時計回りに回転
- k - 時計回りに回転
- スペース - 落とす

### インストール方法

まず [QMK Firmware](https://github.com/qmk/qmk_firmware) を [ドキュメント](https://docs.qmk.fm/#/newbs_getting_started) の通りに実行してキーボードに転送できるようにしておいて下さい。

Helix でデフォルトのキー配置を利用する場合は `make helix:default` でビルドして `make helix:default:avrdude` でキーボードに転送できます。

次に、このリポジトリの `games/` ディレクトリを QMK Firmware リポジトリの `keyboards/helix/games/` ディレクトリにコピーします。

```bash
cp -r games/ /path/to/qmk_firmware/keyboards/helix/games/
```

次に、既存のソースコードにいくつか変更を加えます。

Helix の rev2/default のキー配置をそのまま使っている場合、以下のパッチを適用するだけで構いません。

パッチの適用は、以下のパッチを適当なファイル(今回の場合は `tetris.patch`)に保存し、QMK Firmware のリポジトリ上で `patch -p1 < tetris.patch` と実行します。

```diff
diff --git a/keyboards/helix/rev2/keymaps/default/keymap.c b/keyboards/helix/rev2/keymaps/default/keymap.c
index a64eed3e7..1f8f9ae9a 100644
--- a/keyboards/helix/rev2/keymaps/default/keymap.c
+++ b/keyboards/helix/rev2/keymaps/default/keymap.c
@@ -11,6 +11,11 @@
   #include "ssd1306.h"
 #endif
 
+#include "games/tetris.h"
+#include "games/screen.h"
+
+Tetris g_tetris;
+
 extern keymap_config_t keymap_config;
 
 #ifdef RGBLIGHT_ENABLE
@@ -333,6 +338,26 @@ void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
 }
 
 bool process_record_user(uint16_t keycode, keyrecord_t *record) {
+  if (record->event.pressed) {
+    switch (keycode) {
+    case KC_H:
+      tetris_move(&g_tetris, 0);
+      break;
+    case KC_L:
+      tetris_move(&g_tetris, 1);
+      break;
+    case KC_J:
+      tetris_rotate(&g_tetris, 0);
+      break;
+    case KC_K:
+      tetris_rotate(&g_tetris, 1);
+      break;
+    case KC_SPC:
+      tetris_move(&g_tetris, 2);
+      break;
+    }
+  }
+
   switch (keycode) {
     case QWERTY:
       if (record->event.pressed) {
@@ -525,16 +550,16 @@ void matrix_update(struct CharacterMatrix *dest,
 #define L_ADJUST (1<<_ADJUST)
 #define L_ADJUST_TRI (L_ADJUST|L_RAISE|L_LOWER)
 
-static void render_logo(struct CharacterMatrix *matrix) {
-
-  static char logo[]={
-    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
-    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
-    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,
-    0};
-  matrix_write(matrix, logo);
-  //matrix_write_P(&matrix, PSTR(" Split keyboard kit"));
-}
+//static void render_logo(struct CharacterMatrix *matrix) {
+//
+//  static char logo[]={
+//    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
+//    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
+//    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,
+//    0};
+//  matrix_write(matrix, logo);
+//  //matrix_write_P(&matrix, PSTR(" Split keyboard kit"));
+//}
 
 
 
@@ -584,6 +609,7 @@ void render_status(struct CharacterMatrix *matrix) {
 }
 
 
+/*
 void iota_gfx_task_user(void) {
   struct CharacterMatrix matrix;
 
@@ -601,5 +627,16 @@ void iota_gfx_task_user(void) {
   }
   matrix_update(&display, &matrix);
 }
+*/
+
+void iota_gfx_task_user(void) {
+  ScreenMatrix matrix;
+  screen_clear(&matrix);
+
+  tetris_update(&g_tetris);
+  tetris_render(&g_tetris, &matrix);
+
+  screen_update(&g_screen, &matrix);
+}
 
 #endif
diff --git a/keyboards/helix/rev2/keymaps/default/rules.mk b/keyboards/helix/rev2/keymaps/default/rules.mk
index 37ef8632d..016781bfe 100644
--- a/keyboards/helix/rev2/keymaps/default/rules.mk
+++ b/keyboards/helix/rev2/keymaps/default/rules.mk
@@ -30,7 +30,7 @@ endef
 # you can edit follows 7 Variables
 #  jp: 以下の7つの変数を必要に応じて編集します。
 HELIX_ROWS = 5              # Helix Rows is 4 or 5
-OLED_ENABLE = no            # OLED_ENABLE
+OLED_ENABLE = yes           # OLED_ENABLE
 LOCAL_GLCDFONT = no         # use each keymaps "helixfont.h" insted of "common/glcdfont.c"
 LED_BACK_ENABLE = no        # LED backlight (Enable WS2812 RGB underlight.)
 LED_UNDERGLOW_ENABLE = no   # LED underglow (Enable WS2812 RGB underlight.)
diff --git a/keyboards/helix/rules.mk b/keyboards/helix/rules.mk
index be234e60e..4462ef142 100644
--- a/keyboards/helix/rules.mk
+++ b/keyboards/helix/rules.mk
@@ -1,6 +1,9 @@
 SRC += i2c.c
 SRC += serial.c
-SRC += ssd1306.c
+SRC += games/ssd1306.c
+SRC += games/screen.c
+SRC += games/xorshift.c
+SRC += games/tetris.c
 
 # MCU name
 #MCU = at90usb1287
```

無事パッチの適用とビルドが出来たら、`make helix:default:avrdude` を実行してキーボードに転送します。

デフォルトのキー配置でない場合、あるいは上記のパッチの適用に失敗した場合は、以下の説明を読んで手動で変更して下さい。

`keyboards/helix/rules.mk`:

既存の `ssd1306.c` を除けて、`games/` 以下のファイルを追加します。

```diff
 SRC += i2c.c
 SRC += serial.c
-SRC += ssd1306.c
+SRC += games/ssd1306.c
+SRC += games/screen.c
+SRC += games/xorshift.c
+SRC += games/tetris.c
```

`keyboards/helix/rev2/keymaps/default/rules.mk`:

OLED を有効にします。

```diff
 # you can edit follows 7 Variables
 #  jp: 以下の7つの変数を必要に応じて編集します。
 HELIX_ROWS = 5              # Helix Rows is 4 or 5
-OLED_ENABLE = no            # OLED_ENABLE
+OLED_ENABLE = yes           # OLED_ENABLE
 LOCAL_GLCDFONT = no         # use each keymaps "helixfont.h" insted of "common/glcdfont.c"
 LED_BACK_ENABLE = no        # LED backlight (Enable WS2812 RGB underlight.)
 LED_UNDERGLOW_ENABLE = no   # LED underglow (Enable WS2812 RGB underlight.)
```

`keyboards/helix/rev2/keymaps/default/keymap.c`:

ファイルの先頭付近で、必要なファイルの `#include` と `Tetris` オブジェクトの作成を行います。

```diff
+#include "games/tetris.h"
+#include "games/screen.h"
+
+Tetris g_tetris;
```

ユーザのキー入力を処理する関数（今回の場合は `process_record_user`）で、キーが入力された時の処理を書きます。
ブロックの移動や回転するキーをカスタマイズしたい場合はここを変更します。

```diff
 bool process_record_user(uint16_t keycode, keyrecord_t *record) {
+  if (record->event.pressed) {
+    switch (keycode) {
+    case KC_H:
+      tetris_move(&g_tetris, 0);
+      break;
+    case KC_L:
+      tetris_move(&g_tetris, 1);
+      break;
+    case KC_J:
+      tetris_rotate(&g_tetris, 0);
+      break;
+    case KC_K:
+      tetris_rotate(&g_tetris, 1);
+      break;
+    case KC_SPC:
+      tetris_move(&g_tetris, 2);
+      break;
+    }
+  }
+
```

`render_logo` 関数を使っていないという警告がエラーとして報告されるので、削除します。
他のキーマップで出るかどうかは分かりません。

```diff
-static void render_logo(struct CharacterMatrix *matrix) {
-
-  static char logo[]={
-    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
-    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
-    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,
-    0};
-  matrix_write(matrix, logo);
-  //matrix_write_P(&matrix, PSTR(" Split keyboard kit"));
-}
+//static void render_logo(struct CharacterMatrix *matrix) {
+//
+//  static char logo[]={
+//    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
+//    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
+//    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,
+//    0};
+//  matrix_write(matrix, logo);
+//  //matrix_write_P(&matrix, PSTR(" Split keyboard kit"));
+//}
```

`iota_gfx_task_user(void)` 関数を既存の処理から完全に別にして、ゲームのロジック処理や描画処理を行います。

```diff
+/*
 void iota_gfx_task_user(void) {
   struct CharacterMatrix matrix;
 
@@ -601,5 +627,16 @@ void iota_gfx_task_user(void) {
   }
   matrix_update(&display, &matrix);
 }
+*/
+
+void iota_gfx_task_user(void) {
+  ScreenMatrix matrix;
+  screen_clear(&matrix);
+
+  tetris_update(&g_tetris);
+  tetris_render(&g_tetris, &matrix);
+
+  screen_update(&g_screen, &matrix);
+}
```

#### Helix 以外の環境にインストールする

`games/ssd1306.c` というファイルは QMK Firmware の `keyboards/helix/ssd1306.c` を改変したものです。

そのため Helix 以外の環境である場合は内容が合わないと思うので、以下の `qmk_firmware/keyboards/helix/ssd1306.c` と `qmk_games/games/ssd1306.c` の差分を見て手動で修正して下さい。

基本的に `matrix_render` 関数の代わりに、自作の `screen_render` 関数を利用するように変更しているだけです。

```diff
--- qmk_firmware/keyboards/helix/ssd1306.c	2019-04-13 00:13:44.000000000 +0900
+++ qmk_games/games/ssd1306.c	2019-04-13 00:09:15.000000000 +0900
@@ -2,6 +2,7 @@
 #ifdef SSD1306OLED
 
 #include "ssd1306.h"
+#include "screen.h"
 #include "i2c.h"
 #include <string.h>
 #include "print.h"
@@ -313,8 +314,52 @@
 #endif
 }
 
+// ビット反転
+uint8_t revbits8(uint8_t v) {
+  v = ((v >> 1) & 0x55) | ((v & 0x55) << 1);
+  v = ((v >> 2) & 0x33) | ((v & 0x33) << 2);
+  v = ((v >> 4) & 0x0F) | ((v & 0x0F) << 4);
+  return v;
+}
+
+void screen_render(ScreenMatrix* screen) {
+  last_flush = timer_read();
+  iota_gfx_on();
+#if DEBUG_TO_SCREEN
+  ++displaying;
+#endif
+
+  // Move to the home position
+  send_cmd3(PageAddr, 0, (DisplayHeight / 8) - 1);
+  send_cmd3(ColumnAddr, 0, DisplayWidth - 1);
+
+  if (i2c_start_write(SSD1306_ADDRESS)) {
+    goto done;
+  }
+  if (i2c_master_write(0x40)) {
+    // Data mode
+    goto done;
+  }
+
+  for (uint8_t row = 0; row < MatrixRows; ++row) {
+    for (uint8_t col = 0; col < DisplayWidth; ++col) {
+      uint8_t bits = screen->screen[DisplayWidth - col - 1][row];
+      i2c_master_write(revbits8(bits));
+    }
+  }
+
+  screen->dirty = false;
+
+done:
+  i2c_master_stop();
+#if DEBUG_TO_SCREEN
+  --displaying;
+#endif
+}
+
 void iota_gfx_flush(void) {
-  matrix_render(&display);
+  //matrix_render(&display);
+  screen_render(&g_screen);
 }
 
 __attribute__ ((weak))
@@ -324,7 +369,7 @@
 void iota_gfx_task(void) {
   iota_gfx_task_user();
 
-  if (display.dirty|| force_dirty) {
+  if (g_screen.dirty || force_dirty) {
     iota_gfx_flush();
     force_dirty = false;
   }
```


## Lifegame

コンウェイのライフゲームです。

[ライフゲーム - Wikipedia](https://ja.wikipedia.org/wiki/%E3%83%A9%E3%82%A4%E3%83%95%E3%82%B2%E3%83%BC%E3%83%A0)

Tetrisと同じく、
128x32 の OLED しか想定していません。
確認済み動作環境は Claw44 に付いてくる 128x32 の SSD1306 の OLED だけです。


### 遊び方

操作なしでスタートします。
遊ぶというよりかは、眺めるという感じのシミュレーションタイプです。

ただし、ライフゲームは全滅したり、同じパターンの繰り返しが続くことがありますので、
最初からやり直す機能だけはつけています。
（詳細はインストール方法をみて下さい）


### インストール方法

基本的にTetrisのインストール方法に従ってください。
下記にLifegame用の差分を示しますので、
該当部分をTetrisから読み替えて設定ください。

`keyboards/claw44/rules.mk`:

Tetrisではなく、Lifegameのソースを読み込みます。

```diff
 SRC += i2c.c
 SRC += serial.c
-SRC += ssd1306.c
+SRC += games/ssd1306.c
+SRC += games/screen.c
+SRC += games/xorshift.c
+SRC += games/lifegame.c
```

`keyboards/claw44/rev1/keymaps/default/keymap.c`:

ファイルの先頭付近で、必要なファイルの `#include` と `Lifegame` オブジェクトの作成を行います。

```diff
+#include "games/lifegame.h"
+#include "games/screen.h"
+
+Lifegame g_lifegame;
```

ライフゲームを最初からやり直す機能を割り当てるキーを定義します。
既存キーでも構わない場合はここは飛ばしてください。
また、すでにレイヤー切り替えなどでカスタムキーコードの定義がある場合は、
そのenumに追加するほうがよいでしょう。

```diff
+enum custom_keycodes {
+  KC_LGINI
+};
```


ユーザのキー入力を処理する関数（今回の場合は `process_record_user`）で、
KC_LGINIキーが入力された時の処理を書きます。

```diff
 bool process_record_user(uint16_t keycode, keyrecord_t *record) {
+  if (record->event.pressed) {
+    switch (keycode) {
+    case KC_LGINI:
+      lifegame_reset(&g_lifegame);
+      break;
+    }
+  }
+
```



また、KC_LGINIキーをマッピングします。
下記ではRAISEレイヤーにマッピングしていますが、どこにマッピングしても構いません。

```diff
  [_RAISE] = LAYOUT( \
  //,--------+--------+--------+--------+--------+--------.   ,--------+--------+--------+--------+--------+--------.
     _______, KC_BSLS, KC_CIRC, KC_EXLM, KC_AMPR, KC_PIPE,     KC_AT  , KC_EQL , KC_PLUS, KC_ASTR, KC_PERC, KC_MINS,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     KC_LPRN, KC_HASH, KC_DLR , KC_DQT , KC_QUOT, KC_TILD,     KC_LEFT, KC_DOWN,  KC_UP , KC_RGHT, KC_GRV , KC_RPRN,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     _______, _______, _______, _______, KC_LCBR, KC_LBRC,     KC_RBRC, KC_RCBR, _______, _______, _______, _______,
  //`--------+--------+--------+--------+--------+--------/   \--------+--------+--------+--------+--------+--------'
-                       _______, _______, _______, _______,     _______, _______, _______, RESET
+                       _______, _______, _______, _______,     _______, _______,KC_LGINI, RESET
  //                  `--------+--------+--------+--------'   `--------+--------+--------+--------'
  ),
```


`iota_gfx_task_user(void)` 関数を既存の処理から完全に別にして、ゲームのロジック処理や描画処理を行います。

```diff
+/*
 void iota_gfx_task_user(void) {
   struct CharacterMatrix matrix;
 
@@ -601,5 +627,16 @@ void iota_gfx_task_user(void) {
   }
   matrix_update(&display, &matrix);
 }
+*/
+
+void iota_gfx_task_user(void) {
+  ScreenMatrix matrix;
+  screen_clear(&matrix);
+
+  lifegame_update(&g_lifegame);
+  lifegame_render(&g_lifegame, &matrix);
+
+  screen_update(&g_screen, &matrix);
+}
```





## 問題の報告について

このリポジトリの issue に報告して下さい。
