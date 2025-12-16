/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   CEcoTaskScheduler1Lab_C761620F
 * </сводка>
 *
 * <описание>
 *   Данный исходный код описывает реализацию интерфейсов CEcoTaskScheduler1Lab_C761620F
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoTaskScheduler1Lab.h"
#include "CEcoTask1Lab.h"

/* Выделяем память под один экземпляр */
CEcoTaskScheduler1Lab_C761620F g_xCEcoTaskScheduler1Lab_C761620F = {0};

/* Резервируем область под статические задачи */
#define MAX_STATIC_TASK_COUNT   10
CEcoTask1Lab_C761620F g_xCEcoTask1List_C761620F[MAX_STATIC_TASK_COUNT] = {0};

/* Резервируем область под стеки статических задач */
#define MAX_STATIC_STACK_TASK_COUNT   4096 * MAX_STATIC_TASK_COUNT
__attribute__((aligned(16))) 
uint64_t g_xCEcoStackTask1List_C761620F[MAX_STATIC_STACK_TASK_COUNT] = {0};

/* Указатель на текущую выполняемую задачу */
volatile CEcoTask1Lab_C761620F* g_pCurrentTask = 0;
static CEcoTask1Lab_C761620F* g_pNextTask = 0;

/* Контекст */
uint64_t * volatile g_pxCurrentTCB_C761620F = 0;

uint64_t g_indx = 0;

void CEcoTaskScheduler1Lab_C761620F_ChangeTask(void);

/*
 *
 * <сводка>
 *   Функция TaskSwitchContext
 * </сводка>
 *
 * <описание>
 *   Функция 
 * </описание>
 *
 */

/*__attribute__((naked))*/ void CEcoTaskScheduler1Lab_C761620F_TaskSwitchContext( void ) {

        //if (g_pxCurrentTCB_C761620F == (uint64_t*)&g_xCEcoTask1List_C761620F[g_indx] && g_xCEcoTask1List_C761620F[g_indx].pfunc != 0) {
            g_indx++;
            if (g_indx > 1) {
                g_indx = 0;
            }

          //  g_pxCurrentTCB_C761620F = (uint64_t*)&g_xCEcoTask1List_C761620F[g_indx];
        //}

   // g_xCEcoTaskScheduler1Lab_C761620F.m_pIArmTimer->pVTbl->Reset(g_xCEcoTaskScheduler1Lab_C761620F.m_pIArmTimer);
    g_xCEcoTask1List_C761620F[g_indx].pfunc();
}

/*
 *
 * <сводка>
 *   Функция TimerHandler
 * </сводка>
 *
 * <описание>
 *   Функция 
 * </описание>
 *
 */
__attribute__((naked))
void CEcoTaskScheduler1Lab_C761620F_TimerHandler(void) {
    __asm volatile (
        "mrs x0, elr_el1 \n\t"
        "mrs x1, spsr_el1 \n\t"
        "stp x0, x1, [sp, #-16]! \n\t"

        "stp x29, x30, [sp, #-16]! \n\t"

        "bl CEcoTaskScheduler1Lab_C761620F_ChangeTask \n\t"

        "ldp x29, x30, [sp], #16 \n\t"

        "ldr x0, =g_pCurrentTask \n\t"
        "ldr x1, [x0] \n\t"
        "ldr x2, =g_pNextTask \n\t"
        "ldr x3, [x2] \n\t"
        
        "cmp x1, x3 \n\t"
        "b.eq 1f \n\t"

        "cbz x1, 2f \n\t"
        "mov x4, sp \n\t"
        "str x4, [x1, #16] \n\t"
        "2: \n\t"

        "str x3, [x0] \n\t"
        "ldr x5, [x3, #16] \n\t"
        "mov sp, x5 \n\t"

        "ldp x0, x1, [sp], #16 \n\t"
        "msr elr_el1, x0 \n\t"
        "msr spsr_el1, x1 \n\t"

        "ldp x0, x1, [sp], #16 \n\t"   

        "ldp x1, x2, [sp], #16 \n\t"
        "ldp x3, x4, [sp], #16 \n\t"
        "ldp x5, x6, [sp], #16 \n\t"
        "ldp x7, x8, [sp], #16 \n\t"
        "ldp x9, x10, [sp], #16 \n\t"
        "ldp x11, x12, [sp], #16 \n\t"
        "ldp x13, x14, [sp], #16 \n\t"
        "ldp x15, x16, [sp], #16 \n\t"
        "ldp x17, x18, [sp], #16 \n\t"
        "ldp x19, x20, [sp], #16 \n\t"
        "ldp x21, x22, [sp], #16 \n\t"
        "ldp x23, x24, [sp], #16 \n\t"
        "ldp x25, x26, [sp], #16 \n\t"
        "ldp x27, x28, [sp], #16 \n\t"
        "ldp x29, x30, [sp], #16 \n\t"
        
        "eret \n\t"

        "1: \n\t"
        "ldp x0, x1, [sp], #16 \n\t"
        "msr elr_el1, x0 \n\t"
        "msr spsr_el1, x1 \n\t"
        "ret \n\t"
        : 
        : 
        : "memory"
    );
}

