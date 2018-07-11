/******************************************************************************

    @file    StateOS: oscore.h
    @author  Rajmund Szymanski
    @date    11.07.2018
    @brief   StateOS port file for STM8 uC.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#ifndef __STATEOSCORE_H
#define __STATEOSCORE_H

#include <osbase.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_HEAP_SIZE
#define OS_HEAP_SIZE          0 /* default system heap: all free memory       */
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_STACK_SIZE
#define OS_STACK_SIZE       128 /* default task stack size in bytes           */
#endif

#ifndef OS_IDLE_STACK
#define OS_IDLE_STACK       128 /* idle task stack size in bytes              */
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_LOCK_LEVEL
#define OS_LOCK_LEVEL         0 /* critical section blocks all interrupts     */
#endif

#if     OS_LOCK_LEVEL > 0
#error  osconfig.h: Incorrect OS_LOCK_LEVEL value! Must be 0.
#endif

/* -------------------------------------------------------------------------- */

#ifndef OS_MAIN_PRIO
#define OS_MAIN_PRIO          0 /* priority of main process                   */
#endif

/* -------------------------------------------------------------------------- */

#ifdef  __cplusplus

#ifndef OS_FUNCTIONAL

#define OS_FUNCTIONAL         0 /* c++ functional library header not included */

#elif   OS_FUNCTIONAL

#error  c++ functional library not allowed for this compiler.

#endif//OS_FUNCTIONAL

#endif

/* -------------------------------------------------------------------------- */

typedef uint8_t               lck_t;
typedef uint8_t               stk_t;

/* -------------------------------------------------------------------------- */

#if   defined(__CSMC__)
extern  stk_t                _stack[];
#define MAIN_TOP             _stack+1
#endif

/* -------------------------------------------------------------------------- */
// task context

typedef struct __ctx ctx_t;

struct __ctx
{
	char     dummy; // sp is a pointer to the first free byte on the stack
	// context saved by the software
#if   defined(__CSMC__)
	char     r[10]; // c_lreg[4], c_y[3], c_x[3]
#endif
	// context saved by the hardware
	char     cc;
	char     a;
	unsigned x;
	unsigned y;
#if   defined(__SDCC)
	char     far;   // hardware saves pc as a far pointer (3 bytes)
#else
	FAR
#endif
	fun_t  * pc;
};

#if   defined(__SDCC)
#define _CTX_INIT( pc ) { 0, 0x20, 0, 0, 0, 0, pc }
#else
#define _CTX_INIT( pc ) { 0, { 0 }, 0x20, 0, 0, 0, (FAR fun_t *) pc }
#endif

/* -------------------------------------------------------------------------- */
// init task context

__STATIC_INLINE
void port_ctx_init( ctx_t *ctx, fun_t *pc )
{
	ctx->cc = 0x20;
#if   defined(__SDCC)
	ctx->far = 0;
	ctx->pc = pc;
#else
	ctx->pc = (FAR fun_t *) pc;
#endif
}

/* -------------------------------------------------------------------------- */
// is procedure inside ISR?

__STATIC_INLINE
bool port_isr_inside( void )
{
	return false;
}

/* -------------------------------------------------------------------------- */

#if   defined(__CSMC__)

#define _get_SP()    (void *)_asm("ldw x, sp")
#define _set_SP(sp)  (void)  _asm("ldw sp, x", (void *)(sp))

#define _get_CC()    (lck_t) _asm("push cc""\n""pop a")
#define _set_CC(cc)  (void)  _asm("push a""\n""pop cc", (lck_t)(cc))

#elif defined(__SDCC)

void  * _get_SP( void );
void    _set_SP( void *sp );

lck_t   _get_CC( void );
void    _set_CC( lck_t cc );

#endif

/* -------------------------------------------------------------------------- */
// get current stack pointer

__STATIC_INLINE
void * port_get_sp( void )
{
	return _get_SP();
}

/* -------------------------------------------------------------------------- */

#define port_get_lock()      _get_CC()
#define port_put_lock(lck)   _set_CC(lck)

#define port_set_lock()       disableInterrupts()
#define port_clr_lock()       enableInterrupts()

#define port_sys_lock()  do { lck_t __LOCK = port_get_lock(); port_set_lock()
#define port_sys_unlock()     port_put_lock(__LOCK); } while(0)

#define port_isr_lock()  do { port_set_lock()
#define port_isr_unlock()     port_clr_lock(); } while(0)

#define port_set_barrier()    nop()

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif//__STATEOSCORE_H
