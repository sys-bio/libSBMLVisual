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

/** @file fr.h
 * @brief Fruchterman-Reingold algorithm
  */

//== BEGINNING OF CODE ===============================================================

#ifndef __SBNW_LAYOUT_FR_H_
#define __SBNW_LAYOUT_FR_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"
#include "graphfab/layout/gf_canvas.h"
#include "graphfab/network/gf_network.h"
#include "graphfab/sbml/gf_layout.h"

//-- C code --

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __fr_options {
    /// Algorithm constant
    Real k;
    /// Constrain to boundary?
    int boundary;
    /// Use magnetism? (forces bridge reactions)
    int mag;
    /// Amount of gravity
    Real grav;
    /// Center of gravitational force
    Real baryx, baryy;
    /// Should the barycenter be set automatically from layout info?
    int autobary;
    /// Enable compartment force calc?
    int enable_comps;
    /// Randomize node positions before doing layout algo (library code DOES NOT call srand for reproducibility reasons)
    int prerandomize;
} fr_options;

/** @brief Run the autolayout (Fruchterman-Reingold) algorithm on a given layout structure
 *  @param[in] opt The options controlling the layout algorithm
 *  @param[in/out] l The layout info
 *  \ingroup C_API
 */
_GraphfabExport void gf_doLayoutAlgorithm(fr_options opt, gf_layoutInfo* l);

/** @brief Run the autolayout (Fruchterman-Reingold) algorithm on a a network and optional canvas
 *  @details Can be used when full layout struct is not available
 *  @param[in] opt The options controlling the layout algorithm
 *  @param[in/out] n The network
 *  @param[in] c The canvas (may be NULL)
 *  \ingroup C_API
 */
_GraphfabExport void gf_doLayoutAlgorithm2(fr_options opt, gf_network* n, gf_canvas* c);

/** @brief Returns the default values for the layout options
 *  @param[out] l The layout info in which to store the options
 *  \ingroup C_API
 */
_GraphfabExport void gf_getLayoutOptDefaults(fr_options* opt);

/** @brief Set the stiffness for the FR algorithm
 *  @param[out] opt The layout info in which to store the stiffness
 *  @param[in] k The stiffness
 *  \ingroup C_API
 */
_GraphfabExport void gf_layout_setStiffness(fr_options* opt, double k);

#ifdef __cplusplus
}//extern "C"
#endif

//-- C++ code --
#ifdef __cplusplus

// #include <string>

#include <iostream>

namespace Graphfab {

    /// Software Practice & Experience '91
    void FruchtermanReingold(fr_options opt, Network& net, Canvas* can, gf_layoutInfo* l);
    
}

#endif

#endif