void CEcoTaskScheduler1Lab_C761620F_ChangeTask(void) {
    int i = 0;
    uint32_t maxTime = 0;
    
    if (g_pCurrentTask != 0 && g_pCurrentTask->m_bCompleted == 0) {
        if (g_pCurrentTask->m_uRemainingTime > 0) {
            g_pCurrentTask->m_uRemainingTime--;
        }
        if (g_pCurrentTask->m_uRemainingTime == 0) {
            g_pCurrentTask->m_bCompleted = 1;
        }
    }

    if (g_pCurrentTask != 0 && g_pCurrentTask->m_bCompleted == 0) {
        g_pNextTask = (CEcoTask1Lab_C761620F*)g_pCurrentTask;
        return; 
    }

    g_pNextTask = 0;
    for(i = 0; i < MAX_STATIC_TASK_COUNT; i++) {
        if (g_xCEcoTask1List_C761620F[i].pfunc != 0 && 
            g_xCEcoTask1List_C761620F[i].m_bCompleted == 0 && 
            g_xCEcoTask1List_C761620F[i].m_uRemainingTime > maxTime) {
            maxTime = g_xCEcoTask1List_C761620F[i].m_uRemainingTime;
            g_pNextTask = &g_xCEcoTask1List_C761620F[i];
        }
    }

    if (g_pNextTask != 0) 
        return;
    g_pNextTask = g_pCurrentTask != 0 ? (CEcoTask1Lab_C761620F*)g_pCurrentTask : &g_xCEcoTask1List_C761620F[0];
    
}

