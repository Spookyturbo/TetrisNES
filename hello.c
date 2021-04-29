
/*
A simple "hello world" example.
Set the screen background color and palette colors.
Then write a message to the nametable.
Finally, turn on the PPU to display video.
*/

#include "neslib.h"
#include "stdlib.h"
#include "vrambuf.h"
//#link "vrambuf.c"

typedef unsigned char uint8;
typedef signed char int8;

//Function prototypes
void gameloop(void);
void setLevelPal(uint8 level);

void setLevel(int level);
void setScore(long score);

//Color is the offset used in the chr table to choose the sprite
uint8 drawTetrimino(uint8 id, uint8 rotation, uint8 row, uint8 col, uint8 sprid, uint8 color);
uint8 displayNextTetrimino(uint8 id, uint8 sprid, uint8 color);

//Const and defines

//Screen
const uint8 Tetris_Screen_RLE[793]={
0x01,0x9f,0x8e,0x8f,0x01,0x02,0x8e,0x01,0x02,0x8f,0x8f,0x8e,0x8f,0x8e,0x8f,0x01,
0x02,0x8e,0x8e,0x8f,0x8f,0x8e,0x8f,0x8e,0x8f,0x01,0x02,0x8e,0x8e,0x8f,0x8e,0x8f,
0x8e,0x8f,0x01,0x02,0x8e,0x8f,0x9e,0x9e,0x8f,0x8f,0x9e,0x9e,0x9f,0x8e,0x8f,0x01,
0x02,0x9e,0x8f,0x8f,0x9e,0x9e,0x9f,0x8e,0x8f,0x01,0x02,0x9e,0x8f,0x9e,0x8f,0x9e,
0x9e,0x8f,0x8e,0x8f,0x8e,0x9e,0x9e,0x8e,0xc0,0xc1,0x01,0x09,0xc2,0xc0,0xc1,0x01,
0x05,0xc2,0x9e,0x8e,0x8f,0x9e,0x9e,0x9f,0x9e,0x9f,0x9e,0x01,0x03,0xd0,0x00,0x00,
0x54,0x45,0x54,0x52,0x49,0x53,0x00,0x00,0xd2,0xd0,0x00,0x01,0x05,0xd2,0x8e,0x9e,
0x9f,0x8e,0x9e,0x8e,0x8f,0x8f,0x9e,0x8e,0x8f,0x9e,0xe0,0xe1,0x01,0x09,0xe2,0xd0,
0x82,0x00,0x01,0x04,0xd2,0x9e,0x8e,0x8f,0x9e,0x8f,0x8e,0x8f,0x9e,0x01,0x02,0x9f,
0x9e,0xa5,0xa6,0x01,0x09,0xa7,0xd0,0x53,0x43,0x4f,0x52,0x45,0x3a,0xd2,0x9e,0x8f,
0x9e,0x8f,0x9e,0x9e,0x8e,0x8f,0x8e,0x8e,0x8f,0x8f,0xb5,0x00,0x01,0x09,0xb7,0xd0,
0x30,0x01,0x05,0xd2,0x8e,0x01,0x02,0x8f,0x8f,0x9e,0x9e,0x9f,0x9e,0x8f,0x8f,0x9e,
0xb5,0x00,0x01,0x09,0xb7,0xd0,0x00,0x01,0x05,0xd2,0x9e,0x9e,0xee,0x8e,0x9e,0x8e,
0x8f,0x8e,0x8f,0x8f,0x8e,0x8f,0xb5,0x00,0x01,0x09,0xb7,0xd0,0x00,0x01,0x05,0xd2,
0x9e,0x01,0x03,0x8f,0x8e,0x9e,0x8f,0x8e,0x9e,0x9e,0x9f,0xb5,0x00,0x01,0x09,0xb7,
0xe0,0xe1,0x01,0x05,0xe2,0x9e,0x8e,0xcc,0x8f,0x9e,0x9e,0x8f,0x8e,0xcc,0x8e,0x8f,
0x8e,0xb5,0x00,0x01,0x09,0xb7,0xed,0xee,0x01,0x02,0xed,0xee,0xef,0x8e,0x8f,0x8e,
0x8f,0x01,0x02,0x9e,0x8e,0x9e,0x8e,0x9e,0x9f,0x9e,0xb5,0x00,0x01,0x09,0xb7,0xa5,
0xa6,0x01,0x03,0xa7,0xff,0x9e,0x9f,0x8e,0x8f,0x8f,0x8e,0x8e,0xcc,0x8e,0x9e,0x8f,
0x8f,0x9e,0xb5,0x00,0x01,0x09,0xb7,0xb5,0x4e,0x45,0x58,0x54,0xb7,0x8e,0x8e,0x8f,
0x01,0x02,0x9e,0x01,0x02,0x8e,0x9e,0x8f,0x8e,0x8f,0x9e,0xb5,0x00,0x01,0x09,0xb7,
0xb5,0x00,0x01,0x03,0xb7,0x9e,0x8f,0x8f,0x8e,0x8f,0x8e,0xcc,0x8e,0xcc,0x8f,0x9e,
0x9e,0x9f,0x8e,0xb5,0x00,0x01,0x09,0xb7,0xb5,0x00,0x01,0x03,0xb7,0x8e,0x8f,0x8e,
0x8f,0x9e,0x8f,0x8e,0x01,0x02,0x8f,0x8f,0x8e,0x8f,0xcc,0xb5,0x00,0x01,0x09,0xb7,
0xb5,0x00,0x01,0x03,0xb7,0x8e,0x9e,0x9e,0x9f,0x8e,0x8e,0xcc,0x9e,0x8e,0x9e,0x8e,
0x8f,0x8e,0x8f,0xb5,0x00,0x01,0x09,0xb7,0xb5,0x00,0x01,0x03,0xb7,0x9e,0x9e,0x8e,
0x8f,0xcc,0x9e,0x8e,0x9e,0x9e,0x8e,0x8f,0x9e,0x8f,0x9e,0xb5,0x00,0x01,0x09,0xb7,
0xc5,0xc6,0x01,0x03,0xc7,0x9e,0x8f,0x8e,0x8e,0x8f,0x8e,0xcc,0x9e,0x01,0x02,0x9f,
0x8e,0x8f,0x9e,0xb5,0x00,0x01,0x09,0xb7,0xc0,0xc1,0x01,0x04,0xc2,0x8e,0xcc,0x9e,
0x9f,0x9e,0x8e,0x8e,0x9e,0x8e,0x8e,0xcc,0x8e,0x8f,0xb5,0x00,0x01,0x09,0xb7,0xd0,
0x4c,0x45,0x56,0x45,0x4c,0xd2,0x8e,0x9e,0x8e,0x8f,0x8f,0x9e,0x9e,0x8e,0x9e,0x8f,
0x8e,0x9e,0x9f,0xb5,0x00,0x01,0x09,0xb7,0xd0,0x00,0x01,0x02,0x30,0x00,0xd2,0x9e,
0x8f,0x8f,0x9e,0x8e,0xcc,0x9e,0x9e,0x8f,0x9e,0x9e,0x8f,0x8f,0xb5,0x00,0x01,0x09,
0xb7,0xe0,0xe1,0x01,0x04,0xe2,0x8e,0x8e,0x8f,0x8e,0x8f,0x8f,0x9e,0x9e,0x8e,0x8e,
0x8f,0x8e,0x8f,0xb5,0x00,0x01,0x09,0xb7,0x8e,0x8e,0x8f,0x8f,0x8e,0x8f,0x8e,0x9e,
0x8f,0x9e,0x8f,0x8e,0x9e,0x8e,0x8e,0xcc,0x9e,0x8e,0x8f,0x9e,0xb5,0x00,0x01,0x09,
0xb7,0x9e,0x8e,0x8f,0x9e,0x9e,0x9f,0x9e,0x9e,0x8e,0x8f,0x8e,0xcc,0x8f,0x9e,0x9e,
0x8e,0x9e,0x9e,0x8e,0x9e,0xb5,0x00,0x01,0x09,0xb7,0x9e,0x9e,0x8e,0x8e,0x8f,0x8f,
0x9e,0x8f,0x9e,0x9f,0x8e,0x01,0x02,0x9e,0x8f,0x9e,0x8e,0x9e,0x9e,0x8f,0xb5,0x00,
0x01,0x09,0xb7,0x9e,0x01,0x02,0x8f,0x9e,0x8e,0x8e,0x8f,0x8f,0x8e,0xcc,0x9e,0x9e,
0x8e,0x8f,0x9e,0x9e,0x8f,0x8e,0x9e,0xc5,0xc6,0x01,0x09,0xc7,0x8e,0x8f,0x9e,0x8e,
0x8e,0xcc,0x8e,0x8f,0x9e,0x9e,0x8e,0xcc,0x9e,0x9e,0x8e,0x9e,0x9e,0x8e,0x9e,0x8f,
0x8f,0x8e,0x8f,0x01,0x02,0x8e,0x8f,0x8f,0x8e,0x8f,0x8e,0x8f,0x8e,0x9e,0x8e,0xcc,
0x8f,0x9e,0x9e,0x9f,0x8e,0x8f,0x8e,0x8f,0x9e,0x01,0x02,0x8e,0x8f,0x9e,0x8f,0x8f,
0x8e,0x8f,0x8e,0x8f,0x8f,0x8e,0x8f,0x9e,0x9e,0x9f,0x9e,0x8e,0xcc,0x9e,0x8e,0x8e,
0x8f,0x8e,0x8f,0x8f,0x9e,0x9f,0x9e,0x8e,0x8f,0x8f,0xcc,0x9e,0x9f,0x8e,0x8f,0x01,
0x02,0x9e,0x8f,0x9e,0x8e,0xcc,0x8e,0x8f,0x01,0x02,0x9e,0x9e,0x8e,0x8f,0xcc,0x9e,
0x9f,0x9e,0x8e,0x8f,0x01,0x02,0x9e,0x9e,0x9f,0xff,0x01,0x09,0x3f,0x0f,0xff,0x01,
0x06,0x00,0xff,0x01,0x05,0x3f,0x33,0xff,0x01,0x05,0xf3,0xf3,0xff,0x01,0x05,0x3f,
0xff,0x01,0x0c,0x0f,0x01,0x06,0x0f,0x01,0x00
};

