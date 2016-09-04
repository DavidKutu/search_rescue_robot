#include <xc.h> 
#include <plib.h> 
#include <math.h> 
   
//Configuration 
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPLLMUL = MUL_20 //Oscilador de 
cristal > 4MHZ,Oscilador primario con PLL,Multiplicador de frecuencia x 20 
#pragma config FPLLIDIV = DIV_2, FPLLODIV = DIV_1 // //Divisor de entrada 1:2, Divisor 
de salida 1:1 
#pragma config FSOSCEN = OFF, FPBDIV = DIV_1 //Oscilador externo secundario 
off,PBCLK (PeripheralBusClock) igual a SYSCLK 
#pragma config FWDTEN = OFF, CP = OFF, FCKSM = CSECMD //Watch Dog off, Codigo 
de Programa off 
   
#define SYS_FREQ        (80000000L) 
#define PRESCALER       64 
#define T1_TICK         1250 //T1_TICK = (0.001 / (T_PBCLK * PRESCALER)) = 1 ms 
#define CORE_TICK       40000 //CORE_TICK= (0.001 / (T_SYS_FREQ * 2)) = 1 ms 
#define PWM_FREQ        50 
   
float PWM_PERIOD = (SYS_FREQ / (PWM_FREQ * PRESCALER)) ­ 1; 
float pi = 3.14159265359; 
unsigned int offset = 100000;   //100000 para piso 
                                //150000 para zacate 
  
unsigned long dis0, dis1, dis2, dis3, dis4, dis5; 
unsigned int contServo = 0; 
  
void inicializa () { 
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_64, T1_TICK); //Configuracion del 
Timer1 para generar interrupcion cada 1 ms 
    OpenCoreTimer(CORE_TICK); // 1 ms 
    OpenOC1(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0); // 
Configuracion de OutputCompare1 para generar salidas PWM 
    OpenOC2(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0); // 
Configuracion de OutputCompare2 para generar salidas PWM 
    OpenTimer2(T2_ON | T2_PS_1_64, PWM_PERIOD); //Configuracion del Timer2 para la 
generacion del DutyCycle 
    //Configuracion de Niveles de Prioridad 
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_2); 
    INTSetVectorPriority(INT_CORE_TIMER_VECTOR, INT_PRIORITY_LEVEL_3);  
    //Habilitar interrupciones de Timer1,CoreTimer,Timer2 y Timer3 
    INTEnable(INT_T1, INT_ENABLED); 
    INTEnable(INT_CT, INT_ENABLED); 
    INTEnableSystemMultiVectoredInt();} 
  
void delay (int x) { 
    while (x > 0) 
        x­­;} 
  
void pulseOut (int x){ 
    switch (x) 
    { 
        case 0 :                //sensor delantero 
            PORTDbits.RD2 = 1; 
            delay(100); 
            PORTDbits.RD2 = 0; 
            delay(100); 
            break; 
  
        case 1 :                //sensor izq adelante 
            PORTDbits.RD3 = 1; 
            delay(100); 
            PORTDbits.RD3 = 0; 
            delay(100); 
            break; 
  
        case 3 :                //sensor izq atras 
            PORTDbits.RD4 = 1; 
            delay(100); 
            PORTDbits.RD4 = 0; 
            delay(100); 
            break; 
  
        case 2 :                //sensor der adelante 
            PORTDbits.RD8 = 1; 
            delay(100); 
            PORTDbits.RD8 = 0; 
            delay(100); 
            break; 
  
        case 4 :                //sensor der atras 
            PORTDbits.RD9 = 1; 
            delay(100); 
            PORTDbits.RD9 = 0; 
            delay(100); 
            break;  
  
        case 5 :                //sensor trasero 
            PORTDbits.RD10 = 1; 
            delay(100); 
            PORTDbits.RD10 = 0; 
            delay(100); 
            break; 
    }} 
