#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#define __FREQ__ (16000000)
#define FROM_MS_TO_TICKS(x) (x * (__FREQ__/1000) - 1)


void SysTickHandler(void);
void Toggle_White(void);


int main()
{
  ////  Initialization  ////
  // Enable Port F.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
  
  // Unlock the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  // Commit the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;
  
  // Set pins 1..3 as outputs (LEDs).
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
    GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
  
  // Initialize Systick.
  SysTickDisable();
  SysTickPeriodSet(FROM_MS_TO_TICKS(500));
  SysTickIntEnable();
  SysTickIntRegister(SysTickHandler);
  SysTickEnable();
  
  __asm ("    CPSIE  I\n");
  
  while (1) {
    __asm(" wfi\n");
  }
   
  return 0;
}



void SysTickHandler(void)
{
    static int halfSeconds = 0;
    if (halfSeconds == 4)
    {
        Toggle_White();         /* toggle the white LED */
    }
    halfSeconds = (halfSeconds + 1) % 5;
}


void Toggle_White(void)
{
  int val = GPIOPinRead(GPIO_PORTF_BASE, 
                 (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3));
  GPIOPinWrite(GPIO_PORTF_BASE, 
                 (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3),
                 (~val));
}