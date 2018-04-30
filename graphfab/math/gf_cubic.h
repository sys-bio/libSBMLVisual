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

/**
 * @author JKM
 * @file transform.h
 * @date 12/18/2014
 * @copyright BSD 3-clause (details in source)
 * @brief Roots of cubic equations
 * @details See https://hal.archives-ouvertes.fr/file/index/docid/627327/filename/SFCEC.pdf
  */

//== BEGINNING OF CODE ===============================================================

#ifndef __SBNW_CUBIC_H_
#define __SBNW_CUBIC_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"

//-- C++ code --
# ifdef __cplusplus

# include <iostream>

namespace Graphfab {
    
	class _GraphfabExport CubicRoots {
        public:
            /// Solve the cubic polynomial x^3 + a2*x^2 + a1*x + a0 = 0
            CubicRoots(Real a2, Real a1, Real a0);

            Complex getRoot(int i) const;

            bool isRootReal(int i) const;

            Real getRealRoot(int i) const;

            /// Square root according to ZWH convention
            static Complex sqrtConventional(Complex x);

            /// Cubic root according to ZWH convention
            static Complex curtConventional(Complex x);

        protected:
          Complex x1_, x2_, x3_;
    };

    _GraphfabExport std::ostream& operator<<(std::ostream& o, const CubicRoots& c);
    
}

# endif

#endif
