#ifndef __MAIN_H
#define __MAIN_H

#include "stm8s.h"
#include "stm8s_flash.h"

#define LED7_PORT		GPIOD
#define LED7_SHIFT		GPIO_PIN_2
#define LED7_LOAD		GPIO_PIN_3
#define LED7_DIO		GPIO_PIN_4

#define CONTROL_PORT	GPIOC
#define BUTTON1			GPIO_PIN_3		// RESET/SETTING HOLD/SELECT
#define BUTTON2			GPIO_PIN_4		// TRIGGER/CHANGE
#define RELAY			GPIO_PIN_5
#define LED1			GPIO_PIN_6
#define LED2			GPIO_PIN_7

#define TRIGGER_PORT	GPIOA
#define TRIGGER			GPIO_PIN_3

#define TIM4_PERIOD		124
#define DEBOUNCE		200
#define LONG_PRESS		3000
#define SHORT_PRESS		150
#define TIME_OUT		5000

enum { false, true };
enum {STATE_ON, STATE_OFF};

uint32_t tick1 = 0;
uint32_t tick2 = 0;
uint32_t b1tick = 0;
uint32_t b2tick = 0;
uint32_t timeout = 0;
uint32_t counter = 0;
__IO uint32_t delay = 0;

uint8_t state = 0;
uint8_t phase = 0;
uint8_t blinkCount = 0;
uint8_t blink = 0;
uint8_t tick100 = 0;
int selected = -1;

uint8_t button1State = true;
uint8_t button2State = true;
uint8_t changeState = false;

int time1 = 100;
int time2 = 200;
int countdown = 0;
uint8_t phase1 = STATE_OFF;
uint8_t phase2 = STATE_OFF;
uint8_t phase3 = STATE_OFF;

struct numArray { 
    uint8_t chars[4]; 
}; 

uint8_t numbers[] = 
    {// 0    1     2     3     4     5     6     7     8     9     o     n     f     transparent
       0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xA3, 0xAB, 0x8E, 0xff
    };	
	
/* Private function prototypes -----------------------------------------------*/

void digitalWrite(GPIO_TypeDef * PORT, GPIO_Pin_TypeDef Pin, uint8_t value);
void Delay(__IO uint32_t time);
void resetTimer(void);
void load(void);
void save(void);
void showLed(struct numArray numarr);
void reset(void);
void trigger(void);
void buttonProcess(void);
void setLeds(uint8_t led1, uint8_t led2);
void ledProcess(void);
void setup(void);
void timer4Handler(void);
void tick100ms(void);
uint32_t millis(void);
struct numArray num2Array(int number);
struct numArray state2Array(uint8_t phase, uint8_t _state);


#endif
