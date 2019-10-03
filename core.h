/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * core.h
 * Copyright (C) abhoriel 2010 <abhoriel@gmail.com>
 * 
 * gbem is free software copyrighted by abhoriel.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``abhoriel'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 
 * gbem IS PROVIDED BY abhoriel ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _CORE_H
#define _CORE_H

#include "gbem.h"
#include "memory.h"

#define INT_VBLANK	0x01
#define INT_STAT 	0x02
#define INT_TIMER 	0x04
#define INT_SERIAL 	0x08
#define INT_BUTTON 	0x10

#define FREQ_NORMAL	1
#define FREQ_DOUBLE	2

typedef struct {
		/* CPU registers */
		union UWord reg_af, reg_bc, reg_de, reg_hl;
		Word reg_sp, reg_pc;
		/* zero, subtract, half carry and carry flags */
		int flag_z, flag_n, flag_h, flag_c;
		int ei;
		int is_halted, is_stopped, ime;
		unsigned int frequency;
} CoreState;

int execute_cycles(int max_cycles);
void core_reset(void);
void dump_state(void);
void core_save(void);
void core_load(void);

static inline void raise_int(Byte interrupt) {
	writeb(HWREG_IF, readb(HWREG_IF) | interrupt);
}

#endif  // _CORE_H

