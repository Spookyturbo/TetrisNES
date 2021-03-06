
#include "neslib.h"
#include "stdlib.h"
#include "vrambuf.h"
#include "string.h"
#include "apu.h"
//#link "apu.c"
//#link "vrambuf.c"

typedef unsigned char uint8;
typedef signed char int8;

//Function prototypes
void gameloop(void);

void setLevelPal(uint8 level);
void setLevel(uint8 level);
void setLevelSpeed(uint8 level);
  
void setScore(long score);

//Color is the offset used in the chr table to choose the sprite
uint8 drawTetrimino(struct Tetrimino* tet, uint8 sprid);
uint8 displayNextTetrimino(uint8 id, uint8 sprid, uint8 color);

uint8 isValidSpot(struct Tetrimino* tet);

//Returns true if a change was made to the tetrimino, false if it stayed the same
//Ensures the move done is valid
uint8 moveTetrimino(struct Tetrimino* tet, int8 colChange, int8 rotationChange);

//Using the given tetrimino, lock it into the background
void lockTetrimino(struct Tetrimino* tet);
void spawnTetrimino(struct Tetrimino* tet, uint8 id, uint8 color);

//Using the just place tetrimino, find out what rows, if any have been solved
//Clear them, and return the number of rows cleared
uint8 clearRows(struct Tetrimino* tet);
//This actually does the changes to the playfield array when clearing
void clearPlayfieldRows(int8 rowsToClear[], uint8 len);
void drawPlayfield(void);
uint8 isRowEmpty(uint8 row);

int getPoints(uint8 rowsCleared, uint8 level);

//Returns the wrapped value
int8 wrap(int8 v, int8 min, int8 max);
void selectionSort(int8 arr[], int8 n);

//Advances the random value and returns it
unsigned int random(void);

//Music
byte next_music_byte(void);
void play_music(void);
void start_music(const byte* music);
void play_sound(const word vals[]);
//Const and defines


