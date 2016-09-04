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
unsigned int offset = 150000;   //100000 para piso
unsigned long dis0, dis1, dis2, dis3;

void delay (int x)
{
    while (x > 0)
        x--;
}

void pulseOut (int x)
{
	switch (x)
	{
		case 0 :				//sensor delantero
			PORTDbits.RD2 = 1;
    		delay(100);
   			PORTDbits.RD2 = 0;
    		delay(100);
			break;

		case 1 :				//sensor izq adelante
			PORTDbits.RD3 = 1;
    		delay(100);
    		PORTDbits.RD3 = 0;
    		delay(100);
			break;

		case 2 :				//sensor der adelante
			PORTDbits.RD8 = 1;
			delay(100);
			PORTDbits.RD8 = 0;
			delay(100);
			break;
			
		case 3 :				//sensor izq atras
			PORTDbits.RD4 = 1;
    		delay(100);
    		PORTDbits.RD4 = 0;
    		delay(100);
			break;

		case 4 :				//sensor der atras
			PORTDbits.RD9 = 1;
			delay(100);
			PORTDbits.RD9 = 0;
			delay(100);
			break;

		case 5 :				//sensor trasero
			PORTDbits.RD10 = 1;
			delay(100);
			PORTDbits.RD10 = 0;
			delay(100);
			break;
	}
}

unsigned long pulseIn (int x)
{
	unsigned long width = 0;
	
	switch (x)
	{
		case 0 :				//sensor delantero
			// wait for any previous pulse to end
			while (PORTEbits.RE0 == 1)  {}
			
			// wait for the pulse to start
			while (PORTEbits.RE0 != 1)  {}
			
			// wait for the pulse to stop
			while (PORTEbits.RE0 == 1)
				width++;
			
			return width;
			break;

		case 1 :				//sensor izq adelante
			// wait for any previous pulse to end
			while (PORTEbits.RE1 == 1)  {}
			
			// wait for the pulse to start
			while (PORTEbits.RE1 != 1)  {}
			
			// wait for the pulse to stop
			while (PORTEbits.RE1 == 1)
				width++;
			
			return width;
			break;

		case 3 :				//sensor izq atras
			// wait for any previous pulse to end
			while (PORTEbits.RE3 == 1)  {}
			
			// wait for the pulse to start
			while (PORTEbits.RE3 != 1)  {}
			
			// wait for the pulse to stop
			while (PORTEbits.RE3 == 1)
				width++;
			
			return width;
			break;

		case 2 :				//sensor der adelante
			// wait for any previous pulse to end
			while (PORTEbits.RE2 == 1)  {}
			
			// wait for the pulse to start
			while (PORTEbits.RE2 != 1)  {}
			
			// wait for the pulse to stop
			while (PORTEbits.RE2 == 1)
				width++;
			
			return width;
			break;

		case 4 :				//sensor der atras
			// wait for any previous pulse to end
			while (PORTEbits.RE4 == 1)  {}
			
			// wait for the pulse to start
			while (PORTEbits.RE4 != 1)  {}
			
			// wait for the pulse to stop
			while (PORTEbits.RE4 == 1)
				width++;
			
			return width;
			break;

		case 5 :				//sensor trasero
			// wait for any previous pulse to end
			while (PORTEbits.RE5 == 1)  {}
			
			// wait for the pulse to start
			while (PORTEbits.RE5 != 1)  {}
			
			// wait for the pulse to stop
			while (PORTEbits.RE5 == 1)
				width++;
			
			return width;
			break;
	}
}

main()
{
    TRISD = 0x00;	//Puerto D como salidas
    PORTD = 0x00;	//Inicializa salidas en 0
    TRISE = 0xFF;	//Puerto E como entradas

	delay(10000);
	
	PORTDbits.RD11 = 0;
    delay(5);
    
	pulseOut(1);
	dis0 = pulseIn(1);
    
    pulseOut(3);
    dis2 = pulseIn(3);
	
    while (1)
    {
		pulseOut(1);
		dis1 = pulseIn(1);
        
        pulseOut(3);
        dis3 = pulseIn(3);
		
		if ((dis0 + 500 < dis1) && (dis2 + 500 < dis3))
        //if (dis0 + 200 < dis1)
		{
            delay(500000);
			PORTDbits.RD11 = 1;
			delay(5);
		}
    }
}