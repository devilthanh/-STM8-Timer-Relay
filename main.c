/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private functions ---------------------------------------------------------*/

void digitalWrite(GPIO_TypeDef * PORT, GPIO_Pin_TypeDef PIN, uint8_t value){
	if(value == 0){
		GPIO_WriteLow(PORT, PIN);
	}else{
		GPIO_WriteHigh(PORT, PIN);
	}
}

void Delay(__IO uint32_t time){
	delay = time;
	while(delay != 0);
}

void timer4Handler(void){
	counter++;
	tick100++;
	if(delay != 0) delay--;
	if(tick100 == 100) tick100ms();
}

void tick100ms(void){
	
	blinkCount++;
	if(blinkCount == 3){
		blinkCount = 0;
		blink = blink == 0x00? 0xff : 0x00;
	}
	
	if(countdown > 0){
		if(phase == 0){
			countdown = countdown - 1;
			if(countdown <= 0){
				countdown = time2;
				phase++;
				setLeds(STATE_OFF, STATE_ON);
				digitalWrite(CONTROL_PORT, RELAY, phase2);
			}
		}else if(phase == 1){
			countdown = countdown - 1;
			if(countdown <= 0){
				countdown = 0;
				phase++;
				setLeds(STATE_ON, STATE_ON);
				digitalWrite(CONTROL_PORT, RELAY, phase3);
			}
		}
	}
	tick100 = 0;
}

uint32_t millis(void){
	return counter;
}

void resetTimer(void){
	tick100 = 0;
}

void load(void){
	
	uint8_t flag = false;
	
	uint8_t *EEAddress = (uint8_t *) (0x4000);
	time1 = *(EEAddress + 0) * 1000 + *(EEAddress + 1) * 100 + *(EEAddress + 2) * 10 + *(EEAddress + 3);
	time2 = *(EEAddress + 4) * 1000 + *(EEAddress + 5) * 100 + *(EEAddress + 6) * 10 + *(EEAddress + 7);
	phase1 = *(EEAddress + 8);
	phase2 = *(EEAddress + 9);
	phase3 = *(EEAddress + 10);
	
	if(time1 < 0 || time1 > 9999){
		time1 = 50;
		flag = true;
	}
	
	if(time2 < 0 || time2 > 9999){
		time2 = 50;
		flag = true;
	}
	
	if(phase1 != STATE_OFF && phase1 != STATE_ON){
		phase1 = STATE_OFF;
		flag = true;
	}
	
	if(phase2 != STATE_OFF && phase2 != STATE_ON){
		phase2 = STATE_ON;
		flag = true;
	}
	
	if(phase3 != STATE_OFF && phase3 != STATE_ON){
		phase3 = STATE_OFF;
		flag = true;
	}
	
	if(flag == true) save();
}

void save(void){
	
	FLASH_Unlock(FLASH_MEMTYPE_DATA);
        
	int temp = time1;
	
	(*(uint8_t *) (0x4000 + 0)) = temp/1000;
	temp = temp % 1000;
	(*(uint8_t *) (0x4000 + 1)) = temp/100;
	temp = temp % 100;
	(*(uint8_t *) (0x4000 + 2)) = temp/10;
	(*(uint8_t *) (0x4000 + 3)) = temp%10;
	
	temp = time2;
	
	(*(uint8_t *) (0x4000 + 4)) = temp/1000;
	temp = temp % 1000;
	(*(uint8_t *) (0x4000 + 5)) = temp/100;
	temp = temp % 100;
	(*(uint8_t *) (0x4000 + 6)) = temp/10;
	(*(uint8_t *) (0x4000 + 7)) = temp%10;
	
	(*(uint8_t *) (0x4000 + 8)) = phase1;
	(*(uint8_t *) (0x4000 + 9)) = phase2;
	(*(uint8_t *) (0x4000 + 10)) = phase3;
	
	FLASH_Lock(FLASH_MEMTYPE_DATA);
}

void reset(void){
	state = 0;
	phase = 0;
	countdown = 0;
	setLeds(STATE_OFF, STATE_OFF);
	digitalWrite(CONTROL_PORT, RELAY, STATE_OFF);
}

void trigger(void){
	reset();
	countdown = time1;
	setLeds(STATE_ON, STATE_OFF);
	digitalWrite(CONTROL_PORT, RELAY, phase1);
	resetTimer();
}

void setLeds(uint8_t led1, uint8_t led2){
	digitalWrite(CONTROL_PORT, LED1, led1);
	digitalWrite(CONTROL_PORT, LED2, led2);
}

void showLed(struct numArray numarr){
	for(int i = 0; i < 4; i++){
		uint8_t temp = numarr.chars[i];
		if(selected == i){
			temp = temp | blink;
		}
		for(int j = 0; j<8; j++){
			if(temp & 0x80){
				GPIO_WriteHigh(LED7_PORT, LED7_DIO);
			}else{
				GPIO_WriteLow(LED7_PORT, LED7_DIO);
			}
			temp = temp << 1;
			GPIO_WriteLow(LED7_PORT, LED7_SHIFT);
			GPIO_WriteHigh(LED7_PORT, LED7_SHIFT);
		}

		uint8_t pos = 0x08 >> i;

		for(int j = 0; j<8; j++){
			if(pos & 0x80){
				GPIO_WriteHigh(LED7_PORT, LED7_DIO);
			}else{
				GPIO_WriteLow(LED7_PORT, LED7_DIO);
			}
			pos = pos << 1;
			GPIO_WriteLow(LED7_PORT, LED7_SHIFT);
			GPIO_WriteHigh(LED7_PORT, LED7_SHIFT);
		}
		
		GPIO_WriteLow(LED7_PORT, LED7_LOAD);
		GPIO_WriteHigh(LED7_PORT, LED7_LOAD);
		Delay(3);
	}
}

