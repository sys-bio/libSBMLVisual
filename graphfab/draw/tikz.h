/*== GRAPHFAB =======================================================================
 * Copyright (c) 2012-2015 Jesse K Medley
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

/**
 * @author JKM
 * @file tikz.h
 * @date 01/13/2015
 * @copyright BSD 3-clause (details in source)
 * @brief Render TikZ plots
  */

//== BEGINNING OF CODE ===============================================================

#ifndef __SBNW_DRAW_TIKZ_H_
#define __SBNW_DRAW_TIKZ_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"
#include "graphfab/layout/box.h"
#include "graphfab/layout/canvas.h"
#include "graphfab/network/network.h"
#include "graphfab/interface/layout.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Render the model as a TikZ image
 *  @param[in] l The model/layout infor
 *  \ingroup C_API
 */
_GraphfabExport const char* gf_renderTikZ(gf_layoutInfo* l);

/** @brief Render the model as a TikZ image
 *  @param[in] l The model/layout infor
 *  \ingroup C_API
 */
_GraphfabExport int gf_renderTikZFile(gf_layoutInfo* l, const char* filename);

#ifdef __cplusplus
}//extern "C"
#endif

//-- C++ code --
# ifdef __cplusplus

# include <iostream>

namespace Graphfab {
    
	class _GraphfabExport TikZRenderer {
    public:
      TikZRenderer(Box extents, Real widthcm, Real heightcm);

      std::string str(Network* net, Canvas* can);

      std::string process(Point p) const;

      std::string formatNodeText(const std::string& text) const;

    protected:
      Box extents_;
      Real widthcm_, heightcm_;
  };

    _GraphfabExport std::ostream& operator<<(std::ostream& o, const TikZRenderer& r);
    
}

# endif

#endif
