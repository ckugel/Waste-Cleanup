/*
*
*   uart.c
*
*
*
*
*
*   @author
*   @date
*/

#include "uart.h"

volatile short uart_event;
volatile char uart_char;

void uart_init(void){
	//TODO

    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;
    while(!(SYSCTL_PRUART_R & 0x2));
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
    while(!(SYSCTL_PRGPIO_R & 0x2));


    GPIO_PORTB_AFSEL_R |= 0x03;
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) | 0x00000011;
    GPIO_PORTB_DEN_R |= 0x03;
    GPIO_PORTB_DIR_R &= ~0x03;



    UART1_CTL_R &= 0xFE;
    UART1_IBRD_R = 8;
    UART1_FBRD_R = 44;
    UART1_LCRH_R = (UART_LCRH_WLEN_8);
    UART1_CC_R = UART_CC_CS_SYSCLK;
    UART1_CTL_R |= (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);
}

void uart_sendChar(char data){
	//TODO
    while(UART1_FR_R & UART_FR_TXFF){};
    UART1_DR_R =data;
}

char uart_receive(void){
	//TODO
    while (UART1_FR_R & UART_FR_RXFE){};
    return (char)(UART1_DR_R & 0xFF);
}

void uart_sendStr(const char *data){
	//TODO for reference see lcd_puts from lcd.c file
    while(*data != '\0'){
        uart_sendChar(*data);
        data++;
    }
}

void uart_interrupt_init()
{
    // Enable interrupts for receiving bytes through UART1
    UART1_IM_R |= 0x10; //enable interrupt on receive - page 924

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= 0x40; //enable uart1 interrupts - page 104

    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104
    IntRegister(INT_UART1, uart_interrupt_handler); //give the microcontroller the address of our interrupt handler - page 104 22 is the vector number
}

void uart_interrupt_handler()
{
// STEP1: Check the Masked Interrupt Status
    while (UART1_MIS_R & 0x10 == 0){};
//STEP2:  Copy the data
    uart_event = 1;
    uart_char = (char)(UART1_DR_R & 0xFF);
//STEP3:  Clear the interrupt
    UART1_ICR_R |= 0x10;
}
