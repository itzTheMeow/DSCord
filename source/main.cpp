#include <stdio.h>
#include <string.h>
#include <nds.h>
#include <nf_lib.h>
#include <stdlib.h>
#include <nds/ndstypes.h>
#include <nds/arm9/background.h>
#include <nds/arm9/keyboard.h>
#include <nds/arm9/input.h>

void doText(u8 screen, u8 layer, u16 x, u16 y, const char *text, bool reset)
{
	if(reset) NF_ClearTextLayer(0, 0);
	NF_WriteText(screen, layer, x, y, text);
	NF_UpdateTextLayers();
}

// https://stackoverflow.com/a/7159973
char *join(const char* s1,  const char c)
{
    int len = strlen(s1);
    char* result = (char*) malloc(len + 2);

    if (result)
    {
            strcpy(result, s1);
            result[len] = c;
            result[len+1] = '\0';
    }

    return result;
}

int main(int argc, char **argv) 
{	
  Keyboard     kb;
  char* enteredText = "";

  videoSetMode(MODE_0_2D);
  videoSetModeSub(MODE_0_2D);

  vramSetPrimaryBanks(VRAM_A_MAIN_BG,
                      VRAM_B_MAIN_SPRITE,
                      VRAM_C_SUB_BG,
                      VRAM_D_SUB_SPRITE);

  keyboardInit(&kb,    0, BgType_Text4bpp, BgSize_T_256x512, 14, 0, false, true);
	keyboardShow();

	NF_Set2D(0,0);
	NF_SetRootFolder("NITROFS");

	NF_InitTiledBgBuffers();
	NF_InitTiledBgSys(0);
	
	NF_InitSpriteBuffers();
	NF_InitSpriteSys(0);
	
	NF_InitTextSys(0);
	
	NF_LoadTextFont("fonts/font", "default", 256, 256, 0);
	NF_CreateTextLayer(0, 0, 0, "default");

	while(1) {
		int key = keyboardUpdate();

		if(key > 0) enteredText = join(enteredText, char(key));

		swiWaitForVBlank();
		scanKeys();

		int pressed = keysDown();

		if(pressed & KEY_START) break;
		if(pressed & KEY_A) enteredText = 0;

		doText(0, 0, 2, 2, enteredText, true);
	}

	return 0;
}
