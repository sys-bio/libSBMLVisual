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

/** @file transform.h
 * @brief 2d transform applied to canvas elements
  */

//== BEGINNING OF CODE ===============================================================

#ifndef __SBNW_TRANSFORM_H_
#define __SBNW_TRANSFORM_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"
#include "graphfab/layout/gf_point.h"
#include "graphfab/layout/gf_box.h"

//-- C++ code --
#ifdef __cplusplus

#include <iostream>

namespace Graphfab {
    
    struct cutout {
        cutout(Real a, Real b, Real c, Real d)
            : u(a), v(b), x(c), y(d) {}
        Real u, v;
        Real x, y;
        
        Real det() const { return u*y - v*x; }
    };

    class _GraphfabExport Affine2d {
        protected:
            static Real min(Real x, Real y) {
                if(x < y)
                    return x;
                else
                    return y;
            }
        public:
            Affine2d() {
                _e[0] = 1.; _e[1] = 0.; _e[2] = 0.;
                _e[3] = 0.; _e[4] = 1.; _e[5] = 0.;
                _e[6] = 0.; _e[7] = 0.; _e[8] = 1.;
            }
            
            Affine2d(Real a, Real b, Real c,
                     Real u, Real v, Real w,
                     Real x, Real y, Real z) {
                _e[0] = a; _e[1] = b; _e[2] = c;
                _e[3] = u; _e[4] = v; _e[5] = w;
                _e[6] = x; _e[7] = y; _e[8] = z;
            }
            
            /// Invert
            Affine2d inv() const;
            
            /// Det
            Real det() const;
            
            /// Access the specified row/column
            Real rc(int r, int c) const {
                AT(0 <= r && r < 3, "Row out of range");
                AT(0 <= c && c < 3, "Column out of range");
                return _e[c+r*3];
            }
            
            /// Access the specified row/column as reference
            Real& rcref(int r, int c) {
                AT(0 <= r && r < 3, "Row out of range");
                AT(0 <= c && c < 3, "Column out of range");
                return _e[c+r*3];
            }
            
            // Hacky
            Real scaleFactor() const {
                return rc(0,0);
            }
            
            void set(int i, int j, Real val) { rcref(i,j) = val; }
            
            // Creators:
            
            static Affine2d makeXlate(Real x, Real y) {
                Affine2d a;
                a.rcref(0,2) = x;
                a.rcref(1,2) = y;
                return a;
            }
            
            static Affine2d makeXlate(const Point& p) { return makeXlate(p.x, p.y); }
            
            static Affine2d makeScale(Real x, Real y) {
                Affine2d a;
                a.rcref(0,0) = x;
                a.rcref(1,1) = y;
                return a;
            }
            
            // usual basis transformation - z is xlate
            // renamed to fromBasis
            static Affine2d fromPoints(const Point& x, const Point& y, const Point& z) {
                return Affine2d(
                    x.x, y.x, z.x,
                    x.y, y.y, z.y,
                     0.,  0.,  1.
                );
            }

            /// Construct from two basis vectors and a displacement
            static Affine2d fromBasis(const Point& u, const Point& v,  const Point& disp) {
              return Affine2d(
                u.x, v.x, disp.x,
                u.y, v.y, disp.y, 
                  0,   0, 1
                );
            }
            
            static Affine2d fromBox(const Box& b) {
                return fromPoints(Point(b.getMax().x, 0), Point(0, b.getMax().y), b.getMin());
            }
            
            static Affine2d sendTo(const Box& src, const Box& dst) {
                return fromBox(dst)*fromBox(src).inv();
            }
            
            static Affine2d FitToWindow(const Box& src, const Box& dst);
            
            // Operations:
            
            Point operator*(const Point& x) const;
            
            Box operator*(const Box& x) const;
            
            Affine2d operator*(const Real& k) const;
            
            Affine2d operator/(const Real& k) const { return (*this)*(1./k); }
            
            // Composition:
            
            static Affine2d compose(const Affine2d& u, const Affine2d& v);
            
            Affine2d operator*(const Affine2d& z) const { return compose(*this, z); }

            /// Discard displacement
            Point applyLinearOnly(const Point& x) const;

            Point getScale() const {
              return Point(applyLinearOnly(Point(1, 0)).mag(),
                applyLinearOnly(Point(0, 1)).mag());
            }

            Point getDisplacement() const {
              return Point(rc(0, 2), rc(1, 2));
            }
            
        protected:
            
            Affine2d cofactors() const;
            
            Real cofactor(int i, int j) const;
            
            // minor for row, col
            cutout getCutout(int r, int c) const;
            
            Real _e[9];
    };
    
    Point xformPoint(const Point& p, const Affine2d& t);
    Box xformBox(const Box& b, const Affine2d& t);
    
    /// Translate isometry
    //Affine2d makeXlate(const Point& p);
    
    _GraphfabExport std::ostream& operator<<(std::ostream& o, const Affine2d& t);
    
}

#endif

#endif
