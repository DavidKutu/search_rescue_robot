#include <xc.h>
#include <plib.h>

//Configuration
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPLLMUL = MUL_20 //Oscilador de cristal > 4MHZ,Oscilador primario con PLL,Multiplicador de frecuencia x 20
#pragma config FPLLIDIV = DIV_2, FPLLODIV = DIV_1 // //Divisor de entrada 1:2, Divisor de salida 1:1
#pragma config FSOSCEN = OFF, FPBDIV = DIV_1 //Oscilador externo secundario off,PBCLK (PeripheralBusClock) igual a SYSCLK
#pragma config FWDTEN = OFF, CP = OFF, FCKSM = CSECMD //Watch Dog off, Codigo de Programa off

#define SYS_FREQ        (80000000L)
#define PRESCALER       64
#define T1_TICK         1250 //T1_TICK = (0.001 / (T_PBCLK * PRESCALER)) = 1 ms
#define CORE_TICK       40000 //CORE_TICK= (0.001 / (T_SYS_FREQ * 2)) = 1 ms
#define PWM_FREQ        50

float PWM_PERIOD = (SYS_FREQ / (PWM_FREQ * PRESCALER)) - 1;
float pi = 3.14159265359;

unsigned long waaa = 0;

void delay (int x)
{
    while (x > 0)
        x--;
}

void pulse()
{
    PORTDbits.RD3 = 1;
    delay(100);
    PORTDbits.RD3 = 0;
    delay(100);
}

unsigned long pulseIn()
{
    unsigned long width = 0;
    
   	// wait for any previous pulse to end
	while (PORTEbits.RE0 == 1)  {}
	
	// wait for the pulse to start
	while (PORTEbits.RE0 != 1)  {}
	
	// wait for the pulse to stop
	while (PORTEbits.RE0 == 1)
	{
		width++;
	}
    
    return width;
}

main()
{
    TRISD = 0x00; //Salidas
    PORTD = 0x00; //Salidas
    TRISE = 0xFF; //Entradas
    //ANSELD = 0;

    PORTDbits.RD11 = 1;
    delay(100);
    PORTDbits.RD7 = 1;
    delay(1000000);
    
    
    while (1)
    {
        pulse();
        waaa = pulseIn();
        delay(10000);
        
        if (waaa < 500)
        {
            PORTDbits.RD11 = 0;
            delay(5);
            PORTDbits.RD7 = 0;
            delay(5);
        }
    }
}