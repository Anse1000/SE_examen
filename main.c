
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"

volatile int leftdoorstate=1;
volatile int rightdoorstate=1;
void switches_init() {
    SIM->SCGC5 |= 0x800u;
    PORTC->PCR[3] |= 0xA0102u;
    PORTC->PCR[12] |= 0xA0102u;
    GPIOC->PDDR = 0x0u;
    NVIC_EnableIRQ(PORTC_PORTD_IRQn);
}
void leds_init() {
    SIM->SCGC5 |= 0x3000u;
    PORTD->PCR[5] |= 0x100u;
    GPIOD->PDDR |= 0x20u;
    PORTE->PCR[29] = 0x100u;
    GPIOE->PDDR |= 0x20000000u;
    GPIOE->PSOR |= 0x20000000u;
    GPIOD->PSOR |= 0x20u;
}
void clear_leds() {
    GPIOE->PSOR |= 0x20000000u;
    GPIOD->PSOR |= 0x20u;
}

int check_left_switch() {
    return (int) (PORTC->ISFR & (1 << 12));
}

int check_right_switch() {
    return (int) (PORTC->ISFR & (1 << 3));
}
void PORTC_PORTD_IRQHandler(){
    if(check_left_switch()){
        leftdoorstate=!leftdoorstate;
        PORTC->ISFR = (1 << 12);
    }else if(check_right_switch()){
        rightdoorstate=!rightdoorstate;
        PORTC->ISFR = (1 << 3);
    }
    clear_leds();
    if(leftdoorstate|rightdoorstate){
        LED_GREEN_ON();
    }else if(!leftdoorstate & !rightdoorstate){
        LED_RED_ON();
    }
}

int main(void){

  /* Init board hardware. */
  BOARD_InitPins();
  BOARD_BootClockRUN();
  BOARD_InitDebugConsole();
  switches_init();
  leds_init();
  EnableIRQ(PORTC_PORTD_IRQn);
  LED_GREEN_ON(); //para estado inicial
  for(;;);
}
