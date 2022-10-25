
template<typename Data>
class Vector {
   size_t d_size; // Stores no. of actually stored objects
   size_t d_capacity; // Stores allocated capacity
   Data *d_data; // Stores data
   public:
     Vector() : d_size(0), d_capacity(0), d_data(0) {}; // Default constructor
     Vector(Vector const &other) : d_size(other.d_size), d_capacity(other.d_capacity), d_data(0) { d_data = (Data *)malloc(d_capacity*sizeof(Data)); memcpy(d_data, other.d_data, d_size*sizeof(Data)); }; // Copy constuctor
     ~Vector() { free(d_data); }; // Destructor
     Vector &operator=(Vector const &other) { free(d_data); d_size = other.d_size; d_capacity = other.d_capacity; d_data = (Data *)malloc(d_capacity*sizeof(Data)); memcpy(d_data, other.d_data, d_size*sizeof(Data)); return *this; }; // Needed for memory management
     void push_back(Data const &x) { if (d_capacity == d_size) resize(); d_data[d_size++] = x; }; // Adds new value. If needed, allocates more space
     size_t size() const { return d_size; }; // Size getter
     Data const &operator[](size_t idx) const { return d_data[idx]; }; // Const getter
     Data &operator[](size_t idx) { return d_data[idx]; }; // Changeable getter
   private:
     void resize() { d_capacity = d_capacity ? d_capacity*2 : 1; Data *newdata = (Data *)malloc(d_capacity*sizeof(Data)); memcpy(newdata, d_data, d_size * sizeof(Data)); free(d_data); d_data = newdata; };// Allocates double the old space
};

void serviceMode() {

  if (!digitalRead(BTN_PIN)) {
      //ServoDriverSmooth *srvs[4];
  Vector<ServoDriverSmooth> srvs;
  srvs.push_back(srvPlatform);
  srvs.push_back(srvPlatform);
  srvs.push_back(srvCenter);
  srvs.push_back(srvTop);
    byte curSrv = 0;
    int servoPos = srvs[curSrv].getCurrentDeg();
    systemState = SERVICE;
    PRINTS("Entered service mode");
    while (!digitalRead(BTN_PIN));  // ждём отпускания
    delay(200);
    long pumpTime = 0;
    timerMinim timer100(100);
    //disp.displayInt(0);
    servosLock();
    bool flag;
    for (;;) {
      if (curSrv==1) srvBottom.tick(); else srvs[curSrv].tick(); // fuck inheritance, right?
      enc.tick();

      if (timer100.isReady()) {   // период 100 мс
        // работа помпы со счётчиком
        // if (!digitalRead(ENC_SW)) {
        //   if (flag) pumpTime += 100;
        //   else pumpTime = 0;
        //   //disp.displayInt(pumpTime);
        //   pumpON();
        //   flag = true;
        // } else {
        //   pumpOFF();
        //   flag = false;
        // }

        // зажигаем светодиоды от кнопок
        for (byte i = 0; i < NUM_SHOTS; i++) {
          if (!digitalRead(SW_pins[i])) {
            strip.leds[i] =  mGreen;
          } else {
            strip.leds[i] =  mBlack;
          }
          strip.show();
        }
      }

      if (enc.isTurn()) {
        if (enc.isLeft()) {
          servoPos ++;
        }
        if (enc.isRight()) {
          servoPos --;
        }

        if (curSrv==1) {srvBottom.setTargetDeg(servoPos); srvBottom.setAutoDetach(false);} 
        else {srvs[curSrv].setTargetDeg(servoPos); srvs[curSrv].setAutoDetach(false);}
        PRINT("srv ", curSrv);
        PRINT("pos \n", servoPos);
        //tftRedraw();
      }
      if (enc.isClick()) {
        curSrv = curSrv==3?0:curSrv+1;
        servoPos = curSrv==1 ? srvBottom.getCurrentDeg() : srvs[curSrv].getCurrentDeg();
        PRINT("srv ", curSrv);
        PRINT("pos \n", servoPos);
        //tftRedraw();
       
      }

      if (btn.holded()) {
        break;
      }
    }
  }
}