unsigned long pulseIn (int x) { 
    unsigned long width = 0; 
   
    switch (x) 
    { 
        case 0 :                //sensor delantero 
            // wait for any previous pulse to end 
            while (PORTEbits.RE0 == 1)  {} 
   
            // wait for the pulse to start 
            while (PORTEbits.RE0 != 1)  {} 
   
            // wait for the pulse to stop 
            while (PORTEbits.RE0 == 1) 
                width++; 
   
            return width; 
            break; 
  
        case 1 :                //sensor izq adelante 
            // wait for any previous pulse to end 
            while (PORTEbits.RE1 == 1)  {} 
   
            // wait for the pulse to start 
            while (PORTEbits.RE1 != 1)  {} 
   
            // wait for the pulse to stop 
            while (PORTEbits.RE1 == 1) 
                width++; 
   
            return width; 
            break; 
  
        case 3 :                //sensor izq atras 
            // wait for any previous pulse to end 
            while (PORTEbits.RE3 == 1)  {} 
   
            // wait for the pulse to start  
            while (PORTEbits.RE3 != 1)  {} 
   
            // wait for the pulse to stop 
            while (PORTEbits.RE3 == 1) 
                width++; 
   
            return width; 
            break; 
  
        case 2 :                //sensor der adelante 
            // wait for any previous pulse to end 
            while (PORTEbits.RE2 == 1)  {} 
   
            // wait for the pulse to start 
            while (PORTEbits.RE2 != 1)  {} 
   
            // wait for the pulse to stop 
            while (PORTEbits.RE2 == 1) 
                width++; 
   
            return width; 
            break; 
  
        case 4 :                //sensor der atras 
            // wait for any previous pulse to end 
            while (PORTEbits.RE4 == 1)  {} 
   
            // wait for the pulse to start 
            while (PORTEbits.RE4 != 1)  {} 
   
            // wait for the pulse to stop 
            while (PORTEbits.RE4 == 1) 
                width++; 
   
            return width; 
            break; 
  
        case 5 :                //sensor trasero 
            // wait for any previous pulse to end 
            while (PORTEbits.RE5 == 1)  {} 
   
            // wait for the pulse to start 
            while (PORTEbits.RE5 != 1)  {} 
   
            // wait for the pulse to stop 
            while (PORTEbits.RE5 == 1)  
                width++; 
   
            return width; 
            break; 
    }} 
  
///Gripper 
void close_Gripper() { 
    OC1RS = 300; 
} 
void open_Gripper(){  
    OC1RS = 1100; 
} 
void down_Gripper(){ 
    OC2RS = 800; 
} 
void up_Gripper(){ 
    OC2RS = 300; 
}  
 
void moveFwd (int cm) { 
    PORTDbits.RD11 = 1; 
    delay(5); 
    PORTDbits.RD7 = 1; 
    delay (cm*offset); 
   
    PORTDbits.RD11 = 0; 
    delay(5); 
    PORTDbits.RD7 = 0; 
    delay(5);}  
void moveBack (int cm) { 
    PORTDbits.RD5 = 1; 
    delay(5); 
    PORTDbits.RD6 = 1; 
    delay(cm*offset); 
  
    PORTDbits.RD5 = 0; 
    delay(5); 
    PORTDbits.RD6 = 0; 
    delay(5);} 
  
void rotD(int a){ 
    double grados = a*pi*8/180; 
    PORTDbits.RD11 = 1; 
    delay(5); 
    PORTDbits.RD6 = 1;  
    delay(grados*offset); 
    PORTDbits.RD11 = 0; 
    delay(5); 
    PORTDbits.RD6 = 0; 
    delay(5);} //rotar con decimal 
void rotI(int a){ 
    double grados = a*pi*8/180; 
    PORTDbits.RD7 = 1; 
    delay(5); 
    PORTDbits.RD5 = 1; 
    delay(grados*offset); 
    PORTDbits.RD7 = 0; 
    delay(5); 
    PORTDbits.RD5 = 0; 
    delay(5);} 
 
void rotClk (int deg) { 
    int cm = deg*pi*8/180; 
    PORTDbits.RD11 = 1; 
    delay(5); 
    PORTDbits.RD6 = 1; 
    delay(cm*offset); 
  
    PORTDbits.RD11 = 0; 
    delay(5); 
    PORTDbits.RD6 = 0; 
    delay(5);}  //rotar entero 
void rotCount (int deg) { 
    int cm = deg*pi*8/180; 
    PORTDbits.RD7 = 1; 
    delay(5); 
    PORTDbits.RD5 = 1; 
    delay(cm*offset); 
  
    PORTDbits.RD7 = 0; 
    delay(5); 
    PORTDbits.RD5 = 0; 
    delay(5);} 
 