struct numArray num2Array(int number){
	struct numArray numarr; 
	numarr.chars[0] = numbers[number/1000];
	number = number % 1000;
	numarr.chars[1] = numbers[number/100];
	number = number % 100;
	numarr.chars[2] = numbers[number/10] ^ 0x80;
	numarr.chars[3] = numbers[number%10];
	
	return numarr;	
}

struct numArray state2Array(uint8_t phase, uint8_t _state){
	struct numArray numarr;
	numarr.chars[0] = numbers[phase] ^ 0x80;
	
	if(_state == STATE_OFF){
		numarr.chars[1] = numbers[10];
		numarr.chars[2] = numbers[12];
		numarr.chars[3] = numbers[12];
	}else{
		numarr.chars[1] = numbers[13];
		numarr.chars[2] = numbers[10];
		numarr.chars[3] = numbers[11];
	}
	
	return numarr;
}

void ledProcess(void){
	if(state == 0){
		showLed(num2Array(countdown));
	}else if(state == 1){	// change phase 1 time
		showLed(num2Array(time1));
	}else if(state == 2){	// change phase 2 time
		showLed(num2Array(time2));
	}else if(state == 3){	// change phase state
		showLed(state2Array(1, phase1));
	}else if(state == 4){	// change phase state
		showLed(state2Array(2, phase2));
	}else if(state == 5){	// change phase state
		showLed(state2Array(3, phase3));
	}
}

void buttonProcess(void){
		
	uint8_t b1 = GPIO_ReadInputPin(CONTROL_PORT, BUTTON1);
	uint8_t b2 = GPIO_ReadInputPin(CONTROL_PORT, BUTTON2);
		
	if(b1 != button1State){
		if(b1 == false){
			if(state == 0){	
				reset();
			}else {
				if(selected == -1){
					state = state == 5? 1 : state + 1;
					if(state == 1) setLeds(STATE_ON, STATE_OFF);
					else if(state == 2) setLeds(STATE_OFF, STATE_ON);
					else setLeds(STATE_OFF, STATE_OFF);
				}else{
					int temp = 0;
					if(state == 1) temp = time1;
					else if(state == 2) temp = time2;
				
					struct numArray num;
					num.chars[0] = temp / 1000;
					num.chars[1] = (temp%1000) / 100;
					num.chars[2] = (temp%100) / 10;
					num.chars[3] = temp % 10;
					
					num.chars[selected] = num.chars[selected] == 9? 0: num.chars[selected] + 1;
					temp = num.chars[0]*1000 + num.chars[1]*100 + num.chars[2]*10 + num.chars[3];
					
					if(state == 1) time1 = temp;
					else if(state == 2) time2 = temp;
				}
			}
			changeState = false;
		}
		button1State = b1;
		b1tick = millis();
		timeout = millis();
	}else{
		if(state == 0){
			if(b1 == false && millis() -  b1tick >= LONG_PRESS){
				changeState = true;
				state = 1;
				setLeds(STATE_ON, STATE_OFF);
				timeout = millis();
			}
		}
	}
	
	if(b2 != button2State){
		if(b2 == false){
			if(state == 0){	
				trigger();
			}else if(state == 1 || state == 2){
				selected = selected == 3? -1 : selected + 1;
				blink = 0x00;
				blinkCount = 0;
			}else if(state == 3){
				phase1 = phase1 == 0;
			}else if(state == 4){
				phase2 = phase2 == 0;
			}else if(state == 5){
				phase3 = phase3 == 0;
			}
			changeState = false;
		}
		button2State = b2;
		b2tick = millis();
		timeout = millis();
	}
	
	if(state != 0 && millis() - timeout > TIME_OUT){
		selected = -1;
		reset();
		save();
	}
}

void setup(void){
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
	
	GPIO_DeInit(LED7_PORT);
	GPIO_DeInit(CONTROL_PORT);
	FLASH_DeInit();
	
	GPIO_Init(LED7_PORT, (GPIO_Pin_TypeDef)(LED7_SHIFT | LED7_LOAD | LED7_DIO), GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_Init(CONTROL_PORT, (GPIO_Pin_TypeDef)(RELAY | LED1 | LED2), GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(CONTROL_PORT, (GPIO_Pin_TypeDef)(BUTTON1 | BUTTON2), GPIO_MODE_IN_PU_NO_IT);
	
	TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_PERIOD);
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
	
	enableInterrupts();
	TIM4_Cmd(ENABLE);
	
	
	load();
	setLeds(STATE_OFF, STATE_OFF);
	digitalWrite(CONTROL_PORT, RELAY, STATE_OFF);
	timeout = b1tick = b2tick = tick1 = tick2 = millis();
}

void main(void){
	
	setup();
	
	while (1){
		ledProcess();
		buttonProcess();
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(u8* file, u32 line)
{ 
  while (1)
  {
  }
}
#endif