//Music
const int note_table[64] = { 2152,4063,3835,3620,3417,3225,3044,2873,2712,2560,2416,2280,2152,2032,1918,1810,1708,1612,1522,1437,1356,1280,1208,1140,1076,1016,959,905,854,806,761,718,678,640,604,570,538,508,479,452,427,403,380,359,339,320,302,285,269,254,240,226,214,202,190,180,169,160,151,143,135,127,120,113,};
const int note_table_tri[64] = { 2138, 2018, 1905, 1798, 1697, 1602, 1512, 1427, 1347, 1272, 1200, 1133, 1069, 1009, 953, 899, 849, 801, 756, 714, 674, 636, 601, 567, 535, 505, 477, 450, 425, 401, 379, 358, 338, 319, 301, 284, 268, 253, 239, 226, 213, 201, 190, 179, 169, 160, 151, 142, 135, 127, 120, 113, 107, 101, 95, 90, 85, 80, 76, 72, 68, 64, 60, 57, };
const byte tetris_theme[] = {0x32,0x37,0x13,0x8c,0x1f,0x8c,0x2f,0x32,0x13,0x8c,0x30,0x33,0x1f,0x8c,0x32,0x35,0x13,0x8c,0x37,0x1f,0x86,0x35,0x86,0x30,0x33,0x13,0x8c,0x2f,0x32,0x1f,0x8d,0x2b,0x30,0x18,0x8d,0x24,0x8b,0x2b,0x30,0x18,0x8c,0x30,0x33,0x24,0x8c,0x33,0x37,0x18,0x8c,0x24,0x8c,0x32,0x35,0x18,0x8c,0x30,0x33,0x24,0x8c,0x32,0x2f,0x17,0x8d,0x2b,0x23,0x8b,0x2f,0x17,0x8c,0x33,0x30,0x23,0x8c,0x32,0x35,0x13,0x8c,0x1f,0x8c,0x33,0x37,0x13,0x8c,0x1f,0x8d,0x30,0x33,0x18,0x8d,0x24,0x8c,0x2b,0x30,0x18,0x8c,0x24,0x8b,0x2b,0x30,0x18,0x8c,0x24,0x8c,0x1a,0x8c,0x1b,0x8c,0x1d,0x8d,0x2c,0x35,0x11,0x98,0x30,0x38,0x11,0x8b,0x3c,0x33,0x8c,0x33,0x11,0x86,0x33,0x86,0x32,0x3a,0x18,0x8c,0x30,0x38,0x14,0x8d,0x2e,0x37,0x0f,0x8c,0x1b,0x98,0x2b,0x33,0x1b,0x8c,0x37,0x2e,0x0f,0x8c,0x30,0x16,0x86,0x2e,0x86,0x2c,0x35,0x8c,0x2b,0x33,0x16,0x8c,0x32,0x2f,0x1a,0x8c,0x2b,0x26,0x8c,0x2f,0x32,0x8c,0x30,0x33,0x26,0x8c,0x35,0x32,0x8c,0x2f,0x1f,0x8c,0x37,0x33,0x8c,0x2f,0x23,0x8c,0x33,0x30,0x18,0x8c,0x2b,0x1f,0x8c,0x2b,0x30,0x18,0x8c,0x1f,0x8c,0x2b,0x30,0x18,0xb0,0x27,0x2b,0x18,0x8c,0x1f,0x8c,0x18,0x8c,0x1f,0x8c,0x24,0x27,0x18,0x8c,0x1f,0x8c,0x18,0x8c,0x1f,0x8c,0x26,0x29,0x17,0x8c,0x1f,0x8c,0x17,0x8c,0x1f,0x8c,0x23,0x26,0x17,0x8c,0x1f,0x8c,0x17,0x8c,0x1f,0x8d,0x24,0x27,0x18,0x8d,0x1f,0x8b,0x18,0x8c,0x1f,0x8c,0x1f,0x24,0x18,0x8c,0x1f,0x8c,0x18,0x8c,0x1f,0x8c,0x1f,0x23,0x17,0x8d,0x1f,0x8b,0x17,0x8c,0x1f,0x8c,0x23,0x26,0x17,0x8c,0x1f,0x8c,0x17,0x8c,0x1f,0x8d,0x27,0x2b,0x18,0x8d,0x1f,0x8c,0x18,0x8c,0x1f,0x8c,0x24,0x27,0x18,0x8b,0x1f,0x8c,0x18,0x8c,0x1f,0x8c,0x26,0x29,0x17,0x8d,0x1f,0x8c,0x17,0x8c,0x1f,0x8c,0x23,0x26,0x17,0x8b,0x1f,0x8c,0x17,0x8c,0x1f,0x8c,0x24,0x27,0x18,0x8d,0x1f,0x8c,0x27,0x2b,0x18,0x8c,0x1f,0x8c,0x2b,0x30,0x18,0x8b,0x1f,0x8c,0x18,0x8c,0x1f,0x8c,0x2b,0x2f,0x17,0x8d,0x1f,0x8c,0x17,0x8c,0x1f,0x8c,0x17,0x8b,0x1f,0x8c,0x17,0x8c,0x1f,0xff};

const word placeEffect[20] = {1646, 64, 1, 7, 5, 0, 0, 1162, 192, 4, 4, 1, 4, 1, 1158, 15, 0, 0, 0, 0};
const word clearEffect[20] = {4, 128, 12, 6, 5, 3, 0, 1153, 64, 3, 13, 6, 1, 1};

#define NOTE_TABLE note_table
#define BASS_NOTE 36

const byte* musicPtr = tetris_theme;
byte cur_duration = 0;

