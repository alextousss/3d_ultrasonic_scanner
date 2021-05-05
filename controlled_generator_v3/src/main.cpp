#include <Arduino.h>
#include "PinInfo.h"

int latchPin = 1;
int clockPin = 0;
int angle_x=0;
int angle_y=0;
int active_emitter = 0;


void prnt8Bits(byte b) {
    for(int i = 7; i >= 0; i--)
        Serial.print(bitRead(b,i));
}
void prntBits(uint32_t b) {
    for(int i = 31; i >= 0; i--)
        Serial.print(bitRead(b,i));
}



uint8_t null_data[]= {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
};


int portPerPin[] = {
    6,6,9,9,9,9,7,7,7,7,7,7,7,7,6,6,6,6,6,6,6,6,6,6,6,6,6,6,8,9,8,8,7,9,8,8,8,8,8,8
};

uint32_t pinmaskPerPin[] = {
    0x8, 0x4, 0x10, 0x20, 0x40, 0x100, 0x400, 0x20000, 0x10000, 0x800, 0x1, 0x4, 0x2, 0x8, 0x40000, 0x80000, 0x800000, 0x400000, 0x20000, 0x10000, 0x4000000, 0x8000000, 0x1000000, 0x2000000, 0x1000, 0x2000, 0x40000000, 0x80000000, 0x40000, 0x80000000, 0x800000, 0x400000, 0x1000, 0x80, 0x8000, 0x4000, 0x2000, 0x1000, 0x20000, 0x10000
};




void generatePort(int len, int dataPins[], uint8_t data[], uint32_t out[][3], int *outLen) {
    unsigned int i = 0;
    // latchPin: low
    out[i][0] = 0; 
    out[i][1] = 0; 
    out[i++][2] = 0; 
    for(unsigned int j = 0; j < 8; j++) {
        out[i+j*3][0] = out[i+j*3-1][0];
        out[i+j*3][1] = out[i+j*3-1][1];
        out[i+j*3][2] = out[i+j*3-1][2];
        for(unsigned int k = 0; k < len; k++) {
            bool bit = !!(data[k] & (1 << j));
            int place = 0;
            int port = portPerPin[dataPins[k]];
            //           Serial.print("time to PinInfo="); Serial.println(micros()-start_ts_2);
            unsigned long start_ts_1 = micros();
            if(port == 6) place = 0;
            if(port == 7) place = 1;
            if(port == 9) place = 2;
            uint32_t pinmask = pinmaskPerPin[dataPins[k]];
            //prntBits(pinmask);
            if (bit) {
                out[i+j*3][place] = out[i+j*3][place] | pinmask;
            } else {
                out[i+j*3][place] = out[i+j*3][place] & ~pinmask;
            }
            //            Serial.print("time to write="); Serial.println(micros()-start_ts_1);
            // Serial.print("time in looop="); Serial.println(micros()-start_ts_0);
            //out[i+j*3][0] &= ~digitalPinToBitMask(clockPin); // wtf was that anyway?
        }
        out[i+j*3+1][0] = out[i+j*3][0] | CORE_PIN0_BITMASK;
        out[i+j*3+2][0] = out[i+j*3][0] & ~CORE_PIN0_BITMASK;
        out[i+j*3+1][1] = out[i+j*3][1]; // Ofc we're keeping the data! Only the clock bit should change
        out[i+j*3+2][1] = out[i+j*3][1];
        out[i+j*3+1][2] = out[i+j*3][2];
        out[i+j*3+2][2] = out[i+j*3][2];
    }
    // three values for each iteration of j (1 data, 1 clock low, 1 clock high) and -1 bcoz it works better this way
    i = i+8*3-1;
    // latchPin: high
    out[i+1][0] = (out[i][0] | CORE_PIN1_BITMASK); // On mets la latch à zéro
    // +2 car +1 pour la ligne d'avant et +1 pour car un tableau de taille n a un indice de fin n-1
    *outLen = i+2;
}


void setup () {
    Serial.begin(9600);
    for(unsigned int i = 0; i < 28; i++) {
        pinMode(i, OUTPUT);
    }
}


#define BUF_SIZE (29952/5) // (26*60)
//#define DEBUG
void optimiser(uint32_t data[][3], int len) {
    //while(Serial.available() <= 0){ 
    for(int j = 0; j < 1; j++) 
        for(unsigned int i = 0; i < len; i++) {
#ifdef DEBUG
            delay(1);
            prntBits(data[i][0]); Serial.print("\t");
            prntBits(data[i][1]); Serial.print("\t");
            prntBits(data[i][2]); Serial.print("\n");
            //    Serial.println(data[i], BIN);
#endif
            delayNanoseconds(1);
            GPIO6_DR = data[i][0];
            GPIO7_DR = data[i][1];
            GPIO9_DR = data[i][2];
        }

    //   delay(50);

    //    }
}

int portOfEmitter[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 10, 11, 
    12, 13, 14, 15, 16, 17, 18, 19, 8, 9,
    20, 21, 22, 23, 36, 37, 38, 39, 34, 35,
    24, 25, 26, 27, 28, 29, 30, 31, 32, 33,    
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,    
    56, 57, 58, 59, 52, 53, 54, 55, 50, 51,    
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69,    
    76, 77, 78, 79, 72, 73, 74, 75, 70, 71,    
};

