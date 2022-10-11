#define SRV_PLATFORM_PIN 0
#define SRV_BOTTOM_R_PIN 1
#define SRV_BOTTOM_L_PIN 2
#define SRV_CENTER_PIN 3
#define SRV_TOP_PIN 4
#define SRV_PUMP_PIN 5

#define SERVO_ACCELERATION 0.4
#define SERVO_SPEED 150
#define BOTTOM_L_SERVO_CORRECTION 3

enum Servos {PLATFORM, BOTTOM, CENTER, TOP};
const byte UPPER_POSITION = 50;
const byte parkingPos[4] = {60, 60, 60, 60}; 
// 3  2
// 4  1
const byte shotPos[NUM_SHOTS][4] = {{180, 10, 10, 90}, {158, 10, 120, 90}, {120, 0, 0, 90}, {120, 10, 70, 90}};

ServoDriverSmooth srvPlatform(0x40, 270);
ServoDriverSmooth srvBottomL(0x40, 270);
ServoDriverSmooth srvBottomR(0x40, 270);
ServoDriverSmooth srvCenter(0x40, 270);
ServoDriverSmooth srvTop(0x40, 270);
ServoDriverSmooth srvPump(0x40);

class ServoWrapper {
  ServoDriverSmooth left;
  ServoDriverSmooth right;
  public:
    ServoWrapper(ServoDriverSmooth& r, ServoDriverSmooth& l);
    void attach(byte pinR, byte pinL, byte parkPosition);
    void setSpeed(int speed);
    void setAccel(double accel);
    void setTargetDeg(int angle);
    bool tick();
};
ServoWrapper::ServoWrapper(ServoDriverSmooth& l, ServoDriverSmooth& r) {
  left = l;
  right = r;
}
void ServoWrapper::attach(byte pinR, byte pinL, byte parkPosition) {
  right.attach(pinR, parkPosition);
  left.attach(pinL, parkPosition + BOTTOM_L_SERVO_CORRECTION);
  left.setDirection(true); // reversed
}
void ServoWrapper::setSpeed(int speed) {
  right.setSpeed(speed);
  left.setSpeed(speed);
}
void ServoWrapper::setAccel(double accel){
  right.setAccel(accel);
  left.setAccel(accel);
}
void ServoWrapper::setTargetDeg(int angle) {
  right.setTargetDeg(angle);
  left.setTargetDeg(angle + BOTTOM_L_SERVO_CORRECTION);
}
bool ServoWrapper::tick() {
  return right.tick() && left.tick();
}


ServoWrapper srvBottom(srvBottomR, srvBottomL);

void servoSetup() {
  srvPlatform.attach(SRV_PLATFORM_PIN, parkingPos[Servos::PLATFORM]);
  srvBottom.attach(SRV_BOTTOM_R_PIN, SRV_BOTTOM_L_PIN, parkingPos[Servos::BOTTOM]);
  srvCenter.attach(SRV_CENTER_PIN, parkingPos[Servos::CENTER]);
  srvTop.attach(SRV_TOP_PIN, parkingPos[Servos::TOP]);
  srvPump.attach(SRV_PUMP_PIN, 500, 2500);

  srvTop.setDirection(true);

  srvPlatform.setSpeed(SERVO_SPEED);
  srvPlatform.setAccel(SERVO_ACCELERATION);
  srvBottom.setSpeed(SERVO_SPEED);
  srvBottom.setAccel(SERVO_ACCELERATION);
  srvCenter.setSpeed(SERVO_SPEED);
  srvCenter.setAccel(SERVO_ACCELERATION);
  srvTop.setSpeed(SERVO_SPEED);
  srvTop.setAccel(SERVO_ACCELERATION);
  
  PRINTS("Servo initialized");
}