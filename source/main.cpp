#include <stdio.h>
#include <nds.h>
#include <nf_lib.h>
#include <nds/ndstypes.h>
#include <nds/arm9/background.h>
#include <nds/arm9/keyboard.h>

void doText(u8 screen, u8 layer, u16 x, u16 y, const char *text, bool reset)
{
	if(reset) NF_ClearTextLayer(0, 0);
	NF_WriteText(screen, layer, x, y, text);
	NF_UpdateTextLayers();
}

int main(int argc, char **argv) 
{	
  Keyboard     kb;
  char  enteredText[1024];

  videoSetMode(MODE_0_2D);
  videoSetModeSub(MODE_0_2D);

  vramSetPrimaryBanks(VRAM_A_MAIN_BG,
                      VRAM_B_MAIN_SPRITE,
                      VRAM_C_SUB_BG,
                      VRAM_D_SUB_SPRITE);

  // https://github.com/mtheall/nds_examples/blob/master/keyboard/source/main.c
  keyboardInit(&kb,    0, BgType_Text4bpp, BgSize_T_256x512, 14, 0, false, true);

	NF_Set2D(0,0);
	NF_SetRootFolder("NITROFS");

	NF_InitTiledBgBuffers();
	NF_InitTiledBgSys(0);
	
	NF_InitSpriteBuffers();
	NF_InitSpriteSys(0);
	
	NF_InitTextSys(0);
	
	NF_LoadTextFont("fonts/font", "default", 256, 256, 0);
	NF_CreateTextLayer(0, 0, 0, "default");

	keyboardGetString(enteredText, 1024);

	while(1) {
		doText(0, 0, 2, 2, enteredText, true);
	}

	return 0;
}