#define T_TETRIMINO 0
#define J_TETRIMINO 1
#define Z_TETRIMINO 2
#define O_TETRIMINO 3
#define S_TETRIMINO 4
#define L_TETRIMINO 5
#define I_TETRIMINO 6

//Every Tetrimino has its rotation represented in 4 even if there are repeats
//Just makes things easier
//Reference order Tetrimino[ID][Rotation_Num][Block_Num][0=x, 1=y]
const int8 Tetriminos[][4][4][2] = {
  { //T
    {{-1, 0}, {0, 0}, {1, 0}, {0, 1}}, //T Down (Spawn)
    {{0, -1}, {-1, 0}, {0, 0}, {0, 1}}, //T Left
    {{-1, 0}, {0, 0}, {1, 0}, {0, -1}}, //T Up
    {{0, -1}, {0, 0}, {1, 0}, {0, 1}} //T Right
  },
  
  { //J
    {{-1, 0}, {0, 0}, {1, 0}, {1, 1}}, //J Down (Spawn)
    {{0, -1}, {0, 0}, {-1, 1}, {0, 1}}, //J Left
    {{-1, -1}, {-1, 0}, {0, 0}, {1, 0}}, //J Up
    {{0, -1}, {1, -1}, {0, 0}, {0, 1}} //J Right
  },
  
  { //Z
    {{-1, 0}, {0, 0}, {0, 1}, {1, 1}}, //Z Horizontal (Spawn)
    {{1, -1}, {0, 0}, {1, 0}, {0, 1}}, //Z Vertical
    {{-1, 0}, {0, 0}, {0, 1}, {1, 1}}, //Z Horizontal
    {{1, -1}, {0, 0}, {1, 0}, {0, 1}} //Z Vertical
  },
  
  { //O
    {{-1, 0}, {0, 0}, {-1, 1}, {0, 1}}, //O (Spawn)
    {{-1, 0}, {0, 0}, {-1, 1}, {0, 1}}, //O
    {{-1, 0}, {0, 0}, {-1, 1}, {0, 1}}, //O
    {{-1, 0}, {0, 0}, {-1, 1}, {0, 1}}, //O
  },
  
  { //S
    {{0, 0}, {1, 0}, {-1, 1}, {0, 1}}, //S Horizontal (Spawn)
    {{0, -1}, {0, 0}, {1, 0}, {1, 1}}, //S Vertical
    {{0, 0}, {1, 0}, {-1, 1}, {0, 1}}, //S Horizontal
    {{0, -1}, {0, 0}, {1, 0}, {1, 1}} //S Vertical
  },
  
  { //L
    {{-1, 0}, {0, 0}, {1, 0}, {-1, 1}}, //L Down (Spawn)
    {{-1, -1}, {0, -1}, {0, 0}, {0, 1}}, //L Left
    {{1, -1}, {-1, 0}, {0, 0}, {1, 0}}, //L Up
    {{0, -1}, {0, 0}, {0, 1}, {1, 1}} //L Right
  },
  
  { //I
    {{-2, 0}, {-1, 0}, {0, 0}, {1, 0}}, //I Horizontal (Spawn)
    {{0, -2}, {0, -1}, {0, 0}, {0, 1}}, //I Vertical
    {{-2, 0}, {-1, 0}, {0, 0}, {1, 0}}, //I Horizontal
    {{0, -2}, {0, -1}, {0, 0}, {0, 1}} //I Vertical
  }
};