typedef enum {OFF, MINUS, MINUS_TO_PLUS, PLUS, PLUS_TO_MINUS} state_t;

typedef struct {
    unsigned int id;
    unsigned int start_inc; // increment at which the wave starts
    unsigned int waiting_for; // number of increments during on/off switch delay
} Emitter;

int getPort(Emitter em) {
    return portOfEmitter[em.id]; 
}

uint8_t getPinMask(int pin) {
    uint8_t pin_mask = B00000000;
    if(pin%4 == 0)
        pin_mask = B10000001;
    if(pin%4 == 1)
        pin_mask = B01000010;
    if(pin%4 == 2)
        pin_mask = B00100100;
    if(pin%4 == 3)
        pin_mask = B00011000;
    return pin_mask;
}





uint32_t data[BUF_SIZE][3];

void loop() {
    if(Serial.available() > 0) {
        Serial.println("portPerPin = {");
        //Serial.println(Serial.read());
        int read= Serial.read();
        if(read == 104) { // "h"
            for(unsigned int i = 0; i < 40; i++) {
                int port = PinInfo(i).gpioInfo.gpioPortNr;
                Serial.print(port); Serial.print(",");
            }
            Serial.println("}");

            Serial.println("pinmaskPerPin = {");
            for(unsigned int i = 0; i < 40; i++) {
                uint32_t pinmask = (1<<PinInfo(i).gpioInfo.gpioBitNr);
                Serial.print("0x"); Serial.print(pinmask, HEX); Serial.print(", ");
            }
            Serial.println("}");
        }

        bool active = read == 97; // If it's a "a"!
        if(read == 114) {
            angle_x = -21;
        }
        if(read == 116) {
            angle_y = -21;
        }
        if(read == 121) {
            angle_y = 0;
        }

        if(read == 122) {
            angle_x++;
        }
        if(read == 115) {
            angle_x--;
        }

        if(read == 101) {
            angle_y++;
        }
        if(read == 100) {
            angle_y--;
        }
        if(read == 112) {
            active_emitter++;
        }
        if(read == 111) {
            active_emitter--;
        }


        // angle_x++;
        if(angle_x == 22) {
            angle_x = -21;
        }
        if(angle_y == 22) {
            angle_y = -21;
        }
        if(active) {
            //delay(100); 
            //Serial.println("----------------------------");
            for(int i = 0; i < BUF_SIZE; i++) {
                data[i][0] = 0;
                data[i][1] = 0;
                data[i][2] = 0;
            }
            Emitter emitters[80];
            for(unsigned int i = 0; i < 80; i++) {
                unsigned int start_inc = (i%10)*angle_x + (i/10)*angle_y; 
                emitters[i] = {
                    .id = i,
                    .start_inc = start_inc,
                    .waiting_for=0,
                };
            }

            unsigned int index = 0; 
            unsigned int period=2;

            //Serial.print("Register ID: "); Serial.println(register_id);
            uint8_t active_data[] = {
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000,
                B00000000
            };   //Serial.print("Pin Mask: "); prntBits(pin_mask); Serial.println();
#define TOTAL_LENGTH (BUF_SIZE/26-18)

            unsigned long start_ts_0 = millis();
            for(unsigned int i = 0; i < TOTAL_LENGTH; i++) {
                for(int j = 0; j < 80; j++) {
                    //if(j!= active_emitter) continue;
                    Emitter *em = &emitters[j];
                    unsigned int register_id = getPort(*em)/4;
                    uint8_t pin_mask = getPinMask(getPort(*em));
                    int phi = (i-em->start_inc)%42;
                    //     Serial.print(phi); Serial.print("\t");
                    if((phi < 3) || (phi > 21 && phi < 21+3)) {
                        active_data[register_id] = (active_data[register_id] & ~pin_mask) | (B00000000 & pin_mask);
                    } else if(phi < 21) {
                        active_data[register_id] = (active_data[register_id] & ~pin_mask) | (B11110000 & pin_mask);
                    } else if(phi > 21) {
                        active_data[register_id] = (active_data[register_id] & ~pin_mask) | (B00001111 & pin_mask);
                    }
                }
                //Serial.println();
                int outlen = -1;
                int dataPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
                //int dataPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
                if(i == TOTAL_LENGTH-1) {
                    generatePort(20, dataPins, null_data, data + index, &outlen); 
                } else {
                    generatePort(20, dataPins, active ? active_data: null_data, data + index, &outlen); 
                }
                //Serial.print(index); 
                //for(unsigned int o = 0; o < 20; o++) {
                //Serial.print("\t"); prnt8Bits(active_data[o]);
                //} 
                //Serial.println();
                index += outlen;

            }
            Serial.print("total time="); Serial.println(millis()-start_ts_0);
            Serial.print("active_emitter:"); Serial.print(active_emitter); Serial.print("angle_x:"); Serial.print(angle_x); Serial.print("angle_y:"); Serial.println(angle_y);
            //    digitalWriteFast(LED_BUILTIN, HIGH);
            optimiser(data, index); //{
            //   digitalWriteFast(LED_BUILTIN, LOW);
        }
        }
    }
