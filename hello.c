
/*
A simple "hello world" example.
Set the screen background color and palette colors.
Then write a message to the nametable.
Finally, turn on the PPU to display video.
*/

#include "neslib.h"
#include "stdlib.h"
#include "vrambuf.h"
#include "string.h"
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

//Returns the wrapped value
int8 wrap(int8 v, int8 min, int8 max);
void selectionSort(int8 arr[], int8 n);

//Advances the random value and returns it
unsigned int random(void);

//Const and defines

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
int8 playfield[20][10];
uint8 framesPerDrop;

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
  char test[5];
  int frameCount = 0;
  int level = 0;
  uint8 row;
  uint8 col;
  
  //The falling tetris piece
  struct Tetrimino tet = {0 , 0, 0, 5, 0};
  struct Tetrimino futureTet;
  uint8 nextPieceId = 1;
  uint8 nextPieceColor = 1;
  
  itoa(sizeof(playfield[0]), test, 10);
  
  vrambuf_put(NTADR_A(1, 1), test, 5);
  
  //Initialize the playfield
  for(row = 0; row < 20; row++) {
    for(col = 0; col < 10; col++) {
      playfield[row][col] = -1; 
    }
  }
  
  //Set initial values
  setScore(0);
  setLevel(0);
  setLevelPal(0);
  setLevelSpeed(0);
  
  while(1) {
    uint8 padResult = pad_trigger(0);
    uint8 sprid = 0;
    int8 colChange = 0;
    int8 rotationChange = 0;
    int8 framesTillDrop = framesPerDrop - frameCount;
    
    //Handle the logic for input
    //Input that affects turning/moving/fast dropping happens BEFORE 
    //the actual dropping. This means that if the new position would result
    //in a lock when it drops, it will still perform the move
    if(pad_poll(0) & PAD_DOWN) {
      //Fast Fall
      framesTillDrop = 0;
    }
    else if(padResult & PAD_LEFT) {
      colChange = -1;
    }
    else if(padResult & PAD_RIGHT) {
      colChange = 1;
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
        lockTetrimino(&tet);
        drawPlayfield();
        //clearRows(&tet);
        spawnTetrimino(&tet, nextPieceId, nextPieceColor);
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
    
    oam_hide_rest(sprid);
    
    //Wait for next frame and update count
    frameCount++;
    vrambuf_flush();
  }
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
    //VRAMBUF_PUT(NTADR_A(originX + col, originY + row), 1, NT_UPD_HORZ);
    //VRAMBUF_ADD(0x83 + tet->color);
    //vrambuf_end();
  }
}

uint8 clearRows(struct Tetrimino* tet) {
  uint8 block;
  uint8 i;
  uint8 numRowsChecked = 0;
  uint8 rowsCleared = 0;
  uint8 skip;
  int8 rowChecked[4] = {-1, -1, -1, -1};
  //Because of block size only 4 rows can ever be cleared at once
  int8 rowsToClear[4] = {-1, -1, -1, -1};
  
  //Find the rows that need cleared
  for(block = 0; block < 4; block++) {
    //Row and col of this block
    int8 row = tet->row + Tetriminos[tet->id][tet->rotation][block][1];
    
    skip = 0;
    //Check to see if this row has been checked yet
    for(i = 0; i < numRowsChecked; i++) {
      if(rowChecked[i] == row) {
        skip = 1;
        break;
      }
    }
    
    if(skip)
      continue;
    
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
  //clearPlayfieldRows(rowsToClear, rowsCleared);
  
  //Redraw the updated playfield
  //if(rowsCleared)
  //  drawPlayfield();
  
  return rowsCleared;
}

void clearPlayfieldRows(int8 rowsToClear[], uint8 len) {
  int8 rowIndex;
  int8 curClearRow;
  int8 nextClearRow;
  int8 curEmptyRow;
  int row;
  
  if(len == 0)
    return;
  
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
      memcpy(playfield[curEmptyRow], playfield[row], sizeof(playfield[row]));
      curEmptyRow--; //Set the row just moved down as the empty row
    }
    
  }
  
  //There is no next cleared row, so loop and bring everything down 
  //until an empty row is found
  curClearRow = rowsToClear[len - 1];
  row = curClearRow - 1;
  while(row >= 0) {
    memcpy(playfield[curEmptyRow], playfield[row], sizeof(playfield[row]));
    
    //Finished clearing by copying an empty row
    if(isRowEmpty(row))
      break;
    
    curEmptyRow--; //Set the row just moved down as the empty row
    row--;
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
  static unsigned int value = 86271233; 
  uint8 nextBit = ((value >> 1 ) & 1) ^ ((value >> 9) & 1);
  value = (nextBit << 15) | (value >> 1);
  return value;
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
