#include "stdint.h"
#include "stdio.h"
#include "math.h"
#include "led.h"
#include "usart.h"
#include <stdbool.h>

#define MYTXPIN 6
#define MYRXPIN 7

void initUsart(uint32_t baud);
void outc(char c);
void outs(char *s);
void outx4(uint8_t n);
void outx8(uint8_t b);
void outx16(uint16_t w);
void outi(int32_t i);
void outc_test();
void outs_test();
void outx4_test();
void outx8_test();
void outx16_test();
void outi_test();

void draw_rect_center()
{

	uint16_t rectSize = 100;
	uint16_t centerX = BSP_LCD_GetXSize() / 2;
	uint16_t centerY = BSP_LCD_GetYSize() / 2;
	uint16_t rectX = centerX - rectSize / 2;
	uint16_t rectY = centerY - rectSize / 2;
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(rectX, rectY, rectSize, rectSize); //fill with textcolor
}

void draw_text_center(char *text)
{
	uint16_t centerX = BSP_LCD_GetXSize() / 2;
	uint16_t centerY = BSP_LCD_GetYSize() / 2;
	sFONT *textFont = &Font16;
	uint16_t textH = textFont->Height;
	uint16_t textW = textFont->Width*strlen(text);
	BSP_LCD_SetFont(textFont);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
	BSP_LCD_DisplayStringAt(centerX - textW / 2, centerY - textH / 2, (uint8_t *)text, LEFT_MODE);
}

int main(void)
{




	LCD_LOG_DeInit();

	/*BSP_LCD_Init();
	BSP_LCD_Clear(LCD_COLOR_YELLOW);
	draw_rect_center();
	draw_text_center("Hallo MIT Labor!");*/
	outc_test();
	outs_test();
	outx4_test();
	outx8_test();
	outx16_test();
	outi_test();
	while (1) {

	}


}

void initUsart(uint32_t baudrate) {
	GPIO_TypeDef  *gpioc = GPIOC;         // Zeiger auf GPIOx Register
	USART_TypeDef *usart6 = USART6;        // Zeiger auf USART6 Register

	// USART /Port Clocks
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // GPIOC Clock enable
	RCC->APB2ENR |= RCC_APB2ENR_USART6EN;// Usart6 Clock on

	//USART Sendeteil Konfiguration
	gpioc->MODER |= 0b10 << (2 * MYTXPIN);   // Alternate Function enable
	gpioc->OSPEEDR |= 0b10 << (2 * MYTXPIN);   // I/O High Speed
	gpioc->PUPDR |= 0b00 << (2 * MYTXPIN);   // No PullUp/PullDown
	gpioc->AFR[0] |= 0b1000 << (4 * MYTXPIN); // AF8 -> USART6/TX an PC6

	// USART Empfangsteil Konfiguration
	gpioc->PUPDR |= 0b01 << (2 * MYRXPIN); // PullUp an
	gpioc->MODER |= 0b10 << (2 * MYRXPIN);   // Alternate function
	gpioc->AFR[0] |= 0b1000 << (4 * MYRXPIN); // AF8 -> USART6/RX an PC7

	// USART allgemein
	usart6->BRR = (SystemCoreClock / 2) / baudrate; // ABP2 Clock base
	usart6->CR1 |= USART_CR1_TE |        // Sendeteil einschalten
		USART_CR1_RE |        // Empfangsteil einschalten
		USART_CR1_UE;        // USART einschalten
}

void outc(char c) {
	while (!(USART6->ISR & USART_ISR_TXE));
	USART6->TDR = c;
	printf("%c", c);

}

void outs(char *s) {
	for (int i = 0; i < 13; i++) {
		outc(s[i]);
	}
}

void outx4(uint8_t n) {
	char nibble = n & 0x0F;
	int zahl = (int)nibble + 48;
	if (zahl > 57) zahl += 39;
	outc((char)zahl);
}

void outx8(uint8_t b) {
	uint8_t z = (b >> 4);
	outx4(z);
	outx4(b);

}

void outx16(uint16_t w) {
	uint16_t z = (w >> 8);
	outx8(z);
	outx8(w);

}

void outi(int32_t i) {
	if (i < 0) {
		outc('-');
		outi((i*(-1)));
	}
	else if (i == 0) {
		outc((char)0x30);
	}
	else {
		unsigned int n = 0;
		int j = i;
		int k;
		while (j != 0) {
			k = (j % 10);
			j = (j - k) / 10;
			n++;
		}
		j = i;

		while (n > 0) {
			k = (j % (int)pow(10, n - 1));
			outc((j - k) / pow(10, n - 1) + 48);
			j = k;
			n--;
		}
	}
}

void outc_test() {
	initUsart(9600);
	outc('H');
	outc('a');
	outc('l');
	outc('l');
	outc('o');
	outc('\n');
}

void outs_test() {
	initUsart(9600);
	outs("Hello World!\n");
}

void outx4_test() {
	initUsart(9600);
	uint8_t nibble = 0xF0;
	for (int i = 0; i < 16; i++) {
		outx4(nibble + i);
		BSP_LCD_Init();
	}
	outc('\n');
}

void outx8_test() {
	initUsart(9600);
	uint8_t byte = 0x90;
	for (int i = 0; i < 32; i++) {
		outx8(byte + i);
	}
	outc('\n');
	for (int i = 0; i < 16; i++) {
		outx8(15 - i + (i << 4));
	}
	outc('\n');
}

void outx16_test() {
	initUsart(9600);
	uint16_t word = 0xaa55;
	for (int i = 0; i < 32; i++) {
		outx16(word + 0x1111 * i);
	}
	outc('\n');
}

void outi_test() {
	initUsart(9600);
	for (int i = -10; i <= 10; i++) {
		outi(i);
		outc(' ');
	}
	outc('\n');
	for (int i = -10; i <= 10; i++) {
		outi(i);
		outc(' ');
		outi(i*i);
		outc(' ');
		outi(i*i*i);
		outc('\n');
	}
}