//Screen
const unsigned char Tetris_Screen_RLE[806]={
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
0x9f,0x9e,0x8e,0x8f,0x01,0x02,0x9e,0x9e,0x9f,0xff,0x01,0x09,0x0f,0x0f,0xcf,0xff,
0x01,0x04,0x00,0x00,0xcc,0xff,0x01,0x04,0x00,0x00,0xcc,0xff,0x01,0x04,0x00,0x00,
0xcc,0xff,0x01,0x04,0x00,0x00,0xcc,0xff,0x01,0x04,0xf0,0xf0,0xfc,0xff,0x01,0x02,
0x0f,0x01,0x06,0x0f,0x01,0x00
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

//Some stuff
struct Tetrimino {
  uint8 id;
  int8 rotation;
  int8 row;
  int8 col;
  uint8 color;
};

//Globals

//Access as [row][col]
//-1 = Empty, 0-3 is the color of that spot
//The _ is a back end and the actual one allows rearranging of rows
//Idk why I had to do it like this, I wanted to just malloc and have one of these
//But it was getting very mad
int8 _playfield[20][10];
int8* playfield[20];

uint8 framesPerDrop;
unsigned int seed = 86712333;

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
  int8 gameover = 0;
  int8* play[20];
  int frameCount = 0;
  int8 level = 0;
  int8 clearCounter = 0;
  long score;
  uint8 row;
  uint8 col;
  int8 dropLock = 0;
  
  //The falling tetris piece
  struct Tetrimino tet = {0 , 0, 0, 5, 0};
  struct Tetrimino futureTet;
  uint8 nextPieceId = 1;
  uint8 nextPieceColor = 1;
 
  //Initialize the playfield
  for(row = 0; row < 20; row++) {
    int8* rowArray = (int8*) malloc(100);
    play[row] = rowArray;
    //playfield[row] = rowArray;
    for(col = 0; col < 10; col++) {
      _playfield[row][col] = -1; 
    }
    //Connect the back end to the front end
    playfield[row] = _playfield[row];
  }
	
  //Setup Music
  apu_init();
  start_music(tetris_theme);
  
  //Set initial values
  setScore(0);
  setLevel(0);
  setLevelPal(0);
  setLevelSpeed(0);
  
  while(!gameover) {
    int8 locked = 0;
    int8 cleared = 0;
    uint8 padResult = pad_trigger(0);
    uint8 sprid = 0;
    int8 colChange = 0;
    int8 rotationChange = 0;
    int8 framesTillDrop = framesPerDrop - frameCount;
    int8 fastDropped = 0;
    
    //Handle the logic for input
    //Input that affects turning/moving/fast dropping happens BEFORE 
    //the actual dropping. This means that if the new position would result
    //in a lock when it drops, it will still perform the move
    if(dropLock) {
      if(!pad_poll(0))
        dropLock = 0;
    }
    else if(pad_poll(0) & PAD_DOWN) {
      //Fast Fall
      fastDropped = true;
      framesTillDrop = 0;
    }
    
    if(padResult & PAD_LEFT) {
      colChange = -1;
      seed += 200;
    }
    else if(padResult & PAD_RIGHT) {
      colChange = 1;
      seed += 300;
    }
    
    if(padResult & PAD_B)
      rotationChange -= 1;
    if(padResult & PAD_A)
      rotationChange += 1;
    
    //Variables are used to determine what needs changed so every ordering of input
    //can be checked.
    //Get the move that should happen
    //This handles all the error conditions as well
    moveTetrimino(&tet, colChange, rotationChange);
    
    //Next check if a drop should occur and handle it
    if(framesTillDrop <= 0) {
      futureTet = tet;
      futureTet.row++;
      frameCount = 0;
      
      //If the spot isn't valid, it is time to lock into place
      //Check for lines, spawn the next piece, etc...
      if(!isValidSpot(&futureTet)) {
        int8 numCleared = 0;
        //Lock into place and clear
        lockTetrimino(&tet);
        locked = 1;
        if(fastDropped)
          dropLock = true;

        numCleared = clearRows(&tet);
        if(numCleared > 0) cleared = 1;
        //Update level
        clearCounter += numCleared;
        if(clearCounter >= 10) {
          clearCounter -= 10;
          level++;
          setLevel(level);
          setLevelPal(level);
          setLevelSpeed(level);
        } 
        
        //Update Score
        score += getPoints(numCleared, level);
        setScore(score);
        
        //Spawn Next Piece
        spawnTetrimino(&tet, nextPieceId, nextPieceColor);
        
        //GAMEOVER
        if(!isValidSpot(&tet)) {
          gameover = 1; 
        }
        
        //Generate the next piece
        //Because I don't like lots of duplicates in a row
        //Having it add value that wraps around to choose the peice
        //Duplicates can still happen, just hopefull less often
        nextPieceId += (random() >> 8) & 15;
        nextPieceId = wrap(nextPieceId, 0, 6);
        
        nextPieceColor = wrap((random() >> 8) & 7, 0, 2);
        
      }
      else {
        tet = futureTet; 
      }
    }
      
    
    //Drawing
    sprid = drawTetrimino(&tet, sprid);
    sprid = displayNextTetrimino(nextPieceId, sprid, nextPieceColor);
    
    if(!musicPtr) start_music(tetris_theme);
      play_music();
    
    if(cleared)
      play_sound(clearEffect);
    else if(locked)
      play_sound(placeEffect);
    
    oam_hide_rest(sprid);
    
    //Wait for next frame and update count
    frameCount++;
    vrambuf_flush();
  }
  
  //Game over
  vrambuf_put(NTADR_A(9, 15), "GAMEOVER", 8);
  vrambuf_flush();
  while(1);
}

