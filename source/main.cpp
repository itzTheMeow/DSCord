#define ARM9

#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>

#include "../ndslib/include/nds.h"
#include "../nflib/include/nf_lib.h"
#include "../ndslib/include/nds/ndstypes.h"
#include "../ndslib/include/nds/arm9/background.h"
#include "../ndslib/include/nds/arm9/keyboard.h"
#include "../ndslib/include/nds/arm9/input.h"

int frames = 0;
std::string enteredText = "";
std::string console = "Screen buffer.\nThis is a new line?";
bool cursorFlicker = true;

// simple enough to not need docs
void doText(u8 screen, u8 layer, u16 x, u16 y, const char *text, bool reset) {
  if(reset) NF_ClearTextLayer(0, 0);
  NF_WriteText(screen, layer, x, y, text);
  NF_UpdateTextLayers();
}
// TODO: add actual sending
void sendText() {
  enteredText = "";
}

int main(int argc, char **argv) {
  Keyboard kb;

  videoSetMode(MODE_0_2D);
  videoSetModeSub(MODE_0_2D);

  vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_MAIN_SPRITE, VRAM_C_SUB_BG, VRAM_D_SUB_SPRITE);

  keyboardInit(&kb, 0, BgType_Text4bpp, BgSize_T_256x512, 14, 0, false, true);
  keyboardShow();

  NF_Set2D(0, 0);
  NF_SetRootFolder("NITROFS");

  NF_InitTiledBgBuffers();
  NF_InitTiledBgSys(0);

  NF_InitSpriteBuffers();
  NF_InitSpriteSys(0);

  NF_InitTextSys(0);

  NF_LoadTextFont("fonts/font", "default", 256, 256, 0);
  NF_CreateTextLayer(0, 0, 0, "default");

  while(1) {
    frames++;
    // flicker cursor every 25 frames
    if(frames % 25 == 0) cursorFlicker = !cursorFlicker;

    int key = keyboardUpdate(); // gets keyboard keys
    int pressed = keysDown(); // gets pressed buttons

    if (pressed & KEY_START) break; // detects START button pressed
    else if (pressed & KEY_A || key == 10) sendText(); // A key or RETURN
    else if (key == 8) {
      if (!enteredText.empty()) enteredText.pop_back(); // backspace
    }
    else if (key > 0) enteredText += key;

    swiWaitForVBlank(); // no idea honestly
    scanKeys(); // guessing this scans for new buttons pressed?

    std::string disp = enteredText;
    if(cursorFlicker) disp += "_"; // add cursor
    // screen 0 - layer 0 - X 1 - Y 22 - text - clear the screen
    doText(0, 0, 1, 22, disp.c_str(), true);

    doText(0, 0, 0, 0, console.c_str(), false);
  }

  return 0;
}
