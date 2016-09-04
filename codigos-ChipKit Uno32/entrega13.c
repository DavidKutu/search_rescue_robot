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
int lift_servo = 0;				//variables para mover los servos
int drop_servo = 0;
int open_servo = 0;
int close_servo = 0;
unsigned long dis0, dis1, dis2, dis3, dis4, dis5;
int sensor_state = 0;
int stop;
unsigned long senx, seny;

void init ()
{
	OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_64, T1_TICK); //Configuracion del Timer1 para generar interrupcion cada 1 ms
    OpenCoreTimer(CORE_TICK); // 1 ms
    OpenOC1(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0); // Configuracion de OutputCompare1 para generar salidas PWM
    OpenOC2(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0); // Configuracion de OutputCompare2 para generar salidas PWM
	
    //Configuracion de Niveles de Prioridad
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_2);
    INTSetVectorPriority(INT_CORE_TIMER_VECTOR, INT_PRIORITY_LEVEL_3);
    //INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_3);
    //INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_4);

    //Habilitar interrupciones de Timer1,CoreTimer,Timer2 y Timer3
    INTEnable(INT_T1, INT_ENABLED);
    INTEnable(INT_CT, INT_ENABLED);
    //INTEnable(INT_T2, INT_ENABLED);
    //INTEnable(INT_T3, INT_ENABLED);

    INTEnableSystemMultiVectoredInt();
}

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

		case 3 :				//sensor izq atras
			PORTDbits.RD4 = 1;
    		delay(100);
    		PORTDbits.RD4 = 0;
    		delay(100);
			break;

		case 2 :				//sensor der adelante
			PORTDbits.RD8 = 1;
			delay(100);
			PORTDbits.RD8 = 0;
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

void fix_der ()		//mover llanta derecha
{
    PORTDbits.RD11 = 0;
    delay(5);
    PORTDbits.RD7 = 0;
    delay(5);
            
    PORTDbits.RD7 = 1;
    delay(1000000);
            
    PORTDbits.RD7 = 0;
    delay(5);
}

void fix_izq ()		//mover llanta izq
{
    PORTDbits.RD11 = 0;
    delay(5);
    PORTDbits.RD7 = 0;
    delay(5);
            
    PORTDbits.RD11 = 1;
    delay(1000000);
            
    PORTDbits.RD11 = 0;
    delay(5);
}

int servo(int DC)	//Esto está para el pot
{
    int DutyCA = DC * PWM_PERIOD / 1023;
    int Ser = 0.05 * DutyCA + 1249;
    return Ser;
}

