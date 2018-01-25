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

/** @file error.h
 * @brief Error information
  */

//== BEGINNING OF CODE ===============================================================

#ifndef __SBNW_DIAG_ERROR_H_
#define __SBNW_DIAG_ERROR_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"
#include "graphfab/sbml/autolayoutSBML.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Emit an error
 * @param[in] str The error message
 */
_GraphfabExport void gf_emitError(const char* str);

/**
 * @brief Emit a warning
 * @param[in] str The warning message
 */
_GraphfabExport void gf_emitWarn(const char* str);

/**
 * @brief Register an error listener
 * @param[in] listener The listener
 */
_GraphfabExport void gf_registerErrorListener(void (*)(const char* msg));

/** @brief Gets the last error
 *  @return The error message (owned by callee)
 *  \ingroup C_API
 */
_GraphfabExport char* gf_getLastError();

/** @brief Gets whether an error occurred
 *  @return True if an error has been set
 *  \ingroup C_API
 */
_GraphfabExport int gf_haveError();

/** @brief Clears the last error
 *  \ingroup C_API
 */
_GraphfabExport void gf_clearError();

/**
 * @brief Sets the last error
 * @param[in] msg The error message (is copied)
 *  \ingroup Internal
 */
_GraphfabExport void gf_setError(const char* msg);

#ifdef __cplusplus
}//extern "C"
#endif

#endif
