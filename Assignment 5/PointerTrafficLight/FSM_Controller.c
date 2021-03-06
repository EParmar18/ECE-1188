// PointerTrafficLightmain.c
// Runs on MSP432
// Use a pointer implementation of a Moore finite state machine to operate
// a traffic light.
// Daniel Valvano
// May 10, 2017

/* This example accompanies the book
   "Embedded Systems: Introduction to Robotics,
   Jonathan W. Valvano, ISBN: 9781074544300, copyright (c) 2019
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2019, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// east facing red light connected to P4.5
// east facing yellow light connected to P4.4
// east facing green light connected to P4.3
// north facing red light connected to P4.2
// north facing yellow light connected to P4.1
// north facing green light connected to P4.0
// north facing car detector connected to P5.1 (1=car present)
// east facing car detector connected to P5.0 (1=car present)

#include <stdint.h>
#include "msp.h"
#include "..\inc\Clock.h"
#include "..\inc\TExaS.h"

struct State {
  uint8_t Out;             // 6-bit output
  uint32_t Time;           // 10 ms
  const struct State *Next[4];};// depends on 2-bit input
typedef const struct State State_t;
#define s1   &FSM[0]
#define s2   &FSM[1]
#define s3   &FSM[2]
#define s4   &FSM[3]
#define s5   &FSM[4]
State_t FSM[5]={
 {0x12,300,{s1,s3,s2}},
 {0x02, 500,{s2,s4,s5}},
 {0x10,300,{s3,s2,s4}},
 {0x14, 500,{s1}},
 {0x00,300,{s3}}};
// SENSOR is set to P5IN and LIGHT is set to P4OUT, making the code more readable.
#define SENSOR  (*((volatile uint8_t *)0x40004C40))
#define LIGHT   (*((volatile uint8_t *)0x40004C23))

int main(void){
  State_t *Pt;  // state pointer
  uint8_t Input;
  Clock_Init48MHz();              // initialize clock to 48MHz
  TExaS_Init(LOGICANALYZER_P4);
  P1->SEL0 &= ~0x13;
  P1->SEL1 &= ~0x13;   // 1) configure P1.4  P1.1 P1.0 as GPIO
  P1->DIR &= ~0x12;    // 2) make P1.4 and P1.1 in
  P1->DIR |= 0x01;     // 2) make P1.0 out
  P1->REN |= 0x12;     // 3) enable pull resistors on P1.4 and P1.1
  P1->OUT |= 0x12;     //    P1.4 and P1.1 are pull-up

  Pt = s1;                    // initial state: Green north; Red east
  while(1){
                               // set lights to current state's Out value
    P1->OUT = (P1->OUT&0xFE)|(Pt->Out);
    Clock_Delay1ms(Pt->Time);  // wait 1 ms * current state's Time value
    Input = (P1->IN&0x12);     // get new input from car detectors
    Pt = Pt->Next[Input];      // transition to next state
  }
}

// Volume 2, program 3.3
//#define BSP_InPort           PIN
//#define BSP_InPort_DIR       P5DIR
//#define BSP_OutPort          P4OUT
//#define BSP_OutPort_DIR      P4DIR
//#define BSP_In_M             0x00000003  // bit mask for pins 1,0
//#define BSP_In_Shift         0x00000000  // shift value for Input pins
//#define BSP_Out_M            0x0000003F  // bit mask for pins 5-0
//#define BSP_Out_Shift        0x00000000  // shift value for Output pins
//
//int main2(void){ State_t *pt;      // state pointer
//  uint32_t input;
//  Clock_Init48MHz();            // initialize clock to 48MHz
//  TExaS_Init(LOGICANALYZER);
//  //BSP_InPort_DIR &= ~BSP_In_M;  // make InPort pins inputs
//  P1->SEL0 &= ~0x13;
//  P1->SEL1 &= ~0x13;   // 1) configure P1.4  P1.1 P1.0 as GPIO
//  P1->DIR &= ~0x12;    // 2) make P1.4 and P1.1 in
//  P1->DIR |= 0x01;     // 2) make P1.0 out
//  P1->REN |= 0x12;     // 3) enable pull resistors on P1.4 and P1.1
//  P1->OUT |= 0x12;     //    P1.4 and P1.1 are pull-up
//  //BSP_OutPort_DIR |= BSP_Out_M; // make OutPort pins out
//  pt = goN;
//  while(1){
//    BSP_OutPort = (BSP_OutPort&(~BSP_Out_M))|((pt->Out)>>BSP_Out_Shift);
//    TExaS_Set(BSP_OutPort);
//    Clock_Delay1ms(pt->Time);
//    input = (BSP_InPort&BSP_In_M)>>BSP_In_Shift; //00,01,10,11
//    pt = pt->Next[input];
//  }
//}