main()
{
    TRISD = 0x00;	//Puerto D como salidas
    PORTD = 0x00;	//Inicializa salidas en 0
    TRISE = 0xFF;	//Puerto E como entradas
    
	OC1RS = 0;		//Inicializa servos en 0
    OC2RS = 0;
	
	init();
	int state = 0;
    
    while (1)
    {
		switch (state)
		{
			case 0 :	
				moveFwd(30);	//avanzar
				rotCount(90);	//vuelta izq (counter clk)
				moveFwd(30);	//avanzar
				rotClk(90);		//vuelta der
				state++;
				break;
				
			case 1 :	//avanzar
				sensor_state = 1;		//sensor 0, 2, 4
				while (dis0 > 30)
				{
					PORTDbits.RD11 = 1;
					delay(100);
					PORTDbits.RD7 = 1;
					delay(100);
					
					if (dis2 < dis4)
						fix_der();
					else if (dis2 > dis4)
						fix_izq();
				}
				PORTDbits.RD11 = 0;
				delay(100);
				PORTDbits.RD7 = 0;
				delay(100);
				
				state++;
				sensor_state = 0;
				break;
				
			case 2 :	//agarrar victima
				//FALTAAAAA
				state++;
				break;
				
			case 3 :	//vuelta en 180
				rotClk(180);
				state++;
				break;
				
			case 4 :	//avanzar
				//FATLAAAA CORREGIR QUE LIDIE CON EL HEUCO AL FINAL
				sensor_state = 2;			//sensors 0, 1, 3
				while (dis0 > 30)
				{
					PORTDbits.RD11 = 1;
					delay(100);
					PORTDbits.RD7 = 1;
					delay(100);
					
					if (dis1 < dis3)
						fix_izq();
					else if (dis1 > dis3)
						fix_der();
				}
				PORTDbits.RD11 = 0;
				delay(100);
				PORTDbits.RD7 = 0;
				delay(100);
				
				sensor_state = 0;
				state++;
				break;
				
			case 5 :	//suelta victima
				//FALTAAAA
				state++;
				break;
			
			case 6 : 	//vuelta 180 y avanza hasta encontrar hueco
				rotClk(180);
				sensor_state = 3;		//sensores 1 y 3
				
				pulseOut(1);
				senx = pulseIn(1);
				
				pulseOut(3);
				seny = pulseIn(3);
				
				stop = 0;
				while (!stop)
				{
					PORTDbits.RD11 = 1;
					delay(100);
					PORTDbits.RD7 = 1;
					delay(100);
					
					if ((dis1 + 200 < senx) && (dis3 + 200 < seny))
						stop = 1;
				}
				PORTDbits.RD11 = 0;
				delay(100);
				PORTDbits.RD7 = 0;
				delay(100);
				
				sensor_state = 0;
				state++;
				break;
				
			case 7 :		//vuelta a la izq y avanza hasta que sensor 5 lo detenga
				rotCount(90);
				sensor_state = 4;		//sensor 5
				
				//avanzar hasta que el sensor 5 valga mas que x
				
				
				sensor_state = 0;
				state++;
				break;
				
			case 8 :	//ir por segunda victima
				rotCount(90);
				sensor_state = 2;			//sensors 0, 1, 3
				while (dis0 > 30)
				{
					PORTDbits.RD11 = 1;
					delay(100);
					PORTDbits.RD7 = 1;
					delay(100);
					
					if (dis1 < dis3)
						fix_izq();
					else if (dis1 > dis3)
						fix_der();
				}
				PORTDbits.RD11 = 0;
				delay(100);
				PORTDbits.RD7 = 0;
				delay(100);
				
				sensor_state = 0;
				state++;
				break;
				
			case 9 :	//agarrar victima
				//FALTAAAAA
				state++;
				break;
				
			case 10 :	//vuelta 180 y avanza hasta encontrar hueco en la derecha
				rotClk(180);
				sensor_state = 5;		//sensores 2 y 4
				
				pulseOut(2);
				senx = pulseIn(2);
				
				pulseOut(4);
				seny = pulseIn(4);
				
				stop = 0;
				while (!stop)
				{
					PORTDbits.RD11 = 1;
					delay(100);
					PORTDbits.RD7 = 1;
					delay(100);
					
					if ((dis2 + 200 < senx) && (dis4 + 200 < seny))
						stop = 1;
				}
				PORTDbits.RD11 = 0;
				delay(100);
				PORTDbits.RD7 = 0;
				delay(100);
				
				sensor_state = 0;
				state++;
				break;
				
			case 11 : 		//turn rigth, move forward, and turn rigth again
				rotClk(90);
				moveFwd(30); //revisar distancia
				rotClk(90);
				state++;
				break;
				
			case 12 : 		//avanzar derecho
				sensor_state = 2;			//sensors 0, 1, 3
				while (dis0 > 30)
				{
					PORTDbits.RD11 = 1;
					delay(100);
					PORTDbits.RD7 = 1;
					delay(100);
					
					if (dis1 < dis3)
						fix_izq();
					else if (dis1 > dis3)
						fix_der();
				}
				PORTDbits.RD11 = 0;
				delay(100);
				PORTDbits.RD7 = 0;
				delay(100);
				
				sensor_state = 0;
				state++;
				break;
				
			case 13 :	//suelta victima
				//FALTAAAA
				state++;
				break;
			
			
			
			
			
			/**/
			case 17 :	//retrocede
				moveBack(10);
				state++;
				break;
				
			case 18 :	//vuelta izq (couter clk)
				rotCount(90);
				state++;
				break;
				
			case 19 :	//avanzar
				moveFwd(30);
				state++;
				break;
				
			case 20 :	//vuelta der (clkwise)
				rotClk(90);
				state++;
				break;
				
			case 21 :	//avanzar
				moveFwd(30);
				state++;
				break;
			/**/
		}
    }
}

void __ISR(_TIMER_1_VECTOR, ipl2) Timer1Handler(void) 
{
	INTDisableInterrupts(); 		//Disable interrupts
	mT1ClearIntFlag(); 				//Limpiar la bandera de interrupcion
	
	INTEnableInterrupts();			//Enables interrupts again
}

void __ISR(_CORE_TIMER_VECTOR, ipl3) CoreTimerHandler(void)
{
	INTDisableInterrupts(); 		//Disable interrupts
	mCTClearIntFlag();				//Limpiar la bandera de interrupcion
	UpdateCoreTimer(CORE_TICK);		// Reinicia el CoreTimer
	
	switch (sensor_state)
	{
		case 0 :					//do nothing
		
			break;
		
		case 1 :					//sensors 0, 2, 4
			pulseOut(0);			//move straigth with wall on right
			dis0 = pulseIn(0);
			delay(10000);
			
			pulseOut(2);
			dis2 = pulseIn(2);
			delay(10000);
			
			pulseOut(4);
			dis4 = pulseIn(4);
			delay(10000);
			break;
		
		case 2 :					//sensors 0, 1, 3
			pulseOut(0);			//move straigth with wall on left
			dis0 = pulseIn(0);
			delay(10000);
			
			pulseOut(1);
			dis1 = pulseIn(1);
			delay(10000);
			
			pulseOut(3);
			dis3 = pulseIn(3);
			delay(10000);
			break;
			
		case 3 :					//sensors 1 and 3
			pulseOut(1);
			dis1 = pulseIn(1);
			delay(10000);
			
			pulseOut(3);
			dis3 = pulseIn(3);
			delay(10000);
			break;
			
		case 4 :					//sensor 5
			pulseOut(5);
			dis5 = pulseIn(5);
			delay(10000);
			break;
			
		case 5 : 					//sensors 2 and 4
			pulseOut(2);
			dis2 = pulseIn(2);
			delay(10000);
			
			pulseOut(4);
			dis4 = pulseIn(4);
			delay(10000);
			break;
	}
	
	INTEnableInterrupts();			//Enables interrupts again
}