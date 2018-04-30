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

/** @file canvas.h
 * @brief Canvas for drawing diagram, dimensions
  */

//== BEGINNING OF CODE ===============================================================

#ifndef __SBNW_LAYOUT_POINT_H_
#define __SBNW_LAYOUT_POINT_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"

#include <math.h>

//-- C++ code --
#ifdef __cplusplus

#include <iostream>

namespace Graphfab {
    
    struct Point; // a pain...
    
    /// Translation via another point
    Point operator- (const Point& p, const Point& q);
    Point operator+ (const Point& p, const Point& q);
    
    /// Scalar multiplication
    Point operator* (const Point& p, const Real s);
    Point operator* (const Real s, const Point& p);
    
    /// Scalar division
    Point operator/ (const Point& p, const Real s);
    
    /// Dump to stream
    std::ostream& operator<< (std::ostream&, const Point& p);

    struct Point {
        /// No-arg constructor
        Point() {x = y = 0;}
        
        /// Initialilzing constructor
        Point(Real x_, Real y_)
            : x(x_), y(y_) {}
        
        static Point polar(Real mag, Real theta);

        /// Unary minus
        Point operator-() const {
            return Point(-x, -y);
        }
        
        /// Return the magitude of this point as if it were a vector
        Real mag() const {
            return sqrt(x*x + y*y);
        }
        
        /// Return the magitude squared
        Real mag2() const {
            return x*x + y*y;
        }

        /// Return angle of vector
        Real theta() const {
          const Real ep = 1e-4;
          if (std::abs(x) < ep) {
            if (std::abs(y) < ep)
              return 0;
            if (y > 0)
              return pi/2.;
            else
              return -pi/2.;
          }

          Real t = atan(y/x);

          if (x > 0)
            return t;
          else
            return t+pi;
        }
        
        /// Square both coordinates
        Point squareTerms() const {
            return Point(x*x, y*y);
        }
        
        /// Take the square root of both coords
        Point sqrtTerms() const {
            AT(x >= 0 && y >= 0, "Cannot take negative square root");
            return Point(sqrt(x), sqrt(y));
        }
        
        /// Scales the vector so that its magnitude is not greater than @a cap (safe)
        Point capMag(const Real cap) const {
            Real m = mag2();
            Real xx = x, yy = y;
            if(m > cap*cap) {
                m = sqrt(m);
                xx *= cap/m;
                yy *= cap/m;
            }
            return Point(xx,yy);
        }
        
        /** Scales the vector so that its magnitude is not greater than @a cap (unsafe)
         * @details Overwrite
         */
        void capMag2_(const Real cap2) {
            Real m = mag2();
            if(m > cap2) {
                m = sqrt(cap2/m);
                x *= m;
                y *= m;
            }
        }
        
        /// Normalize (safe)
        Point normed() const {
            Real o = mag();
            if(o < 1e-6)
                return *this;
            return (*this)*(1./o);
        }
        
        /// Normalize (unsafe)
        void norm_() {
            Real o = mag();
            if(o < 1e-6)
                return;
            x /= o;
            y /= o;
        }

        Point operator = (const Point& p) { x =p.x; y = p.y; return *this; }
        
        Point operator+= (const Point& p) { x+=p.x; y+=p.y; return *this; }
        
        Point operator-= (const Point& p) { x-=p.x; y-=p.y; return *this; }
        
        static Real min(Real x, Real y) { return x < y ? x : y; }
        static Real max(Real x, Real y) { return x < y ? y : x; }
        
        // element-wise min
        static Point emin(const Point& u, const Point& v) {
            return Point(min(u.x, v.x), min(u.y,v.y));
        }
        
        // element-wise max
        static Point emax(const Point& u, const Point& v) {
            return Point(max(u.x, v.x), max(u.y,v.y));
        }

        /// Rotate clockwise one right angle
        Point dextro() const {
          return polar(mag(), theta() - 0.5*pi);
        }

        /// Rotate counterclockwise one right angle
        Point sinister() const {
          return polar(mag(), theta() + 0.5*pi);
        }

        std::string rep() const;
        
        Real x;
        Real y;
    };
    
}

#endif

#endif