//Note level can't go above 99. Also, if you can even get past level 30 you're a god
void setLevel(uint8 level) {
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
uint8 drawTetrimino(struct Tetrimino* tet, uint8 sprid) {
  //There are 2 fake rows above row 0 for spawning
  int block;
  int centerX = 64 + (tet->col << 3); //Multiplying by 8
  
  //I don't really know why this is 47 to line up with the
  //nametable, but it is
  int centerY = 47 + (tet->row << 3);
  
  for(block = 0; block < 4; block++) {
    int8 xoffset = Tetriminos[tet->id][tet->rotation][block][0];
    int8 yoffset = Tetriminos[tet->id][tet->rotation][block][1];
    int x;
    int y;
    
    //Not checking for other conditions because this is the only one that
    //should ever happen, and I would prefer to see the error so it is obvious
    //then hide it
    if((yoffset + tet->row) < 0)
      continue;
    
    x = centerX + (xoffset << 3); //multiply by 8
    y = centerY + (yoffset << 3);
    
  
    sprid = oam_spr(x, y, 0x83 + tet->color, 0, sprid);
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
  
  //Set the first background palette and first sprite
  for(i = 0; i < 4; i++) {
    pal_col(16+i, LEVEL_PAL[level][i]);
    pal_col(0+i, LEVEL_PAL[level][i]);
  }
}

void setLevelSpeed(uint8 level) {
  //Linear scaling
  if(level < 9) {
    framesPerDrop = 48;
    //FramesperDrop -= 5 * level
    while(level > 0) {
      framesPerDrop -= 5;
      level--;
    }
    return;
  }
  
  //Non linear scaling
  if(level > 28)
    framesPerDrop = 1;
  else if(level > 18)
    framesPerDrop = 2;
  else if(level > 15)
    framesPerDrop = 3;
  else if(level > 12)
    framesPerDrop = 4;
  else if(level > 9)
    framesPerDrop = 5;
  else
    framesPerDrop = 6;
}

//Takes a tetrimino
void lockTetrimino(struct Tetrimino* tet) {
  uint8 block;
  int originX = 8;
  int originY = 6;
  
  for(block = 0; block < 4; block++) {
    //Row and col of this block
    int row = tet->row + Tetriminos[tet->id][tet->rotation][block][1];
    int col = tet->col + Tetriminos[tet->id][tet->rotation][block][0];
    
    //Ignore pieces above the map
    if(row < 0)
      continue;
    
    //set it in the playfield
    playfield[row][col] = tet->color;
    
    //Draw single byte to the nametable
    VRAMBUF_PUT(NTADR_A(originX + col, originY + row), 1, NT_UPD_HORZ);
    VRAMBUF_ADD(0x83 + tet->color);
    vrambuf_end();
    //Ensure this renders before the tetrimino is considered "Spawned" And teleported
    //Without this, there will be a flash when a piece is locked
    vrambuf_flush();
  }
}

uint8 clearRows(struct Tetrimino* tet) {
  uint8 block;
  uint8 i;
  uint8 numRowsChecked = 0;
  uint8 rowsCleared = 0;
  uint8 skip;
  int8 rowsChecked[4] = {-1, -1, -1, -1};
  //Because of block size only 4 rows can ever be cleared at once
  int8 rowsToClear[4] = {-1, -1, -1, -1};
  
  //Find the rows that need cleared
  for(block = 0; block < 4; block++) {
    //Row and col of this block
    int8 row = tet->row + Tetriminos[tet->id][tet->rotation][block][1];
    
    skip = 0;
    //Check to see if this row has been checked yet
    for(i = 0; i < numRowsChecked; i++) {
      if(rowsChecked[i] == row) {
        skip = 1;
        break;
      }
    }
    
    if(skip)
      continue;
    
    rowsChecked[numRowsChecked] = row;
    numRowsChecked++;
    
    skip = 0;
    //Now check the row
    for(i = 0; i < 10; i++) {
       if(playfield[row][i] == -1) {
         skip = 1;
         break;
       }
    }
    
    //Row had a hole in it, check next
    if(skip)
      continue;
    
    //This row is full! flag it to be cleared
    //When flagging, sort highest to lowest
    rowsToClear[rowsCleared] = row;
    rowsCleared++;
  }
  
  //Clearing and condensing is done after everything is found, 
  //that way messing with the playfield doesn't affect the checks
  clearPlayfieldRows(rowsToClear, rowsCleared);
  
  //Redraw the updated playfield
  if(rowsCleared)
    drawPlayfield();
  
  return rowsCleared;
}

void clearPlayfieldRows(int8 rowsToClear[], uint8 len) {
  int8* emptyRows[4];
  int8 rowIndex;
  int8 curClearRow;
  int8 nextClearRow;
  int8 curEmptyRow;
  int8 row;
  int8 i;
  
  if(len == 0)
    return;
  
  //clear all the rows that need cleared
  for(rowIndex = 0; rowIndex < len; rowIndex++) {
    row = rowsToClear[rowIndex];
    emptyRows[rowIndex] = playfield[row];
    for(i = 0; i < 10; i++) {
      playfield[row][i] = -1; 
    }
  }
  
  //Move all the rows down
  
  //Sort the rows array high to low
  selectionSort(rowsToClear, len);
  
  //Setup initial row to be copied to
  curEmptyRow = rowsToClear[0];
  
  //Loop through the sets of rows to clear
  for(rowIndex = 0; rowIndex < len - 1; rowIndex++) {
    curClearRow = rowsToClear[rowIndex];
    //If there isn't a next row that got cleared, this loop exits
    nextClearRow = rowsToClear[rowIndex + 1];
   
    //Move all the rows down that are between this and the next clear row
    for(row = curClearRow - 1; row > nextClearRow; row--) {
      playfield[curEmptyRow] = playfield[row];
      //memcpy(playfield[curEmptyRow], playfield[row], sizeof(playfield[row]));
      curEmptyRow--; //Set the row just moved down as the empty row
    }
    
  }
  
  //There is no next cleared row, so loop and bring everything down 
  curClearRow = rowsToClear[len - 1];
  row = curClearRow - 1;
  while(row >= 0) {
    playfield[curEmptyRow] = playfield[row];
    
    curEmptyRow--; //Set the row just moved down as the empty row
    row--;
  }
  
  //Place the cleared rows at the top
  for(rowIndex = 0; rowIndex < len;  rowIndex++) {
    playfield[rowIndex] = emptyRows[rowIndex]; 
  }
}

void drawPlayfield() {
  int row;
  int i;
  
  for(row = 19; row > -1; row--) {
    char rowValues[10];
    for(i = 0; i < 10; i++) {
      if(playfield[row][i] == -1)
        rowValues[i] = 0x00;
      else
        rowValues[i] = 0x83 + playfield[row][i];
    }
    vrambuf_put(NTADR_A(8, 6 + row), rowValues, 10);
  }
}

uint8 isRowEmpty(uint8 row) {
  int i;
  for(i = 0; i < 10; i++) {
    if(playfield[row][i] != -1)
      return false;
  }
  
  return true;
}

//Used to check if a move is valid
uint8 isValidSpot(struct Tetrimino* tet) {
  uint8 block;
  for(block = 0; block < 4; block++) {
    //Row and col of this block
    int row = tet->row + Tetriminos[tet->id][tet->rotation][block][1];
    int col = tet->col + Tetriminos[tet->id][tet->rotation][block][0];
    
    //Ensure in bounds
    if(row >= 20)
      return false;
    
    
    //Blocks can't go up so not checking
    if(col >= 10 || col < 0)
      return false;
    
    //Ignore parts not on screen
    if(row < 0)
      continue;
    
    //Check to see if already a piece there
    if(playfield[row][col] != -1)
       return false;
  }
  
  //All the blocks passed the tests, its valid
  return true;
}

uint8 moveTetrimino(struct Tetrimino* tet, int8 colChange, int8 rotationChange) {
  struct Tetrimino futureTet;
  
  //Try applying both changes first
  if(colChange != 0 && rotationChange != 0) {
    futureTet = *tet;
    futureTet.col += colChange;
    futureTet.rotation = wrap(futureTet.rotation + rotationChange, 0, 3);
    if(isValidSpot(&futureTet)) {
      *tet = futureTet;
      return true;
    }
  }
  
  //Try just sliding
  if(colChange != 0) {
    futureTet = *tet;
    futureTet.col += colChange;
    if(isValidSpot(&futureTet)) {
      *tet = futureTet;
      return true;
    }
  }
  
  //Try just rotating
  if(rotationChange != 0) {
    futureTet = *tet;
    futureTet.rotation = wrap(futureTet.rotation + rotationChange, 0, 3);
    if(isValidSpot(&futureTet)) {
      *tet = futureTet;
      return true;
    }
  }
  
  //No changes made
  return false;
}

void spawnTetrimino(struct Tetrimino* tet, uint8 id, uint8 color) {
  tet->id = id;
  tet->rotation = 0;
  tet->color = color;
  tet->row = 0;
  tet->col = 5;
}

int getPoints(uint8 rowsCleared, uint8 level) {
  int score = 0;
  int pointsPerLevel;
  
  if(rowsCleared == 4)
    pointsPerLevel = 1200;
  else if(rowsCleared == 3)
    pointsPerLevel = 300;
  else if(rowsCleared == 2)
    pointsPerLevel = 100;
  else if(rowsCleared == 1)
    pointsPerLevel = 40;
  else
    return 0;
  
  level += 1;
  while(level-- > 0)
    score += pointsPerLevel;
  
  return score;
}

//Wrap, min and max inclusive
int8 wrap(int8 v, int8 min, int8 max) {
  //wrap
  while(v < min)
    v = (v - min) + (max + 1);
  while(v > max)
    v = (v - max) + (min - 1);
  return v;
}

//Pseudorandom generation as done in the actual tetris game
//Although my arbitrary start value is probably different
unsigned int random() { 
  uint8 nextBit = ((seed >> 1 ) & 1) ^ ((seed >> 9) & 1);
  seed = (nextBit << 15) | (seed >> 1);
  return seed;
}

//Selection sort that sorta highest to lowest
void selectionSort(int8 arr[], int8 n)
{
  int8 i, j, maxIndex, tmp;
 
  // One by one move boundary of unsorted subarray
  for (i = 0; i < n - 1; i++) {
 
    // Find the minimum element in unsorted array
    maxIndex = i;
    for (j = i + 1; j < n; j++)
      if (arr[j] > arr[maxIndex])
        maxIndex = j;
 
    //Swap
    tmp = arr[i];
    arr[i] = arr[maxIndex];
    arr[maxIndex] = tmp;
  }
}

//Music
byte next_music_byte() {
  return *musicPtr++;
}

void play_music() {
  static byte chs = 0;
  if (musicPtr) {
    // run out duration timer yet?
    while (cur_duration == 0) {
      // fetch next byte in score
      byte note = next_music_byte();
      // is this a note?
      if ((note & 0x80) == 0) {
        // pulse plays higher notes, triangle for lower if it's free
        if (note >= BASS_NOTE || (chs & 4)) {
          int period = NOTE_TABLE[note & 63];
          // see which pulse generator is free
          if (!(chs & 1)) {
            APU_PULSE_DECAY(0, period, DUTY_25, 2, 10);
            chs |= 1;
          } else if (!(chs & 2)) {
            APU_PULSE_DECAY(1, period, DUTY_25, 2, 10);
            APU_PULSE_SWEEP(1, 0, 0, 0);
            chs |= 2;
          }
        } else {
          int period = note_table_tri[note & 63];
          APU_TRIANGLE_LENGTH(period, 15);
          chs |= 4;
        }
      } else {
        // end of score marker
        if (note == 0xff)
          musicPtr = NULL;
        // set duration until next note
        cur_duration = note & 63;
        // reset channel used mask
        chs = 0;
      }
    }
    cur_duration--;
  }
}

void start_music(const byte* music) {
  musicPtr = music;
  cur_duration = 0;
}

void play_sound(const word vals[]) {
  APU_PULSE_DECAY(0, vals[0], vals[1], vals[2], vals[3]);
  APU_PULSE_SWEEP(0, vals[4], vals[5], vals[6]);
  APU_PULSE_DECAY(1, vals[7], vals[8], vals[9], vals[10]);
  APU_PULSE_SWEEP(1, vals[11], vals[12], vals[13]);
  APU_TRIANGLE_LENGTH(vals[14], vals[15]);
  APU_NOISE_DECAY(vals[16]|vals[19], vals[17], vals[18]);
}