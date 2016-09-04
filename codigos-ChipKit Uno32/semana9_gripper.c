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

void init ()
{
	//OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_64, T1_TICK); //Configuracion del Timer1 para generar interrupcion cada 1 ms
    //OpenCoreTimer(CORE_TICK); // 1 ms
    OpenOC1(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0); // Configuracion de OutputCompare1 para generar salidas PWM
    OpenOC2(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0); // Configuracion de OutputCompare2 para generar salidas PWM
    
	/**
	OpenTimer2(T2_ON | T2_PS_1_64 | T2_SOURCE_INT, PWM_PERIOD); //Configuracion del Timer2 para la generacion del DutyCycle
    OpenOC3(OC_ON | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0); // Configuracion de OutputCompare2 para generar salidas PWM
    OpenOC4(OC_ON | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0); // Configuracion de OutputCompare2 para generar salidas PWM
    OpenTimer3(T3_ON | T3_PS_1_64 | T3_SOURCE_INT, PWM_PERIOD); //Configuracion del Timer2 para la generacion del DutyCycle
	/**/
	
    //Configuracion de Niveles de Prioridad
    //INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_2);
    //INTSetVectorPriority(INT_CORE_TIMER_VECTOR, INT_PRIORITY_LEVEL_3);
    //INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_3);
    //INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_4);

    //Habilitar interrupciones de Timer1,CoreTimer,Timer2 y Timer3
    //INTEnable(INT_T1, INT_ENABLED);
    //INTEnable(INT_CT, INT_ENABLED);
    //INTEnable(INT_T2, INT_ENABLED);
    //INTEnable(INT_T3, INT_ENABLED);

    INTEnableSystemMultiVectoredInt();

    //Configuracion del ADC
	/**
    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    CloseADC10();

    #define PARAM1  ADC_MODULE_ON | ADC_FORMAT_INTG | ADC_CLK_AUTO | ADC_AUTO_SAMPLING_ON
    #define PARAM2  ADC_VREF_AVDD_AVSS | ADC_OFFSET_CAL_DISABLE | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_2 | ADC_ALT_BUF_ON | ADC_ALT_INPUT_ON
    #define PARAM3  ADC_CONV_CLK_INTERNAL_RC | ADC_SAMPLE_TIME_15
    #define PARAM4  SKIP_SCAN_ALL
    #define PARAM5  ENABLE_AN14_ANA | ENABLE_AN15_ANA //Definicion de Pines para Lectura Analogica de Potenciometro en AN5 y AN11

    SetChanADC10(ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN14 | ADC_CH0_NEG_SAMPLEB_NVREF | ADC_CH0_POS_SAMPLEB_AN15); // configure to sample AN4 & AN5
    OpenADC10(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5);

    EnableADC10(); //Habilitar el ADC
    
    while (!mAD1GetIntFlag()) //Espera que termine la conversión
    {
        
    }
	/**/
}

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

void close_Gripper()
{
    OC1RS = 300;
}
void open_Gripper()
{ 
    OC1RS = 1100;
}
void up_Gripper()
{
    OC2RS = 800;
}
void down_Gripper()
{
    OC2RS = 300;
}

main()
{
    TRISD = 0x00;	//Puerto D como salidas
    PORTD = 0x00;	//Inicializa salidas en 0
    TRISE = 0xFF;	//Puerto E como entradas
    
	OC1RS = 0;		//Inicializa servos en 0
    OC2RS = 0;
	
	init();
	
	moveFwd(50);
	open_Gripper();
	moveFwd(5);
	close_Gripper();
	up_Gripper();
    
    while (1)
    {
       
    }
}