#include <stdio.h>
#include <string.h>
#include <string>
#include <nds.h>
#include <nf_lib.h>
#include <stdlib.h>
#include <nds/ndstypes.h>
#include <nds/arm9/background.h>
#include <nds/arm9/keyboard.h>
#include <nds/arm9/input.h>

int frames = 0;
std::string enteredText = "";
bool cursorFlicker = true;

void doText(u8 screen, u8 layer, u16 x, u16 y, const char *text, bool reset) {
    if(reset) NF_ClearTextLayer(0, 0);
    NF_WriteText(screen, layer, x, y, text);
    NF_UpdateTextLayers();
}
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
        if(frames % 25 == 0) cursorFlicker = !cursorFlicker;

        int key = keyboardUpdate();
        int pressed = keysDown();

        if(pressed & KEY_START) break;
        else if(pressed & KEY_A || key == 10) sendText();
        else if(key == 8 && enteredText.length() > 0) enteredText.pop_back();
        else if(key > 0) enteredText += key;

        swiWaitForVBlank();
        scanKeys();

        std::string disp = enteredText;
        if(cursorFlicker) disp += "_";
        doText(0, 0, 1, 22, disp.c_str(), true);
    }

    return 0;
}
