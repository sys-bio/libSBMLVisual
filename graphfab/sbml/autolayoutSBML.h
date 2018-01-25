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

/** @file sbml.h
 * @brief SBML interface
  */

//== BEGINNING OF CODE ===============================================================

#ifndef __SBNW_SBML_H_
#define __SBNW_SBML_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief C wrapper for SBMLDocument
 */
typedef struct __gf_SBMLLayout {
    void* pdoc; /// Pointer to SBMLDocument cast to void
} gf_SBMLModel;

/** @brief Destructor for @ref gf_SBMLModel
 *  @param[in] lo The SBML model; all memory used by the model is freed
 *  \ingroup C_API
 */
_GraphfabExport void gf_freeSBMLModel(gf_SBMLModel* lo);

/** @brief Load SBML from memory buffer. Struct contains a pointer to the document.
 *  @param[in] buf The buffer containing the SBML file
 *  @param[out] r The SBML model; the model that contains the SBML info from the buffer
 *  \ingroup C_API
 */
_GraphfabExport gf_SBMLModel* gf_loadSBMLbuf(const char* buf);

/** @brief Load SBML from memory buffer. Struct contains a pointer to the document.
 *  @param[in] buf The buffer containing the SBML file
 *  @param[out] r The SBML model; the model that contains the SBML info from the buffer
 *  \ingroup C_API
 */
_GraphfabExport gf_SBMLModel* gf_loadSBMLfile(const char* file);

#ifdef __cplusplus
}//extern "C"
#endif

#endif