// link the pattern table into CHR ROM
//#link "chr_generic.s"

/*{pal:"nes",layout:"nes"}*/
const uint8 BG_PAL[16]={ 0x0F,0x1A,0x10,0x30,0x0F,0x16,0x21,0x30,0x0F,0x14,0x16,0x30,0x0F,0x2C,0x00,0x30 };

/*{pal:"nes",layout:"nes"}*/
const uint8 SPR_PAL[16]={ 0x0F,0x16,0x12,0x30,0x0F,0x16,0x12,0x30,0x0F,0x14,0x00,0x30,0x0F,0x2C,0x00,0x30 };

const uint8 LEVEL_PAL[10][4] = {
  {0x0F, 0x21, 0x12, 0x30},
  {0x0F, 0x29, 0x1A, 0x30},
  {0x0F, 0x24, 0x14, 0x30},
  {0x0F, 0x2A, 0x12, 0x30},
  {0x0F, 0x2B, 0x15, 0x30},
  {0x0F, 0x22, 0x2B, 0x30},
  {0x0F, 0x00, 0x16, 0x30},
  {0x0F, 0x05, 0x13, 0x30},
  {0x0F, 0x16, 0x12, 0x30},
  {0x0F, 0x27, 0x16, 0x30}
};

//Globals

//Access as [row][col]
//-1 = Empty, 0-3 is the color of that spot
int8 playfield[20][10];