/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция QueryInterface для интерфейса IEcoTaskScheduler1Lab
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_QueryInterface(/* in */ IEcoTaskScheduler1Ptr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;

    /* Проверка указателей */
    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoTaskScheduler1) ) {
        *ppv = &pCMe->m_pVTblIScheduler;
        pCMe->m_pVTblIScheduler->AddRef((IEcoTaskScheduler1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIScheduler;
        pCMe->m_pVTblIScheduler->AddRef((IEcoTaskScheduler1*)pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    return ERR_ECO_SUCCESES;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoTaskScheduler1Lab
 * </описание>
 *
 */
static uint32_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_AddRef(/* in */ IEcoTaskScheduler1Ptr_t me) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;

    /* Проверка указателя */
    if (me == 0 ) {
        return -1;
    }

    return atomicincrement_int32_t(&pCMe->m_cRef);
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoTaskScheduler1Lab
 * </описание>
 *
 */
static uint32_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Release(/* in */ IEcoTaskScheduler1Ptr_t me) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;

    /* Проверка указателя */
    if (me == 0 ) {
        return -1;
    }

    /* Уменьшение счетчика ссылок на компонент */
    atomicdecrement_int32_t(&pCMe->m_cRef);

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if ( pCMe->m_cRef == 0 ) {
        deleteCEcoTaskScheduler1Lab_C761620F((IEcoTaskScheduler1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Init
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Init(/*in*/IEcoTaskScheduler1Ptr_t me, /*in*/ IEcoInterfaceBus1Ptr_t pIBus) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 || pIBus == 0) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция InitWith
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_InitWith(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ IEcoInterfaceBus1Ptr_t pIBus, /*in*/ voidptr_t heapStartAddress, /*in*/ uint32_t size) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    int16_t result = -1;

    /* Проверка указателей */
    if (me == 0 || pIBus == 0) {
        return -1;
    }

    /* Инициализация данных */
    pCMe->m_pTaskList = g_xCEcoTask1List_C761620F;
    pCMe->m_pIBus = pIBus;

    /* Получение интерфейса для работы с программируемым таймером */
    result = pCMe->m_pIBus->pVTbl->QueryComponent(pCMe->m_pIBus, &CID_EcoTimer1, 0, &IID_IEcoTimer1, (void**) &pCMe->m_pIArmTimer);
    /* Проверка */
    if (result != 0 || pCMe->m_pIArmTimer == 0) {
        return result;
    }

    /* Установка обработчика прерывания программируемого таймера */
    pCMe->m_pIArmTimer->pVTbl->set_Interval(pCMe->m_pIArmTimer, 20000);
    pCMe->m_pIArmTimer->pVTbl->set_IrqHandler(pCMe->m_pIArmTimer, (voidptr_t*)CEcoTaskScheduler1Lab_C761620F_TimerHandler);

    return 0;
}

/*
 *
 * <сводка>
 *   Функция NewTask
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_NewTask(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ voidptr_t address, /*in*/ voidptr_t data, /*in*/ uint32_t stackSize, /* out */ IEcoTask1** ppITask) {
    int32_t indx = 0;
    int i = 0;
    uint64_t* pxTopOfStack = 0;
    uint64_t stackAddr = 0;
    uint32_t burstTime = (uint32_t)((uint64_t)data); 
    if (me == 0 ) return -1;
    for (indx = 0; indx < MAX_STATIC_TASK_COUNT; indx++) {
        if (g_xCEcoTask1List_C761620F[indx].pfunc == 0) {
            g_xCEcoTask1List_C761620F[indx].pfunc = address;
            g_xCEcoTask1List_C761620F[indx].m_cRef = 1;
            g_xCEcoTask1List_C761620F[indx].m_uId = indx;
            g_xCEcoTask1List_C761620F[indx].m_uBurstTime = burstTime;
            g_xCEcoTask1List_C761620F[indx].m_uRemainingTime = burstTime;
            g_xCEcoTask1List_C761620F[indx].m_bCompleted = 0;
            pxTopOfStack = (uint64_t*)&g_xCEcoStackTask1List_C761620F[(indx + 1) * (4096 / 8)];
            stackAddr = (uint64_t)pxTopOfStack;
            if (stackAddr & 0xF) { stackAddr -= 8; }
            pxTopOfStack = (uint64_t*)stackAddr;
            pxTopOfStack -= 30; 
            for(i=0; i<30; i++) pxTopOfStack[i] = 0;
            pxTopOfStack -= 2;
            pxTopOfStack[0] = 0;
            pxTopOfStack[1] = 0;
            pxTopOfStack -= 2;
            pxTopOfStack[0] = (uint64_t)address;
            pxTopOfStack[1] = 0x00000005;
            g_xCEcoTask1List_C761620F[indx].m_sp = pxTopOfStack;
            *ppITask = (IEcoTask1*)&g_xCEcoTask1List_C761620F[indx];
            return 0;
        }
    }
    return -1;
}

/*
 *
 * <сводка>
 *   Функция DeleteTask
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_DeleteTask(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ uint16_t taskId) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция SuspendTask
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_SuspendTask(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ uint16_t taskId) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция ResumeTask
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_ResumeTask(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ uint16_t taskId) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция RegisterInterrupt
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_RegisterInterrupt(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ uint16_t number, /*in*/ voidptr_t handlerAddress, /*in*/ int32_t flag) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция UnRegisterInterrupt
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_UnRegisterInterrupt(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ uint16_t number) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Start
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Start(/*in*/ IEcoTaskScheduler1Ptr_t me) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    if (me == 0 ) return -1;

    g_pCurrentTask = 0;

    pCMe->m_pIArmTimer->pVTbl->Start(pCMe->m_pIArmTimer);

    __asm volatile ("msr daifclr, #2" ::: "memory");

    while (1) {
        asm volatile ("wfi\n\t" ::: "memory");
    }
    return 0;
}

/*
 *
 * <сводка>
 *   Функция Stop
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Stop(/*in*/ struct IEcoTaskScheduler1* me) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Init
 * </сводка>
 *
 * <описание>
 *   Функция инициализации экземпляра
 * </описание>
 *
 */
int16_t ECOCALLMETHOD initCEcoTaskScheduler1Lab_C761620F(/*in*/ IEcoTaskScheduler1Ptr_t me, /* in */ struct IEcoUnknown *pIUnkSystem) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/* Create Virtual Table IEcoTaskScheduler1Lab */
IEcoTaskScheduler1VTbl g_x155C975395654F85B9AA27D5F377A79EVTbl_C761620F = {
    CEcoTaskScheduler1Lab_C761620F_QueryInterface,
    CEcoTaskScheduler1Lab_C761620F_AddRef,
    CEcoTaskScheduler1Lab_C761620F_Release,
    CEcoTaskScheduler1Lab_C761620F_Init,
    CEcoTaskScheduler1Lab_C761620F_InitWith,
    CEcoTaskScheduler1Lab_C761620F_NewTask,
    CEcoTaskScheduler1Lab_C761620F_DeleteTask,
    CEcoTaskScheduler1Lab_C761620F_SuspendTask,
    CEcoTaskScheduler1Lab_C761620F_ResumeTask,
    CEcoTaskScheduler1Lab_C761620F_RegisterInterrupt,
    CEcoTaskScheduler1Lab_C761620F_UnRegisterInterrupt,
    CEcoTaskScheduler1Lab_C761620F_Start,
    CEcoTaskScheduler1Lab_C761620F_Stop
};

/*
 *
 * <сводка>
 *   Функция Create
 * </сводка>
 *
 * <описание>
 *   Функция создания экземпляра
 * </описание>
 *
 */
int16_t ECOCALLMETHOD createCEcoTaskScheduler1Lab_C761620F(/* in */ IEcoUnknown* pIUnkSystem, /* in */ IEcoUnknown* pIUnkOuter, /* out */ IEcoTaskScheduler1Ptr_t* ppITaskScheduler) {
    int16_t result = -1;
    CEcoTaskScheduler1Lab_C761620F* pCMe = 0;

    /* Проверка указателей */
    if (ppITaskScheduler == 0) {
        return result;
    }

    /* Получение указателя на структуру компонента расположенной в области инициализированных данных */
    pCMe = &g_xCEcoTaskScheduler1Lab_C761620F;

    if (pCMe->m_cRef == 0) {
        /* Установка счетчика ссылок на компонент */
        pCMe->m_cRef = 1;

        /* Создание таблицы функций интерфейса IEcoTaskScheduler1 */
        pCMe->m_pVTblIScheduler = &g_x155C975395654F85B9AA27D5F377A79EVTbl_C761620F;

        result = 0;
    }

    /* Возврат указателя на интерфейс */
    *ppITaskScheduler = (IEcoTaskScheduler1*)pCMe;

    return result;
}

/*
 *
 * <сводка>
 *   Функция Delete
 * </сводка>
 *
 * <описание>
 *   Функция освобождения экземпляра
 * </описание>
 *
 */
void ECOCALLMETHOD deleteCEcoTaskScheduler1Lab_C761620F(/* in */ IEcoTaskScheduler1Ptr_t pITaskScheduler) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)pITaskScheduler;*/

}