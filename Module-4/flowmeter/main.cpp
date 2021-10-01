/**----------------------------------------------------------------------------
 
   \file main.cpp
--                                                                           --
--              ECEN 5803 Mastering Embedded System Architecture             --
--                  Project 1 Module 4                                       --
--                Microcontroller Firmware                                   --
--                      main.cpp                                             --
--                                                                           --
-------------------------------------------------------------------------------
--
--  Designed for:  University of Colorado at Boulder
--               
--                
--  Designed by:  Tim Scherr
--  Revised by:  Student's name 
-- 
-- Version: 2.1
-- Date of current revision:  2017-09-25   
-- Target Microcontroller: Freescale MKL25ZVMT4 
-- Tools used:  ARM mbed compiler
--              ARM mbed SDK
--              Freescale FRDM-KL25Z Freedom Board
--               
-- 
-- Functional Description:  Main code file generated by mbed, and then
--                           modified to implement a super loop bare metal OS.
--
--      Copyright (c) 2015, 2016 Tim Scherr  All rights reserved.
--
*/

#define MAIN
#include "shared.h"
#undef MAIN
#include "adc.h"
#include "flowmeter.h"

extern volatile uint16_t SwTimerIsrCounter; 
extern volatile uint8_t redLedState; 

Ticker tick;             //  Creates a timer interrupt using mbed methods
 /****************      ECEN 5803 add code as indicated   ***************/
                // Add code to control red, green and blue LEDs here
DigitalOut redLED(LED_RED, 1);
DigitalOut greenLED(LED_GREEN, 1);                                                    

Serial pc(USBTX, USBRX);
//PwmOut _4_20_current(PTE30);
//PwmOut _pulse(PTE31);
//SPI lcd_disp(PTD2, PTD3, PTD1, PTD0); // MOSI MISO SCLK CS

/*
 * @brief Blinks the green LED
 */
void flip() 
{
    greenLED = !greenLED;
}

/*
 * @brief blinks the RED LED regularly to indicate hearbeat.
 */
void flip_r()  
{                
    redLED = !redLED;
}
 

float frequnecy = 88.0f, flow = 0.0f;
/**
 * @brief Main routine used for initialization and driving the
 *        code. This routine is doing the following
 *        - Initilizing hardware
 *        - Starting the Interrupt
 *        - Polling for console inputs
 *        - Computing the flow rate
 *        - Running the hearbeat led
 * 
 * @return return 1
 */
int main() 
{
/****************      ECEN 5803 add code as indicated   ***************/
                    //  Add code to call timer0 function every 100 uS
	  Adc_Init();
		tick.attach(&timer0, 0.0001);
    uint32_t  count = 0;
    
// initialize serial buffer pointers
   rx_in_ptr =  rx_buf; /* pointer to the receive in data */
   rx_out_ptr = rx_buf; /* pointer to the receive out data*/
   tx_in_ptr =  tx_buf; /* pointer to the transmit in data*/
   tx_out_ptr = tx_buf; /*pointer to the transmit out */
    
   
  // Print the initial banner
    pc.printf("\r\nHello World!\n\n\r");

    /****************      ECEN 5803 add code as indicated   ***************/
    // uncomment this section after adding monitor code.  
   /* send a message to the terminal  */                    
   
   UART_direct_msg_put("\r\nSystem Reset\r\nCode ver. ");
   UART_direct_msg_put( CODE_VERSION );
   UART_direct_msg_put("\r\n");
   UART_direct_msg_put( COPYRIGHT );
   UART_direct_msg_put("\r\n");

		set_display_mode();                                      
		
    while(1)       // Cyclical Executive Loop
    {

        count++;                  // counts the number of times through the loop
//      __enable_interrupts();
//      __clear_watchdog_timer();
       
        serial();            // Polls the serial port
        chk_UART_msg();     // checks for a serial port message received
        monitor();           // Sends serial port output messages depending
                         //  on commands received and display mode
 
   /****************      ECEN 5803 add code as indicated   ***************/
   
//  readADC()
		uint16_t vref = Adc_ReadMesaurement(CHANNEL_0);
		uint16_t temp = Adc_ReadMesaurement(CHANNEL_2);
		float v1 = vref*3.3f/65536;                               //Convert v1 to Volts
    float v2 = temp*3300.0f/65536;                            //Convert v2 to millivolts
    float temp_t = 25.0f-(v2-716)*1000.0f/1620;                  //Calculate temp in Celsius
//  calculate flow()
		    flow = calculateFlow(frequnecy, temp_t);
		//pc.printf("Flow rate: %f\r\n", flow);
//  4-20 output ()    // use TMP0 channel 3  proporional rate to flow
		    //_4_20_current.pulsewidth(flow);
//  Pulse output()   // use TMP0 channel 4  propotional rate to frequency
		    //_pulse.period_us((float)1.0f/frquency);
//  LCD_Display()   // use the SPI port to send flow number
		    //lcd_disp.write((float)flow);
//  End ECEN 5803 code addition


        if ((SwTimerIsrCounter & 0x1FFF) > 0x0FFF)
        {
            flip();  // Toggle Green LED
					  SwTimerIsrCounter = 0;
        }
				if(redLedState)
				{	
						// Toggle Red LED every 500ms to ensure life
						flip_r();
					  redLedState = 0;
				}
    } 
	  return 1;
}

