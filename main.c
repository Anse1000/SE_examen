#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"
volatile int speed=2;
int speeds[4]={0,0xBEBC1E,0x17D783C,0x2FAF078};
volatile int led_state=0;

void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0: 32KHZ internal reference clock; 1: 4MHz IRC
}

void switches_init() {
    SIM->SCGC5 |= 0x800u;
    PORTC->PCR[3] |= 0xA0102u;
    PORTC->PCR[12] |= 0xA0102u;
    GPIOC->PDDR = 0x0u;
    EnableIRQ(PORTC_PORTD_IRQn);
}
void leds_init() {
    SIM->SCGC5 |= 0x3000u;
    PORTD->PCR[5] |= 0x100u;
    GPIOD->PDDR |= 0x20u;
    GPIOD->PSOR &= ~0x20u;
}
int check_left_switch() {
    return (int) (PORTC->ISFR & (1 << 12));
}
int check_right_switch() {
    return (int) (PORTC->ISFR & (1 << 3));
}
void timer_init() {
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; // Habilitar el reloj para el PIT
    PIT->MCR = 0x00;                  // Habilitar PIT
    PIT->CHANNEL[0].LDVAL = speeds[speed]; // Configurar para medio segundo (aproximadamente)
    PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK; // Habilitar interrupciones y temporizador
    EnableIRQ(PIT_IRQn);// Habilitar interrupción del PIT
}
void PIT_IRQHandler(){
    if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
        PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;
        led_state=!led_state;
        if(led_state){
            LED_GREEN_ON();
        }else{
            LED_GREEN_OFF();
        }
    }
}

void PORTC_PORTD_IRQHandler(){
    if(check_left_switch()){
        if(speed>0)speed-=1;
        PORTC->ISFR = (1 << 12);
    }else if(check_right_switch()){
        if(speed<3)speed+=1;
        PORTC->ISFR = (1 << 3);
    }
    if(speed==0){
        LED_GREEN_OFF();
        DisableIRQ(PIT_IRQn);
    }else{
        EnableIRQ(PIT_IRQn);
        PIT->CHANNEL[0].LDVAL = speeds[speed];
    }

}
int main(void){

  /* Init board hardware. */
  BOARD_InitPins();
  BOARD_BootClockRUN();
  BOARD_InitDebugConsole();
  switches_init();
  timer_init();
  leds_init();
  for(;;);
}
