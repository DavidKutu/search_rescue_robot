#include <xc.h>
#include <plib.h>

//Configuration Bits
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

int contDC0 = 0;
int contDC1 = 0;
unsigned int An_value0; //Valor analógico que se leerá del Potenciometro
unsigned int An_value1;
unsigned int offset;
int contADC = 0;
int contVel = 0;

int speed(int DC)
{
    return (45 * DC);
}

int servo(int DC)
{
    int DutyCA = DC * PWM_PERIOD / 1023;
    int Ser = 0.05 * DutyCA + 1249;
    return Ser;
}

Init()
{
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_64, T1_TICK); //Configuracion del Timer1 para generar interrupcion cada 1 ms

    OpenCoreTimer(CORE_TICK); // 1 ms

    OpenOC1(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0); // Configuracion de OutputCompare1 para generar salidas PWM
    OpenOC2(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0); // Configuracion de OutputCompare2 para generar salidas PWM
    OpenTimer2(T2_ON | T2_PS_1_64 | T2_SOURCE_INT, PWM_PERIOD); //Configuracion del Timer2 para la generacion del DutyCycle

    OpenOC3(OC_ON | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0); // Configuracion de OutputCompare2 para generar salidas PWM
    OpenOC4(OC_ON | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0); // Configuracion de OutputCompare2 para generar salidas PWM
    OpenTimer3(T3_ON | T3_PS_1_64 | T3_SOURCE_INT, PWM_PERIOD); //Configuracion del Timer2 para la generacion del DutyCycle

    //Configuracion de Niveles de Prioridad
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_2);
    INTSetVectorPriority(INT_CORE_TIMER_VECTOR, INT_PRIORITY_LEVEL_3);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_3);
    INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_4);

    //Habilitar interrupciones de Timer1,CoreTimer,Timer2 y Timer3
    INTEnable(INT_T1, INT_ENABLED);
    INTEnable(INT_CT, INT_ENABLED);
    INTEnable(INT_T2, INT_ENABLED);
    INTEnable(INT_T3, INT_ENABLED);

    INTEnableSystemMultiVectoredInt();

    //Configuracion del ADC

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
}

main() 
{
    TRISD = 0x00; //Salidas
    PORTD = 0x00; //Salidas
    TRISE = 0xFF; //Entradas

    Init();

    //Valores iniciales de DutyCycle en 0
    OC1RS = 0; //LLanta Derecha Trasera, Cable Azul, ( 1 ms = Adelante, 2 ms = Atras, 1.5 ms = Detener)
    OC2RS = 0; //LLanta Derecha Delantera, Cable Naranja Largo

    while (1)
    {
        
    }
}

void __ISR(_TIMER_1_VECTOR, ipl2) Timer1Handler(void) 
{
    mT1ClearIntFlag(); //Limpiar la bandera de interrupcion
	
    
    if (PORTEbits.RE0) 
    {
        contDC0++;
        PORTDbits.RD11 = 0;
    }
    else
    {
        contDC0 = 0;
        PORTDbits.RD11 = 1;
    }

    if (PORTEbits.RE0 && contDC0 <= 500)
    {
        PORTDbits.RD5 = 1;
    }
    else
    {
        PORTDbits.RD5 = 0;
        contDC0 = 0;
    }
    /**/
    if (PORTEbits.RE1) 
    {
        contDC1++;
        PORTDbits.RD7 = 0;
    }
    else
    {
        contDC1 = 0;
        PORTDbits.RD7 = 1;
    }
    
    if (PORTEbits.RE1 && contDC1 <= 500)
    {
        PORTDbits.RD6 = 1;
    }
    else
    {
        PORTDbits.RD6 = 0;
        contDC1 = 0;
    }
    /**/
}

void __ISR(_TIMER_2_VECTOR, ipl3) Timer2Handler(void) 
{
    mT2ClearIntFlag(); //Limpiar la bandera de interrupcion
    
    //OC1RS = servo(An_value0);
    //OC1RS = servo(An_value0);
    OC1RS = speed(An_value0);
    OC2RS = speed(An_value1);
}

void __ISR(_TIMER_3_VECTOR, ipl4) Timer3Handler(void) 
{
    mT3ClearIntFlag(); //Limpiar la bandera de interrupcion
}

void __ISR(_CORE_TIMER_VECTOR, ipl3) CoreTimerHandler(void)
{
    mCTClearIntFlag(); //Limpiar la bandera de interrupcion
    UpdateCoreTimer(CORE_TICK); // Reinicia el CoreTimer
    
    contADC++;

    if (contADC > 49)
    {
        offset = 8 * ((~ReadActiveBufferADC10() & 0x01));
        An_value0 = ReadADC10(offset);
        An_value1 = ReadADC10(offset + 1);
        contADC = 0;
    }
}

void __ISR(_EXTERNAL_0_VECTOR, ipl2) INT0InterruptHandler(void)
{
    mINT0ClearIntFlag();
}