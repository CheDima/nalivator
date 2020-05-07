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

static Sprite heartFull = { 0x1c, 0x3e, 0x7e, 0xfc, 0xfc,0x7e, 0x3e, 0x1c };
static Sprite heartEmpty = { 0x1c, 0x22, 0x42, 0x84, 0x84, 0x42, 0x22, 0x1c };
