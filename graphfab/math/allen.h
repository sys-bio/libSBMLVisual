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

/** @file allen.h
 * @brief Intervals
  */

//== BEGINNING OF CODE ===============================================================

#ifndef __SBNW_ALLEN_H_
#define __SBNW_ALLEN_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"
#include "graphfab/math/min_max.h"

//-- C++ code --
#ifdef __cplusplus

namespace Graphfab {
    
    class Interval {
        public:
            /// Reorder endpoints if necessary
            Interval(const Real a, const Real b) {
                if(a <= b) {
                    _a = a;
                    _b = b;
                } else {
                    _a = b;
                    _b = a;
                }
            }
            
            /// Number-one reason why ignoring qualifiers is not okay
            Real a() const { return _a; }
            
            /// Returns reference
            Real& a() { return _a; }
            
            Real b() const { return _b; }
            
            Real& b() { return _b; }
        private:
            /// Endpoints, _a < _b
            Real _a, _b;
    };

    /// Get the distance between two intervals [u,v] and [x,y]; zero if they intersect.
    inline Real allenDist(const Real u, const Real v, const Real x, const Real y) {
        if(!(v < x || y < u))
            //intersect
            return 0.;
        Real a = x-v, b=u-y; //one will be negative
        return max(a,b);
    }

    /// allenDist returns absolute val of this; difficult to explain; used in @ref NetworkElement::forceVec
    inline Real allenOrdered(const Real u, const Real v, const Real x, const Real y) {
        // think of it like the result of b-a, where b and a are intervals
        if(!(v < x || y < u))
            //intersect
            return 0.;
        if(v < x)
            return x-v; //positive
        else
            return y-u; //negative
    }
    
    /// Is the point in the interval?
    inline bool pointInInterval(const Real p, const Interval& i) {
        return (i.a() <= p && p <= i.b()) ? true : false;
    }
    
}

#endif

#endif