void SeguirPared(int lado);{ 
    //Prendo sensor delantero 
    pulseOut(0); 
    dis0 = pulseIn(0); 
    delay(10000); 
    //Reviso si llegué a la pared para para los motores 
    if(dis0 < 1500){  
        PORTDbits.RD11 = 0; 
        delay(5); 
        PORTDbits.RD7 = 0; 
        delay(5); 
        break; 
    } 
 
    if(lado = 1){ 
        pulseOut(2); 
        dis2 = pulseIn(2); 
        delay(10000); 
        pulseOut(4); 
        dis4 = pulseIn(4); 
        delay(10000); 
        CorrigoDir(dis2, dis4, lado); 
    } 
    else if(lado = 2){ 
        pulseOut(1); 
        dis1 = pulseIn(1); 
        delay(10000); 
        pulseOut(3); 
        dis3 = pulseIn(3); 
        delay(10000); 
        CorrigoDir(dis1, dis3, lado); 
    } 
    else{ //no quiero revisar pared, solamente quiero revisar cuando llegue a topar con algo 
   
    }} // 1 pared derecha 2 pared iquierda 3 solo pared enfrente 
void CorrigoDir(int x1, int x2, int lado); { 
    if(abs(x1­x2)>150){ 
        PORTDbits.RD11 = 0; 
        delay(5); 
        PORTDbits.RD7 = 0; 
        delay(10000); 
        if(x1>x2){ 
            //Roto a la derecha los grados necesarios para quedar paralelo 
            rotD(atan(((x1­x2)/10)*180/pi)); 
        } 
        else if(x2>x1){ 
            rotI(atan(((x1­x2)/10)*180/pi)); 
        } 
    } 
    if(dis2>720||dis2<570){ 
        if(lado = 1){ 
            pulseOut(2); 
            dis2 = pulseIn(2);  
            delay(10000); 
            PosRobot(dis2); 
        } 
        else if(lado = 2){ 
            pulseOut(1); 
            dis1 = pulseIn(1); 
            delay(10000); 
            PosRobot(dis1); 
        } 
    }   
    PORTDbits.RD11 = 1; 
    delay(5); 
    PORTDbits.RD7 = 1; 
    delay(10000);} 
void PosRobot(int x){ 
    if(x<670){ 
        double grado = atan((670­x)/10)*180/pi; 
        delay(1000); 
        rotI(grado); 
        delay(1000); 
        moveFwd(10); 
        delay(1000); 
        rotD(grado); 
        delay(1000); 
    } 
    else if(x>670){ 
        double grado = atan((x­670)/10)*180/pi; 
        delay(1000); 
        rotI(grado); 
        delay(1000); 
        moveFwd(10); 
        delay(1000); 
        rotD(grado); 
        delay(1000); 
    }} 
 
  
 
