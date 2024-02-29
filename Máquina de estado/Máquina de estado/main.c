#include <stdio.h>
#include <stdlib.h>
#define ESTADO_ERROR        0
#define ESTADO_ABIERTO      1
#define ESTADO_CERRADO      2
#define ESTADO_ABRIENDO     3
#define ESTADO_CERRANDO     4
#define ESTADO_INTERMEDIO   5
#define ESTADO_INIT         6
#define FALSE 0
#define TRUE 1
#define inTrue 0
#define inFalse 1
#define TIME_Ca 60

//Código: Máquina de estados

int Func_ESTADO_ERROR(void);
int Func_ESTADO_ABIERTO(void);
int Func_ESTADO_CERRADO(void);
int Func_ESTADO_ABRIENDO(void);
int Func_ESTADO_CERRANDO(void);
int Func_ESTADO_INTERMEDIO(void);
int Func_ESTADO_INIT(void);

volatile int ESTADO_ANTERIOR = ESTADO_INIT;
volatile int ESTADO_ACTUAL = ESTADO_INIT;
volatile int ESTADO_SIGUIENTE = ESTADO_INIT;
volatile int CntTimeCa = 0; //Contador de un segundo

volatile struct INOUT
{
    unsigned int Sc:1;
    unsigned int Sa:1;
    unsigned int Mc:1;
    unsigned int Ma:1;
    unsigned int Bc:1;
    unsigned int Ba:1;
    unsigned int Led:1;
} inout;

int main()
{
    for(;;)
    {
        if(ESTADO_SIGUIENTE == ESTADO_INIT)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_INIT();
        }
        if(ESTADO_SIGUIENTE == ESTADO_ABIERTO)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_ABIERTO();
        }
        if(ESTADO_SIGUIENTE == ESTADO_CERRADO)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_CERRADO();
        }
        if(ESTADO_SIGUIENTE == ESTADO_ABRIENDO)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_ABRIENDO();
        }
        if(ESTADO_SIGUIENTE == ESTADO_CERRANDO)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_CERRANDO();
        }
        if(ESTADO_SIGUIENTE == ESTADO_INTERMEDIO)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_INTERMEDIO();
        }
        if(ESTADO_SIGUIENTE == ESTADO_ERROR)
        {
            ESTADO_SIGUIENTE = Func_ESTADO_ERROR();
        }
    }
}

int Func_ESTADO_ERROR(void)
{
    for(;;)
    {
        inout.Led = TRUE;
    }
}

int Func_ESTADO_ABIERTO(void)
{
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_ABIERTO;
    inout.Ma = FALSE;
    inout.Mc = FALSE;
    inout.Led = FALSE;
    CntTimeCa = 0;//reset de contador

    for(;;)
    {
        if(CntTimeCa > TIME_Ca)
        {
            return ESTADO_CERRANDO;
        }

        if(inout.Bc == TRUE)
        {
            return ESTADO_CERRANDO;
        }
    }
}

int Func_ESTADO_CERRADO(void)
{
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_CERRADO;
    inout.Mc = FALSE;
    inout.Ma = FALSE;

    for(;;)
    {
        if(inout.Ba == TRUE)
        {
            return ESTADO_ABRIENDO;
        }
    }
}

int Func_ESTADO_ABRIENDO(void)
{
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_ABRIENDO;
    inout.Mc = FALSE;
    inout.Ma = TRUE;

    for(;;)
    {
        if(inout.Sa == TRUE)
        {
            return ESTADO_ABIERTO;
        }
    }
}

int Func_ESTADO_CERRANDO(void)
{
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_CERRANDO;
    inout.Mc = TRUE;
    inout.Ma = FALSE;

    for(;;)
    {
        if(inout.Sc == TRUE)
        {
            return ESTADO_CERRADO;
        }
    }
}

int Func_ESTADO_INTERMEDIO(void)
{
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_INTERMEDIO;
    inout.Mc = FALSE;
    inout.Ma = FALSE;

    for(;;)
    {
        if(inout.Bc == TRUE)
        {
           return ESTADO_CERRANDO;
        }
        if(inout.Ba == TRUE)
        {
           return ESTADO_ABRIENDO;
        }
    }
}

int Func_ESTADO_INIT(void)
{
    ESTADO_ANTERIOR = ESTADO_ACTUAL;
    ESTADO_ACTUAL = ESTADO_INIT;

    inout.Ma = FALSE;
    inout.Mc = FALSE;

    if((inout.Sa == inTrue) && (intout.Sc == inTrue))
    {
        return ESTADO_ERROR;
    }
    if(inout.Sa == inTrue)
    {
        return ESTADO_ABIERTO;
    }
    if(inout.Sc == inTrue)
    {
        return ESTADO_CERRADO;
    }
    if((inout.Sa == inFalse) && (intout.Sc == inFalse))
    {
        return ESTADO_INTERMEDIO;
    }
}

void intGeneral100ms(void)
{
    static int cont_int = 0;
    cont_int++;

    if(cont_int == 9)
    {
        CntTimeCa++;
        cont_int = 0;
    }

    if(HAL_GPIO_Read(PUERTO, PIN_Bc))//leg
    {
        inout.Bc = TRUE;
    }else
    {
        inout.Bc = FALSE;
    }
    if(HAL_GPIO_Read(PUERTO, PIN_Ba))//leg
    {
        inout.Ba = TRUE;
    }else
    {
        inout.Ba = FALSE;
    }
    if(HAL_GPIO_Read(PUERTO, PIN_Sc))//leg
    {
        inout.Sc = TRUE;
    }else
    {
        inout.Sc = FALSE;
    }
    if(HAL_GPIO_Read(PUERTO, PIN_Sa))//leg
    {
        inout.Sa = TRUE;
    }else
    {
        inout.Sa = FALSE;
    }
    if(inout.Ma == TRUE)//leg
    {
        HAL_GPIO_Write(PUERTO, PIN_Ma,1);
    }else
    {
        HAL_GPIO_Write(PUERTO, PIN_Ma,0);
    }
    if(inout.Mc == TRUE)//leg
    {
        HAL_GPIO_Write(PUERTO, PIN_Mc,1);
    }else
    {
        HAL_GPIO_Write(PUERTO, PIN_Mc,0);
    }
}
