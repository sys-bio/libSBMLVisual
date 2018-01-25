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
#include "graphfab/math/geom.h"
#include "graphfab/math/cubic.h"

namespace Graphfab {

    Point calcCurveBackup(const Point& src, const Point& cent, const Box& ext, Real dist) {

      // left side
      {
        Point p1(ext.getMin());
        Point p2(ext.getBottomLeftCorner());
        LinearIntersection head(p1, p2, src, cent);
        if (head.exists())
          return new2ndPos(src, head.p(), 0., -dist, false);
      }

      // right side
      {
        Point p1(ext.getTopRightCorner());
        Point p2(ext.getMax());
        LinearIntersection head(p1, p2, src, cent);
        if (head.exists())
          return new2ndPos(src, head.p(), 0., -dist, false);
      }

      // top side
      {
        Point p1(ext.getMin());
        Point p2(ext.getTopRightCorner());
        LinearIntersection head(p1, p2, src, cent);
        if (head.exists())
          return new2ndPos(src, head.p(), 0., -dist, false);
      }

      // bottom side
      {
        Point p1(ext.getBottomLeftCorner());
        Point p2(ext.getMax());
        LinearIntersection head(p1, p2, src, cent);
        if (head.exists())
          return new2ndPos(src, head.p(), 0., -dist, false);
      }

      return new2ndPos(src, cent, 0., -dist, false);
    }
    
    // CLASS Line2Desc:
    
    Line2Desc::Line2Desc(const Point& start, const Point& end) {
      A_ = end.y - start.y;
      B_ = start.x - end.x;
      C_ = start.x*(start.y - end.y) + start.y*(end.x - start.x);
    }

    std::ostream& operator<<(std::ostream& o, const Line2Desc& l) {
      o << "A: " << l.A_ << ", B: " << l.B_ << ", C: " << l.C_;
      return o;
    }

    // CLASS CubicBezier2Desc:

    CubicBezier2Desc::CubicBezier2Desc(const Point& start, const Point& c1, const Point& c2, const Point& end) {
      P0_ = start;
      P1_ = c1;
      P2_ = c2;
      P3_ = end;
    }

    Point CubicBezier2Desc::p(Real t) const {
      if (t < 0. || t > 1.)
        std::cerr << "Warning: t is out of bounds\n";
      Real u = 1.-t;
      return P0_*u*u*u + 3*u*u*t*P1_ + 3*u*t*t*P2_ + t*t*t*P3_;
    }

    Point CubicBezier2Desc::getCP(int n) const {
      switch (n) {
        case 0:
          return P0_;
        case 1:
          return P1_;
        case 2:
          return P2_;
        case 3:
          return P3_;
        default:
          AN(0, "Index out of range");
      }
    }

    std::ostream& operator<<(std::ostream& o, const CubicBezier2Desc& c) {
      o << "P0: " << c.P0_ << ", P1: " << c.P1_ << ", P2: " << c.P2_ << ", P3: " << c.P3_;
      return o;
    }

    CubicBezierIntersection::CubicBezierIntersection(const Line2Desc& l, const CubicBezier2Desc& c) {
      Real A = l.getA();
      Real B = l.getB();
      Real C = l.getC();

      Point P0 = c.getCP(0);
      Point P1 = c.getCP(1);
      Point P2 = c.getCP(2);
      Point P3 = c.getCP(3);

      Point alpha = -P0 + 3*P1 - 3*P2 + P3;
      Point beta = 3*P0 - 6*P1 + 3*P2;
      Point gamma = -3*P0 + 3*P1;
      Point delta = P0;

      // compare representations
      std::cerr <<  "  CubicBezierIntersection: original: " << c.p(0) << ", " << c.p(0.5) << ", " << c.p(1) << "\n";
      std::cerr <<  "  CubicBezierIntersection: modified: " << computeCubic(alpha, beta, gamma, delta, 0.)
        << ", " << computeCubic(alpha, beta, gamma, delta, 0.5)
        << ", " << computeCubic(alpha, beta, gamma, delta, 1.) << "\n";

      Real a2 = (A*beta.x + B*beta.y) / (A*alpha.x + B*alpha.y);
      Real a1 = (A*gamma.x + B*gamma.y) / (A*alpha.x + B*alpha.y);
      Real a0 = (C + A*delta.x + B*delta.y) / (A*alpha.x + B*alpha.y);

      CubicRoots r(a2, a1, a0);

      std::cerr << "CubicBezierIntersection: Got roots\n";

      r_.clear();
      for (int i = 0; i<3; ++i)
        if (r.isRootReal(i))
          r_.push_back(r.getRealRoot(i));


      std::cerr << "CubicBezierIntersection: done\n";
    }

    LinearIntersection::LinearIntersection(const Point& p1, const Point& p2, const Point& q1, const Point& q2) {
      Real denom = ((q2.y - q1.y) * (p2.x - p1.x)) - ((q2.x - q1.x) * (p2.y - p1.y));
      if (std::abs(denom) < 1e-3)
        v_ = false;

      Real ua, ub;
      ua = (((q2.x - q1.x) * (p1.y - q1.y)) - ((q2.y - q1.y) * (p1.x - q1.x))) / denom;
      ub = (((p2.x - p1.x) * (p1.y - q1.y)) - ((p2.y - p1.y) * (p1.x - q1.x))) / denom;

      if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1) {
        p_.x = (int)(p1.x + ua*(p2.x - p1.x));
        p_.y = (int)(p1.y + ua*(p2.y - p1.y));
        v_ = true;
      } else
        v_ = false;

    }

}
