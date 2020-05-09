typedef uint8_t Sprite[8];

static Sprite arrow =
{
  0b00001000,
  0b00011100,
  0b00111110,
  0b01111111,
  0b00011100,
  0b00011100, 
  0b00111110,
  0b00000000
};

static Sprite autoMode =
{
  0b01000000,
  0b10100000,
  0b11100010,
  0b10101110,
  0b00001010,
  0b00000010, 
  0b00001010,
  0b00000010
};


static Sprite manualMode =
{
  0b10100000,
  0b11100000,
  0b10100010,
  0b10101110,
  0b00001010,
  0b00000010, 
  0b00001010,
  0b00000010
};

static Sprite smile1 =
{
  0b00000000,
  0b01100011,
  0b01100011,
  0b00000000,
  0b00001000,
  0b01000001, 
  0b00100010,
  0b00011100
};

static Sprite smile2 =
{
  0b00000000,
  0b01100011,
  0b01100011,
  0b00000000,
  0b00001000,
  0b00000000, 
  0b00111110,
  0b00000000
};
static Sprite smile3 =
{
  0b00000000,
  0b01100011,
  0b01100011,
  0b00000000,
  0b00001000,
  0b00000000, 
  0b00011100,
  0b00100010
};
static Sprite invider1 =
{
  0b00011000,
  0b01111110,
  0b11111111,
  0b10011001,
  0b11011101,
  0b11111111, 
  0b11111111,
  0b01010101
};

static Sprite invider2 =
{
  0b00011000,
  0b01111110,
  0b11111111,
  0b10011001,
  0b10111011,
  0b11111111, 
  0b11111111,
  0b01010101
};

static Sprite bomb1 =
{
  0b00000000,
  0b00000000,
  0b00000000,
  0b00011000,
  0b00011000,
  0b00000000, 
  0b00000000,
  0b00000000
};

static Sprite bomb2 =
{
  0b00000000,
  0b00000000,
  0b00100100,
  0b00011000,
  0b00011000,
  0b00100100, 
  0b00000000,
  0b00000000
};
static Sprite bomb3 =
{
  0b10001001,
  0b01000010,
  0b00100100,
  0b00000001,
  0b10000000,
  0b00100100, 
  0b01000010,
  0b10010001
};

static Sprite glass1 =
{
  0b00000000,
  0b10000010,
  0b01000100,
  0b00101000,
  0b00010000,
  0b00010000, 
  0b00010000,
  0b01111100
};

static Sprite glass2 =
{
  0b00000000,
  0b10000010,
  0b01111100,
  0b00111000,
  0b00010000,
  0b00010000, 
  0b00010000,
  0b01111100
};

static Sprite glass3 =
{
  0b00000000,
  0b11111110,
  0b01111100,
  0b00111000,
  0b00010000,
  0b00010000, 
  0b00010000,
  0b01111100
};


static Sprite heartFull = { 0x1c, 0x3e, 0x7e, 0xfc, 0xfc,0x7e, 0x3e, 0x1c };
static Sprite heartEmpty = { 0x1c, 0x22, 0x42, 0x84, 0x84, 0x42, 0x22, 0x1c };
