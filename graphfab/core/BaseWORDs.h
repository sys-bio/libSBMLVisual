/*== SAGITTARIUS =====================================================================
 * Copyright (c) 2012, Jesse K Medley
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of The University of Washington nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//== FILEDOC =========================================================================

/** @file BaseWORDS.h
 * @brief Bit-size typedefs
  */

//== BEGINNING OF CODE ===============================================================

#ifndef __SAGITTARIUS_BASEWORDS_H_
#define __SAGITTARIUS_BASEWORDS_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCommon.h"

#include <stdint.h>

//==DEFINES/TYPES===================================//

//Sorry, but we can't put these typedefs in the Sagittarius
//namespace. There are extern "C" functions defined in C++
//headers that need to use these types, hence they must be
//available in the default namespace
/*#ifdef __cplusplus
namespace Sagittarius
{
#endif*/
    
    //These int sizes must be verified for each platform, but once that is done,
    //just plug in the appropriate types and go.
    
    //INTs:
    
    typedef int8_t      int8;
    typedef uint8_t     uint8;
    
    typedef int16_t     int16;
    typedef uint16_t    uint16;
    
    #define SG_UINT16_MAX 65535
    
    //typedef long            int32; //8 bytes for x86_64
    //typedef unsigned long   uint32; //8 bytes for x86_64
    
    typedef int32_t     int32;
    typedef uint32_t    uint32;
    
    typedef int64_t     int64; //also long long
    typedef uint64_t    uint64;
    
    //FLOATING POINT TYPES:
    
    typedef float       fp32;
    typedef double      fp64;
    
    //CHARACTERS:
    
    typedef char char8;
    typedef unsigned char uchar8;
    
    #define INT64_LOW_MASK  0x00000000FFFFFFFF
    #define INT64_HIGH_MASK 0xFFFFFFFF00000000
    
    //pointers
    #if SAGITTARIUS_ARCH == SAGITTARIUS_ARCH_64
        #define SG_POINTER_UINT uint64
    #elif SAGITTARIUS_ARCH == SAGITTARIUS_ARCH_64
        #define SG_POINTER_UINT uint32
    #endif
    
    //misc
    //booleans
    #define true 1 //don't rely on if(b == true), use if(b) instead
    #define false 0
    #ifndef __cplusplus
        #define sg_casbool sg_cas32
        typedef int32 bool;
    #endif
    
/*#ifdef __cplusplus
}
#endif*/

#endif
