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
#include "graphfab/layout/box.h"
#include "graphfab/math/sign_mag.h"
#include "graphfab/math/allen.h"

namespace Graphfab {
    
    std::ostream& operator<< (std::ostream& o, const Box& b) {
        b.dump(o);
		return o;
    }
    
    // intersection of a ray emating from the origin (in direction v) & a horizontal line
    Point intersectRayHLine(const Point& v, const Real c) {
        AT(mag(v.y) > 1e-6, "No intersection");
        Real s = c / v.y;
        return Point(s*v.x, c);
    }
    
    // intersection of a ray emating from the origin (in direction v) & a vertical line
    Point intersectRayVLine(const Point& v, const Real c) {
        AT(mag(v.x) > 1e-6, "No intersection");
        Real s = c / v.x;
        return Point(c, s*v.y);
    }
    
    // intersection of line segment origin->v & horizontal line y = c
    std::pair<bool, Point> intersectVecHLineBounded(const Point& v, const Real c, const Real min, const Real max) {
        if(pointInInterval(c, Interval(0, v.y))) {
            Point p = intersectRayHLine(v,c);
            // check that it fits in the bounds of the box
            if(min <= p.x && p.x <= max)
                return std::make_pair(true, p);
            return std::make_pair(false, Point(0,0));
        } else
            return std::make_pair(false, Point(0,0));
    }
    
    // intersection of line segment origin->v & vertical line x = c
    std::pair<bool, Point> intersectVecVLineBounded(const Point& v, const Real c, const Real min, const Real max) {
        if(pointInInterval(c, Interval(0, v.x))) {
            Point p = intersectRayVLine(v,c);
            // check that it fits in the bounds of the box
            if(min <= p.y && p.y <= max)
                return std::make_pair(true, p);
            return std::make_pair(false, Point(0,0));
        } else
            return std::make_pair(false, Point(0,0));
    }
    
    std::pair<bool, Point> intersectBoxLine(const Box& b_, const Point& u, const Point& v_) {
        // make relative to a:
        Box b(b_.getMin()-u, b_.getMax()-u);
        Real bx1 = b.getMin().x, bx2 = b.getMax().x, by1 = b.getMin().y, by2 = b.getMax().y;
        Point v(v_-u);
        std::pair<bool, Point> r;
        // try top edge
        r = intersectVecHLineBounded(v, by1, bx1, bx2);
        if(r.first)
            return r;
        // try bottom edge
        r = intersectVecHLineBounded(v, by2, bx1, bx2);
        if(r.first)
            return r;
        // try left edge
        r = intersectVecVLineBounded(v, bx1, by1, by2);
        if(r.first)
            return r;
        // try right edge
        r = intersectVecVLineBounded(v, bx2, by1, by2);
        if(r.first)
            return r;
        
        AN(0, "Should not happen");
        return r;
    }

}