// main function, run after console reset
void main(void) {
  // set up palette colors
  pal_bg(BG_PAL);
  pal_spr(SPR_PAL);
  
  //Draw Initial Screen
  vram_adr(0x2000);		
  vram_unrle(Tetris_Screen_RLE);
  
  // enable PPU rendering (turn on screen)
  ppu_on_all();
  
  //Setup vram changes
  vrambuf_clear();
  set_vram_update(updbuf);
  
  gameloop();
}

void gameloop() {
  int i = 0;
  
  //Just tests for now
  setScore(23456);
  setLevel(20);
  setLevelPal(0);
  while(1) {
    int sprid = 0;
    //int x = 64 + (Tetriminos[0][0][3][0] << 3); //multiply by 8
    //int y = 48 + (Tetriminos[0][0][3][1] << 3);
    //sprid = oam_spr(x, y, 0x80, 0, sprid);
    
    sprid = drawTetrimino(0, 0, 0, 5, sprid, 0);
    sprid = drawTetrimino(0, 0, 0, 1, sprid, 1);
    sprid = drawTetrimino(0, 0, 3, 1, sprid, 2);
    sprid = displayNextTetrimino(O_TETRIMINO, sprid, 0);

    oam_hide_rest(sprid);
    vrambuf_flush();
  }
}

