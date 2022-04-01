#define ARM9
#include <string>
#include <vector>
#include "../nflib/include/nf_lib.h"
using namespace std;

struct {
  int alt = -26;
  int toggle = -23;
  int left = -20;
  int down = -19;
  int right = -18;
  int up = -17;
  int ctrl = -16;
  int capsLock = -15;
  int shift = -14;
  int menu = -5;
  int none = -1;
  int backspace = 8;
  int tab = 9;
  int enter = 10;
} keys;

// https://stackoverflow.com/a/37454181
vector<string> split(const string& str, const string& delim) {
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

int frames = 0;
string enteredText = "";
string console = "Screen buffer.\nThis is a new line?";
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
    else if (pressed & KEY_A || key == keys.enter) sendText(); // A key or RETURN
    else if (key == keys.backspace) {
      if (!enteredText.empty()) enteredText.pop_back(); // backspace
    }
    else if (key > 0) enteredText += char(key);

    swiWaitForVBlank(); // no idea honestly
    scanKeys(); // guessing this scans for new buttons pressed?

    string disp = enteredText;
    if(cursorFlicker) disp += "_"; // add cursor
    // screen 0 - layer 0 - X 1 - Y 22 - text - clear the screen
    doText(0, 0, 1, 22, disp.c_str(), true);

    int y = 0;
    for (auto line : split(console, "\n")) {
      doText(0, 0, 0, y, line.c_str(), false);
      y++;
    }
  }

  return 0;
}