main() 
{ 
    TRISD = 0x00;   //Puerto D como salidas 
    PORTD = 0x00;   //Inicializa salidas en 0 
    TRISE = 0xFF;   //Puerto E como entradas 
   
    OC1RS = 300;        //Inicializa servos en 0 
    OC2RS = 300;  
   
    init(); 
    int state = 0; 
   
    while (1) 
    { 
        switch (state) 
        { 
            case 0 :    //avanza 38cm  
                moveFwd(38); 
                state = 4; 
                break; 
   
            case 1 :    //vuelta izq de 90 grados 
                rotCount(88, 0); 
                state = 2; 
                break; 
   
            case 2 :    //avanzar sin seguir pared hasta llegar a la pared de enfrente  
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1) 
                { 
                    SeguirPared(3); 
                } 
                state = 3; 
                break; 
   
            case 3 :    //vuelta derecha de 90 grados 
                rotClk(85, 0); 
                moveFwd(30); 
                state = 4; 
                break; 
   
            case 4 :    //avanzar guiado por la pared derecha solamente 
                 PORTDbits.RD11 = 1; 
                 delay(100); 
                 PORTDbits.RD7 = 1; 
                 delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 5;  
                break; 
   
            case 5 :    //abre y baja gripper 
   
                if (contServo >= 1000){ 
                    down_Gripper(); 
                    open_Gripper(); 
                    contServo = 0; 
                    state = 6; 
                } 
                break; 
            case 6 :    //avanza para agarrar gripper 
                if (contServo >= 5000){ 
                    moveFwd(10); 
                    contServo = 0; 
                    state = 7; 
                } 
                break; 
            case 7 :    //agarrar victima 
                if (contServo >= 4000){ 
                    close_Gripper(); 
                    contServo = 0; 
                    state = 8; 
   
                } 
                break; 
            case 8 :    //sube el gripper 
                if (contServo >= 4000){ 
                    up_Gripper(); 
                    state = 9; 
                    contServo = 0; 
                } 
                break; 
            case 9 :    //vuelta en 180 
                rotClk(185, 0); 
                state = 10; 
                break; 
   
            case 10 :   //seguir pared izq luego pared der 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    pulseOut(3); 
                    dis3 = pulseIn(3);  
                    delay(10000); 
                    pulseOut(1); 
                    dis1 = pulseIn(1); 
                    delay(10000); 
                    //Reviso si pase la pared para parar los motores 
                    if(dis3 > 1826){ 
                        PORTDbits.RD11 = 0; 
                        delay(5); 
                        PORTDbits.RD7 = 0; 
                        delay(5); 
                        break; 
                    } 
                    CorrigoDir(dis1, dis3, 2); 
                } 
                while(1){ 
                    SeguirPared(1); 
                } 
   
                state = 11; 
                break; 
   
            case 11 :   
                if (contServo >= 1000){ 
                    open_Gripper(); 
                    contServo = 0; 
                    state = 12; 
                } 
                break; 
   
            case 12: 
                moveBack(15); 
                delay(1000); 
                rotClk(180); 
                delay(1000); 
                state = 13; 
                break; 
 
            case 13: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    pulseOut(3); 
                    dis3 = pulseIn(3); 
                    delay(10000);  
                    pulseOut(1); 
                    dis1 = pulseIn(1); 
                    delay(10000); 
                    //Reviso si pase la pared para parar los motores 
                    if(dis3 > 1826){ 
                        PORTDbits.RD11 = 0; 
                        delay(5); 
                        PORTDbits.RD7 = 0; 
                        delay(5); 
                        break; 
                    } 
                    CorrigoDir(dis1, dis3, 2); 
                } 
                state = 14; 
                break; 
 
            case 14:  
                rotCount(88); 
                delay(1000); 
                moveFwd(35); 
                delay(1000); 
                rotCount(88); 
                delay(1000); 
                state = 15; 
                break; 
 
            case 15:  
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                state = 16; 
                break; 
 
            case 16: 
                if (contServo >= 1000){ 
                    down_Gripper(); 
                    open_Gripper(); 
                    contServo = 0; 
                    state = 17; 
                } 
                break; 
            case 17 :    //avanza para agarrar gripper  
                if (contServo >= 5000){ 
                    moveFwd(10); 
                    contServo = 0; 
                    state = 18; 
                } 
                break; 
            case 18 :    //agarrar victima 
                if (contServo >= 4000){ 
                    close_Gripper(); 
                    contServo = 0; 
                    state = 19; 
   
                } 
                break; 
            case 19 :    //sube el gripper 
                if (contServo >= 4000){ 
                    up_Gripper(); 
                    contServo = 0; 
                    state = 20; 
                } 
                break; 
            case 20 :    //vuelta en 180 
                rotClk(185, 0); 
                state = 21; 
                break; 
 
            case 21:  
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    pulseOut(3); 
                    dis3 = pulseIn(3); 
                    delay(10000); 
                    pulseOut(1); 
                    dis1 = pulseIn(1); 
                    delay(10000); 
                    //Reviso si pase la pared para parar los motores 
                    if(dis3 > 1826){ 
                        PORTDbits.RD11 = 0; 
                        delay(5); 
                        PORTDbits.RD7 = 0; 
                        delay(5); 
                        break; 
                    }  
                    CorrigoDir(dis1, dis3, 2); 
                } 
                state = 22; 
                break; 
 
            case 22: 
                rotClk(88); 
                delay(1000); 
                moveFwd(35); 
                delay(1000); 
                rotClk(88); 
                delay(1000); 
                state = 23; 
                break; 
 
            case 23:  
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 5; 
                break; 
 
            case 24: 
                if (contServo >= 1000){ 
                    open_Gripper(); 
                    contServo = 0; 
                    state = 25; 
                } 
                break; 
   
            case 25: 
                moveBack(15); 
                delay(1000); 
                rotClk(180); 
                delay(1000); 
                state = 26; 
                break; 
 
            case 26:  
                PORTDbits.RD11 = 1; 
                delay(100);  
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    pulseOut(3); 
                    dis3 = pulseIn(3); 
                    delay(10000); 
                    pulseOut(1); 
                    dis1 = pulseIn(1); 
                    delay(10000); 
                    //Reviso si pase la pared para parar los motores 
                    if(dis3 > 1826){ 
                        PORTDbits.RD11 = 0; 
                        delay(5); 
                        PORTDbits.RD7 = 0; 
                        delay(5); 
                        break; 
                    } 
                    CorrigoDir(dis1, dis3, 2); 
                } 
                state = 27; 
                break; 
 
            case 27:  
                rotCount(88); 
                delay(1000); 
                moveFwd(70); 
                delay(1000); 
                rotCount(88); 
                delay(1000); 
                state = 28; 
                break; 
 
            case 28:  
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(2); 
                } 
                state = 29; 
                break;  
 
            case 29: 
 
                if (contServo >= 1000){  
                    down_Gripper(); 
                    open_Gripper(); 
                    contServo = 0; 
                    state = 30; 
                } 
                break; 
            case 30 :    //avanza para agarrar gripper 
                if (contServo >= 5000){ 
                    moveFwd(10); 
                    contServo = 0; 
                    state = 31; 
                } 
                break; 
            case 31 :    //agarrar victima 
                if (contServo >= 4000){ 
                    close_Gripper(); 
                    contServo = 0; 
                    state = 32; 
   
                } 
                break; 
            case 32 :    //sube el gripper 
                if (contServo >= 4000){ 
                    up_Gripper(); 
                    state = 33; 
                    contServo = 0; 
                } 
                break; 
            case 33 :    //vuelta en 180 
                rotClk(185, 0); 
                state = 34; 
                break; 
 
            case 34:  
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    pulseOut(4); 
                    dis4 = pulseIn(4); 
                    delay(10000); 
                    pulseOut(2); 
                    dis2 = pulseIn(2); 
                    delay(10000); 
                    //Reviso si pase la pared para parar los motores  
                    if(dis4 > 1826){ 
                        PORTDbits.RD11 = 0; 
                        delay(5); 
                        PORTDbits.RD7 = 0; 
                        delay(5); 
                        break; 
                    } 
                    CorrigoDir(dis2, dis4, 1); 
                } 
                state = 35; 
                break; 
 
            case 35: 
                rotClk(88); 
                delay(1000); 
                moveFwd(70); 
                delay(1000); 
                rotClk(88); 
                delay(1000); 
                state = 36; 
                break; 
 
            case 36:  
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 37; 
                break; 
 
            case 37: 
                if (contServo >= 1000){ 
                    open_Gripper(); 
                    contServo = 0; 
                    state = 38; 
                } 
                break; 
   
            case 38: 
                moveBack(15); 
                delay(1000); 
                rotClk(180);  
                delay(1000); 
                state = 39; 
                break; 
 
            case 39: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 40; 
                break; 
 
            case 40: 
                rotClk(185, 0); 
                state = 41; 
                break; 
   
            case 41 :   //seguir pared izq luego pared der 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    pulseOut(4); 
                    dis4 = pulseIn(4); 
                    delay(10000); 
                    pulseOut(2); 
                    dis2 = pulseIn(2); 
                    delay(10000); 
                    //Reviso si pase la pared para parar los motores 
                    if(dis4 > 1826){ 
                        PORTDbits.RD11 = 0; 
                        delay(5); 
                        PORTDbits.RD7 = 0; 
                        delay(5); 
                        break; 
                    } 
                    CorrigoDir(dis2, dis4, 1); 
                } 
   
                state = 42; 
                break;  
 
            case 42: 
                rotClk(88); 
                delay(1000); 
                moveFwd(70); 
                delay(1000); 
                rotClk(88); 
                delay(1000); 
                state = 43; 
                break; 
            case 43: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 44; 
                break; 
            case 44: 
                rotCount(88); 
                delay(1000); 
                state = 45; 
                break; 
 
            case 45: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 46; 
                break; 
 
            case 46: 
 
                if (contServo >= 1000){ 
                    down_Gripper(); 
                    open_Gripper(); 
                    contServo = 0; 
                    state = 47;  
                } 
                break; 
            case 47 :    //avanza para agarrar gripper 
                if (contServo >= 5000){ 
                    moveFwd(10); 
                    contServo = 0; 
                    state = 48; 
                } 
                break; 
            case 48 :    //agarrar victima 
                if (contServo >= 4000){ 
                    close_Gripper(); 
                    contServo = 0; 
                    state = 49; 
   
                } 
                break; 
            case 49 :    //sube el gripper 
                if (contServo >= 4000){ 
                    up_Gripper(); 
                    state = 50; 
                    contServo = 0; 
                } 
                break; 
            case 50 :    //vuelta en 180 
                rotClk(185, 0); 
                state = 51; 
                break; 
            case 51: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(2); 
                } 
                contServo =0; 
                state = 52; 
                break; 
            case 52: 
                rotClk(88); 
                delay(1000); 
                moveFwd(35); 
                delay(1000); 
                rotCount(88); 
                delay(1000);  
                state = 53; 
                break; 
            case 53: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1) 
                { 
                    SeguirPared(3); 
                } 
                state = 54; 
                break; 
            case 54: 
                rotClk(88); 
                delay(1000); 
                state = 55; 
                break; 
 
            case 55: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    pulseOut(3); 
                    dis3 = pulseIn(3); 
                    delay(10000); 
                    pulseOut(1); 
                    dis1 = pulseIn(1); 
                    delay(10000); 
                    //Reviso si pase la pared para parar los motores 
                    if(dis3 > 1826){ 
                        PORTDbits.RD11 = 0; 
                        delay(5); 
                        PORTDbits.RD7 = 0; 
                        delay(5); 
                        break; 
                    } 
                    CorrigoDir(dis1, dis3, 2); 
                } 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while(1){  
                    SeguirPared(1); 
                } 
   
                state = 56; 
                break; 
   
            case 56 :   
                if (contServo >= 1000){ 
                    open_Gripper(); 
                    contServo = 0; 
                    state = 57; 
                } 
                break; 
   
            case 57: 
                moveBack(15); 
                delay(1000); 
                rotClk(180); 
                delay(1000); 
                state = 58; 
                break; 
            case 58: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 59; 
                break; 
 
            case 59: 
                rotClk(185, 0); 
                state = 60; 
                break; 
   
            case 60 :   //seguir pared izq luego pared der 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    pulseOut(4); 
                    dis4 = pulseIn(4);  
                    delay(10000); 
                    pulseOut(2); 
                    dis2 = pulseIn(2); 
                    delay(10000); 
                    //Reviso si pase la pared para parar los motores 
                    if(dis4 > 1826){ 
                        PORTDbits.RD11 = 0; 
                        delay(5); 
                        PORTDbits.RD7 = 0; 
                        delay(5); 
                        break; 
                    } 
                    CorrigoDir(dis2, dis4, 1); 
                } 
   
                state = 61; 
                break; 
 
            case 61: 
                rotClk(88); 
                delay(1000); 
                moveFwd(70); 
                delay(1000); 
                rotClk(88); 
                delay(1000); 
                state = 62; 
                break; 
            case 62: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 63; 
                break; 
            case 63: 
                rotCount(88); 
                delay(1000); 
                state = 64; 
                break; 
 
            case 64: 
                PORTDbits.RD11 = 1;  
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 65; 
                break; 
 
            case 65:  
                rotCount(88); 
                delay(1000); 
                state = 66; 
                break; 
            case 66:  
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 67; 
                break; 
            case 67: 
                if (contServo >= 1000){ 
                    down_Gripper(); 
                    open_Gripper(); 
                    contServo = 0; 
                    state = 68; 
                } 
                break; 
            case 68 :    //avanza para agarrar gripper 
                if (contServo >= 5000){ 
                    moveFwd(10); 
                    contServo = 0; 
                    state = 69; 
                } 
                break; 
            case 69 :    //agarrar victima 
                if (contServo >= 4000){ 
                    close_Gripper(); 
                    contServo = 0; 
                    state = 70;  
   
                } 
                break; 
            case 70 :    //sube el gripper 
                if (contServo >= 4000){ 
                    up_Gripper(); 
                    state = 71; 
                    contServo = 0; 
                } 
                break; 
            case 71 :    //vuelta en 180 
                rotClk(185, 0); 
                state = 72; 
                break; 
            case 72: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(2); 
                } 
                contServo =0; 
                state = 73; 
                break; 
 
            case 73:  
                rotClk(88); 
                delay(1000); 
                state = 74; 
                break; 
            case 74: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(2); 
                } 
                contServo =0; 
                state = 75; 
                break; 
            case 75: 
                rotClk(88); 
                delay(1000); 
                moveFwd(35);  
                delay(1000); 
                rotCount(88); 
                delay(1000); 
                state = 76; 
                break; 
            case 76: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1) 
                { 
                    SeguirPared(3); 
                } 
                state = 77; 
                break; 
            case 77: 
                rotClk(88); 
                delay(1000); 
                state = 78; 
                break; 
 
            case 78: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    pulseOut(3); 
                    dis3 = pulseIn(3); 
                    delay(10000); 
                    pulseOut(1); 
                    dis1 = pulseIn(1); 
                    delay(10000); 
                    //Reviso si pase la pared para parar los motores 
                    if(dis3 > 1826){ 
                        PORTDbits.RD11 = 0; 
                        delay(5); 
                        PORTDbits.RD7 = 0; 
                        delay(5); 
                        break; 
                    } 
                    CorrigoDir(dis1, dis3, 2); 
                } 
                PORTDbits.RD11 = 1; 
                delay(100);  
                PORTDbits.RD7 = 1; 
                delay(100); 
                while(1){ 
                    SeguirPared(1); 
                } 
   
                state = 79; 
                break; 
   
            case 79 :   
                if (contServo >= 1000){ 
                    open_Gripper(); 
                    contServo = 0; 
                    state = 80; 
                } 
                break; 
   
            case 80: 
                moveBack(15); 
                delay(1000); 
                rotClk(180); 
                delay(1000); 
                state = 81; 
                break; 
            case 81: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 82; 
                break; 
 
            case 82: 
                rotClk(185, 0); 
                state = 83; 
                break; 
   
            case 83 :   //seguir pared izq luego pared der 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100);  
                while (1){ 
                    pulseOut(4); 
                    dis4 = pulseIn(4); 
                    delay(10000); 
                    pulseOut(2); 
                    dis2 = pulseIn(2); 
                    delay(10000); 
                    //Reviso si pase la pared para parar los motores 
                    if(dis4 > 1826){ 
                        PORTDbits.RD11 = 0; 
                        delay(5); 
                        PORTDbits.RD7 = 0; 
                        delay(5); 
                        break; 
                    } 
                    CorrigoDir(dis2, dis4, 1); 
                } 
   
                state = 84; 
                break; 
 
            case 84: 
                rotClk(88); 
                delay(1000); 
                moveFwd(70); 
                delay(1000); 
                rotClk(88); 
                delay(1000); 
                state = 85; 
                break; 
            case 85: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 86; 
                break; 
            case 86: 
                rotCount(88); 
                delay(1000); 
                state = 87; 
                break;  
 
            case 87: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 88; 
                break; 
 
            case 88:  
                rotCount(88); 
                delay(1000); 
                state = 89; 
                break; 
            case 89:  
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 90; 
                break; 
            case 90:  
                rotCount(88); 
                delay(1000); 
                state = 91; 
                break; 
            case 91:  
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(1); 
                } 
                contServo =0; 
                state = 92; 
                break;   
            case 92:  
                if (contServo >= 1000){ 
                    down_Gripper(); 
                    open_Gripper(); 
                    contServo = 0; 
                    state = 93; 
                } 
                break; 
            case 93 :    //avanza para agarrar gripper 
                if (contServo >= 5000){ 
                    moveFwd(10); 
                    contServo = 0; 
                    state = 94; 
                } 
                break; 
            case 94 :    //agarrar victima 
                if (contServo >= 4000){ 
                    close_Gripper(); 
                    contServo = 0; 
                    state = 95; 
   
                } 
                break; 
            case 95 :    //sube el gripper 
                if (contServo >= 4000){ 
                    up_Gripper(); 
                    state = 96; 
                    contServo = 0; 
                } 
                break; 
            case 96 :    //vuelta en 180 
                rotClk(185, 0); 
                state = 97; 
                break; 
            case 97:  
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(2); 
                } 
                contServo =0; 
                state = 98; 
                break;   
            case 98:  
                rotClk(88);  
                delay(1000); 
                state = 99; 
                break; 
            case 99: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(2); 
                } 
                contServo =0; 
                state = 100; 
                break; 
 
            case 100:  
                rotClk(88); 
                delay(1000); 
                state = 101; 
                break; 
            case 101: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    SeguirPared(2); 
                } 
                contServo =0; 
                state = 102; 
                break; 
            case 102: 
                rotClk(88); 
                delay(1000); 
                moveFwd(35); 
                delay(1000); 
                rotCount(88); 
                delay(1000); 
                state = 103; 
                break; 
            case 103: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1)  
                { 
                    SeguirPared(3); 
                } 
                state = 104; 
                break; 
            case 104: 
                rotClk(88); 
                delay(1000); 
                state = 105; 
                break; 
 
            case 105: 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    pulseOut(3); 
                    dis3 = pulseIn(3); 
                    delay(10000); 
                    pulseOut(1); 
                    dis1 = pulseIn(1); 
                    delay(10000); 
                    //Reviso si pase la pared para parar los motores 
                    if(dis3 > 1826){ 
                        PORTDbits.RD11 = 0; 
                        delay(5); 
                        PORTDbits.RD7 = 0; 
                        delay(5); 
                        break; 
                    } 
                    CorrigoDir(dis1, dis3, 2); 
                } 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while(1){ 
                    SeguirPared(1); 
                } 
   
                state = 106; 
                break; 
   
            case 106 :   
                if (contServo >= 1000){  
                    open_Gripper(); 
                    contServo = 0; 
                    state = 106; 
                } 
                break; 
   
            case 107: 
                moveBack(15); 
                delay(1000); 
                rotClk(180); 
                delay(1000); 
                state = 108; 
                break; 
            case 108 :   //seguir pared izq luego pared der 
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1){ 
                    pulseOut(4); 
                    dis4 = pulseIn(4); 
                    delay(10000); 
                    pulseOut(2); 
                    dis2 = pulseIn(2); 
                    delay(10000); 
                    //Reviso si pase la pared para parar los motores 
                    if(dis4 > 1826){ 
                        PORTDbits.RD11 = 0; 
                        delay(5); 
                        PORTDbits.RD7 = 0; 
                        delay(5); 
                        break; 
                    } 
                    CorrigoDir(dis2, dis4, 1); 
                } 
   
                state = 109; 
                break; 
            case 109: 
                rotClk(88); 
                delay(1000); 
                moveFwd(35); 
                delay(1000); 
                rotClk(88); 
                delay(1000); 
                state = 110;  
                break; 
            case 110 :    //avanzar sin seguir pared hasta llegar a la pared de enfrente  
                PORTDbits.RD11 = 1; 
                delay(100); 
                PORTDbits.RD7 = 1; 
                delay(100); 
                while (1) 
                { 
                    SeguirPared(3); 
                } 
                state = 999; 
                break; 
 
 
            default: 
                state = 999; 
                break; 
        } 
    } 
} 
/**/ 
void __ISR(_TIMER_1_VECTOR, ipl2) Timer1Handler(void)  
{ 
    INTDisableInterrupts();         //Disable interrupts 
    mT1ClearIntFlag();              //Limpiar la bandera de interrupcion 
  
    INTEnableInterrupts();          //Enables interrupts again 
} 
/**/ 
  
void __ISR(_CORE_TIMER_VECTOR, ipl3) CoreTimerHandler(void) 
{ 
    INTDisableInterrupts();         //Disable interrupts 
    mCTClearIntFlag();              //Limpiar la bandera de interrupcion 
    UpdateCoreTimer(CORE_TICK);     // Reinicia el CoreTimer 
    contServo++; 
   
    if (contServo == 20000){ 
        contServo = 0; 
    } 
    INTEnableInterrupts();          //Enables interrupts again 
} 