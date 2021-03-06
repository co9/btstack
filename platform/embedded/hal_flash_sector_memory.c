/*
 * Copyright (C) 2017 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MATTHIAS RINGWALD AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 *  hal_flash_sector.c -- volatile test environment that provides just two memory banks
 *
 */

#include "hal_flash_sector.h"
#include "hal_flash_sector_memory.h"
#include "btstack_debug.h"

#include "stdint.h"
#include "string.h"

#ifdef BTSTACK_TEST
#include "stdio.h"
#include <stdlib.h>	// exit(..)
#endif

static uint32_t hal_flash_sector_memory_get_size(void * context){
	hal_flash_sector_memory_t * self = (hal_flash_sector_memory_t *) context;
	return self->bank_size;
}

static void hal_flash_sector_memory_erase(void * context, int bank){
	hal_flash_sector_memory_t * self = (hal_flash_sector_memory_t *) context;
	if (bank > 1) return;
	memset(self->banks[bank], 0xff, self->bank_size);
}

static void hal_flash_sector_memory_read(void * context, int bank, uint32_t offset, uint8_t * buffer, uint32_t size){
	hal_flash_sector_memory_t * self = (hal_flash_sector_memory_t *) context;

	// log_info("read offset %u, len %u", offset, size);

	if (bank > 1) return;
	if (offset > self->bank_size) return;
	if ((offset + size) > self->bank_size) return;

	memcpy(buffer, &self->banks[bank][offset], size);
}

static void hal_flash_sector_memory_write(void * context, int bank, uint32_t offset, const uint8_t * data, uint32_t size){
	hal_flash_sector_memory_t * self = (hal_flash_sector_memory_t *) context;

	// log_info("write offset %u, len %u", offset, size);

	if (bank > 1) return;
	if (offset > self->bank_size) return;
	if ((offset + size) > self->bank_size) return;

#ifdef BTSTACK_TEST
	int i;
	for (i=0;i<size;i++){
		if (self->banks[bank][offset] != 0xff){
			printf("Error: offset %u written twice!\n", offset+i);
			exit(10);
			return;			
		}
	}
#endif

	memcpy(&self->banks[bank][offset], data, size);
}

static const hal_flash_sector_t hal_flash_sector_memory_instance = {
	/* uint32_t (*get_size)() */ &hal_flash_sector_memory_get_size,
	/* void (*erase)(int);    */ &hal_flash_sector_memory_erase,
	/* void (*read)(..);      */ &hal_flash_sector_memory_read,
	/* void (*write)(..);     */ &hal_flash_sector_memory_write,
};

/** 
 * Initialize instance
 */
const hal_flash_sector_t * hal_flash_sector_memory_init_instance(hal_flash_sector_memory_t * self, uint8_t * storage, uint32_t storage_size){
	self->bank_size = storage_size / 2;
	self->banks[0] = storage;
	self->banks[1] = &storage[self->bank_size];		
	return &hal_flash_sector_memory_instance;
}

