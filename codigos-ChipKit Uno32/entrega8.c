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

void delay (int x)
{
    while (x > 0)
        x--;
}

void moveFwd (int cm)
{
    PORTDbits.RD11 = 1;
	delay(5);
    PORTDbits.RD7 = 1;
    delay (cm*offset);
    
    PORTDbits.RD11 = 0;
	delay(5);
    PORTDbits.RD7 = 0;
    delay(5);
}

void moveBack (int cm)
{
    PORTDbits.RD5 = 1;
	delay(5);
    PORTDbits.RD6 = 1;
	delay(cm*offset);

    PORTDbits.RD5 = 0;
	delay(5);
    PORTDbits.RD6 = 0;
    delay(5);
}

void rotClk (int deg)
{
    int cm = deg*pi*8/180;
    PORTDbits.RD11 = 1;
    delay(5);
    PORTDbits.RD6 = 1;
	delay(cm*offset);

    PORTDbits.RD11 = 0;
    delay(5);
    PORTDbits.RD6 = 0;
    delay(5);
}

void rotCount (int deg)
{
    int cm = deg*pi*8/180;
    PORTDbits.RD7 = 1;
    delay(5);
    PORTDbits.RD5 = 1;
	delay(cm*offset);

    PORTDbits.RD7 = 0;
    delay(5);
    PORTDbits.RD5 = 0;
    delay(5);
}

main()
{
    TRISD = 0x00; //Salidas
    PORTD = 0x00; //Salidas
    TRISE = 0xFF; //Entradas
    
    moveFwd(50);
    rotClk(90);
    moveBack(50);
    rotCount(180);
    moveFwd(50);
    rotCount(90);
    moveBack(50);
    rotClk(180);
    moveBack(100);
    
    while (1)
    {
       
    }
}