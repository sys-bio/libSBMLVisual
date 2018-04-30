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

//== BEGINNING OF CODE ===============================================================

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"
#include "graphfab/math/gf_cubic.h"

namespace Graphfab {
    
    // CLASS CubicRoots:
    
    CubicRoots::CubicRoots(Real a2, Real a1, Real a0) {
      Complex p1 = a2*a2*a1*a1 +18.*a2*a1*a0 - 4.*a1*a1*a1 - 27.*a0*a0 - 4.*a2*a2*a2*a0;
      Complex p2 = 9.*a2*a1 - 27.*a0 - 2.*a2*a2*a2;

      Complex s = sqrtConventional(-3.*p1);

      Complex c1 = curtConventional((p2 + 3.*s)/2.);
      Complex c2 = curtConventional((p2 - 3.*s)/2.);

      Complex w = std::polar(1., 2.*pi/3.);

      Complex u1 = (-a2 + w*c1 + w*w*c2)/3.;
      Complex u2 = (-a2 + c1 + c2)/3.;
      Complex u3 = (-a2 + w*w*c1 + w*c2)/3.;

      x1_ = u1;
      x2_ = u2;
      x3_ = u3;
    }

    Complex CubicRoots::getRoot(int i) const {
      switch (i) {
        case 0:
          return x1_;
        case 1:
          return x2_;
        case 2:
          return x3_;
        default:
          SBNW_THROW(InvalidParameterException, "Index out of bounds", "CubicRoots::getRoot");
      }
    }

    bool CubicRoots::isRootReal(int i) const {
      Complex t = getRoot(i);
      Real r = std::real(t);
      const Real ep = 1e-3;
      return std::abs(std::abs(r) - std::abs(t)) < ep;
    }

    Real CubicRoots::getRealRoot(int i) const {
      if (!isRootReal(i))
        SBNW_THROW(RedundancyCheckFailureException, "Root is not real", "CubicRoots::getRealRoot");
      return std::real(getRoot(i));
    }

    Complex CubicRoots::sqrtConventional(Complex x) {
      return std::polar(std::pow(std::abs(x), 0.5), 0.5*std::arg(x));
    }

    Complex CubicRoots::curtConventional(Complex x) {
      Real r = std::pow(std::abs(x), 1/3.);
      Real a = std::arg(x);
      if (-pi < a && a < -pi/2)
        return std::polar(r, -(1./3.*a - 2./3.*pi));
      else if (a == -pi/2.)
        return std::polar(r, -(pi/2.));
      else if (-pi/2. < a && a < pi/2.)
        return std::polar(r, -(1/3.*a));
      else if (a ==  pi/2.)
        return std::polar(r, -(-pi/2.));
      else if (pi/2. < a && a <= pi)
        return std::polar(r, -(1./3.*a + 2./3.*pi));
      else
        // should not happen
        return std::polar(r, pi);
    }

    std::ostream& operator<<(std::ostream& o, const CubicRoots& c) {
      o << c.getRoot(0) << ", " << c.getRoot(1) << ", " << c.getRoot(2);
      return o;
    }

}
