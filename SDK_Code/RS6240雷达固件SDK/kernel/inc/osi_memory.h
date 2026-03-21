/**
 ******************************************************************************
 * @file    osi_memory.h
 * @brief   rtos interface memory define.
 ******************************************************************************
 * @attention
 *
 * Copyright (C) 2025 POSSUMIC TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of POSSUMIC TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#ifndef KERNEL_INC_OSI_MEMORY_H_
#define KERNEL_INC_OSI_MEMORY_H_

/* Includes.
 * ----------------------------------------------------------------------------
 */

#include <osi_common.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Exported macro.
 * ----------------------------------------------------------------------------
 */
#define OSI_Memcpy(d, s, l)      memcpy(d, s, l)
#define OSI_Memset(d, c, l)      memset(d, c, l)
#define OSI_Memcmp(a, b, l)      memcmp(a, b, l)
#define OSI_Memmove(d, s, n)     memmove(d, s, n)

/* Exported functions.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief t
 * @return memory
 */
void * OSI_Malloc(size_t size);

/**
 * @brief t
 * @return memory
 */
void * OSI_Realloc(void * mem, size_t size);

/**
 * @brief t
 * @return memory
 */
void OSI_Free(void * mem);

/**
 * @brief t
 * @return memory
 */
size_t OSI_GetFreeSize(void);


#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INC_OSI_MEMORY_H_ */

/*
 ******************************************************************************
 * (C) COPYRIGHT POSSUMIC TECHNOLOGY
 * END OF FILE
 */