//Note level can't go above 99. Also, if you can even get past level 30 you're a god
void setLevel(int level) {
  char lstring[3];
  int row = 23;
  int len = 1;
  
  if(level > 9) {
    row = 22;
    len = 2;
  }
  
  itoa(level, lstring, 10);
  vrambuf_put(NTADR_A(row, 20), lstring, len);
}

void setScore(long score) {
  char scoreString[7];
  int row = 25;
  int len = 1;
  
  if(score >= 100000) {
    row = 20;
    len = 6;
  }
  else if(score >= 10000) {
    row = 21;
    len = 5;
  }
  else  if(score >= 1000) {
    row = 22;
    len = 4;
  }
  else if(score >= 100) {
    row = 23;
    len = 3;
  }
  else if(score >= 10) {
    row = 24;
    len = 2;
  }
  
  ltoa(score, scoreString, 10);
  vrambuf_put(NTADR_A(row, 6), scoreString, len);
}

//The row and col are for the tetris play field, not pixels
//Color is a number 0-2
uint8 drawTetrimino(uint8 id, uint8 rotation, uint8 row, uint8 col, uint8 sprid, uint8 color) {
  //There are 2 fake rows above row 0 for spawning
  int block;
  int centerX = 64 + (col << 3); //Multiplying by 8
  int centerY = 48 + (row << 3);
  
  for(block = 0; block < 4; block++) {
    int8 xoffset = Tetriminos[id][rotation][block][0];
    int8 yoffset = Tetriminos[id][rotation][block][1];
    int x;
    int y;
    
    //Not checking for other conditions because this is the only one that
    //should ever happen, and I would prefer to see the error so it is obvious
    //then hide it
    if((yoffset + (int8)row) < 0)
      continue;
    
    x = centerX + (xoffset << 3); //multiply by 8
    y = centerY + (yoffset << 3);
    
  
    sprid = oam_spr(x, y, 0x83 + color, 0, sprid);
  }
  
  return sprid;
}

//To render in the middle of the display, must be a sprite
//Color is a number 0-2
uint8 displayNextTetrimino(uint8 id, uint8 sprid, uint8 color)
{
  int block;
  int centerX = 172;
  int centerY = 112;
  
  if(id == I_TETRIMINO || id == O_TETRIMINO)
    centerX = 176;
  
  for(block = 0; block < 4; block++) {
    int8 xoffset = Tetriminos[id][0][block][0];
    int8 yoffset = Tetriminos[id][0][block][1];
    int x;
    int y;
    
    x = centerX + (xoffset << 3); //multiply by 8
    y = centerY + (yoffset << 3);
    
    sprid = oam_spr(x, y, 0x83 + color, 0, sprid);
  }
  
  return sprid;
}

void setLevelPal(uint8 level) {
  int i;
  //My own modulo
  while(level > 9)
    level -= 10;
  for(i = 0; i < 4; i++)
    pal_col(16+i, LEVEL_PAL[level][i]);
}
