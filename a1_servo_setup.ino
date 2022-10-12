#define SRV_PLATFORM_PIN 0
#define SRV_BOTTOM_R_PIN 1
#define SRV_BOTTOM_L_PIN 2
#define SRV_CENTER_PIN 3
#define SRV_TOP_PIN 4
#define SRV_PUMP_PIN 5

#define SERVO_ACCELERATION 0.4
#define SERVO_SPEED 150
#define BOTTOM_L_SERVO_CORRECTION 73

enum Servos {PLATFORM, BOTTOM, CENTER, TOP};
const byte UPPER_POSITION = 50;
const byte parkingPos[4] = {60, 130, 190, 180}; 
// 3  2
// 4  1
const byte shotPos[NUM_SHOTS][4] = {{180, 10, 10, 90}, {158, 10, 120, 90}, {120, 0, 0, 90}, {120, 10, 70, 90}};

ServoDriverSmooth srvPlatform(0x40, 270);
//ServoDriverSmooth srvBottomL(0x40, 270);
//ServoDriverSmooth srvBottomR(0x40, 270);
ServoDriverSmooth srvCenter(0x40, 270);
ServoDriverSmooth srvTop(0x40, 300);
ServoDriverSmooth srvPump(0x40);

class ServoWrapper  {
  ServoDriverSmooth left = ServoDriverSmooth(0x40, 270);
  ServoDriverSmooth right = ServoDriverSmooth(0x40, 270);
  public:
    ServoWrapper();
    void attach(byte pinR, byte pinL, byte parkPosition) const ;
    void setSpeed(int speed) const ;
    void setAccel(double accel) const ;
    void setTargetDeg(int angle) const ;
    int getCurrentDeg() const ;
    bool tick() const;
    void setAutoDetach(bool detach); 
};
ServoWrapper::ServoWrapper() {
}
void ServoWrapper::attach(byte pinR, byte pinL, byte parkPosition) const {
  right.attach(pinR, parkPosition);
  right._servo.setPWMFreq(50);
  left.attach(pinL, parkPosition + BOTTOM_L_SERVO_CORRECTION);
  left._servo.setPWMFreq(50);
  right.setDirection(false);
  left.setDirection(true); // reversed
  
  left.tick();

}
void ServoWrapper::setSpeed(int speed) const {
  right.setSpeed(speed);
  left.setSpeed(speed);
}
void ServoWrapper::setAccel(double accel) const {
  right.setAccel(accel);
  left.setAccel(accel);
}
void ServoWrapper::setTargetDeg(int angle) const {
  right.setTargetDeg(angle);
  left.setTargetDeg(angle + BOTTOM_L_SERVO_CORRECTION);
}
bool ServoWrapper::tick() const {
  bool rt = right.tick();
  bool lt = left.tick();
  return rt && lt;
}
int ServoWrapper::getCurrentDeg() const {
  return right.getCurrentDeg();
}
void ServoWrapper::setAutoDetach(bool isReleased) {
  left.setAutoDetach(isReleased);
  right.setAutoDetach(isReleased);
}

ServoWrapper srvBottom;

void servoSetup() {
  srvPlatform.attach(SRV_PLATFORM_PIN, parkingPos[Servos::PLATFORM]);
  srvBottom.attach(SRV_BOTTOM_R_PIN, SRV_BOTTOM_L_PIN, parkingPos[Servos::BOTTOM]);
  srvCenter.attach(SRV_CENTER_PIN, parkingPos[Servos::CENTER]);
  srvTop.attach(SRV_TOP_PIN, parkingPos[Servos::TOP]);
  srvPump.attach(SRV_PUMP_PIN, 500, 2500);
  srvPlatform._servo.setPWMFreq(50);
  srvCenter._servo.setPWMFreq(50);
  srvTop._servo.setPWMFreq(50);
  srvPump._servo.setPWMFreq(50);

  srvTop.setDirection(true); srvTop.tick();

  srvPlatform.setSpeed(SERVO_SPEED);
  srvPlatform.setAccel(SERVO_ACCELERATION);
  srvBottom.setSpeed(SERVO_SPEED);
  srvBottom.setAccel(SERVO_ACCELERATION);
  srvCenter.setSpeed(SERVO_SPEED);
  srvCenter.setAccel(SERVO_ACCELERATION);
  srvTop.setSpeed(SERVO_SPEED);
  srvTop.setAccel(SERVO_ACCELERATION);
  srvPlatform.tick();
  srvBottom.tick();
  srvCenter.tick();
  srvTop.tick();

  PRINTS("Servo initialized");
}