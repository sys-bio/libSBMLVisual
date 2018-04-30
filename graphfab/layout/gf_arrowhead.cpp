/*== SAGITTARIUS =====================================================================
 * Copyright (c) 2015, Jesse K Medley
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
 * @file arrowhead.cpp
 * @date 02/05/2015
 * @copyright BSD 3-clause (details in source)
 * @brief Arrowhead primitive
  */

//== BEGINNING OF CODE ===============================================================

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"
#include "graphfab/layout/gf_arrowhead.h"

//-- C++ code --
#ifdef __cplusplus

#include <string>

#include <iostream>

static unsigned long n_semicirc_segments = 16;

namespace Graphfab {

  unsigned long ArrowheadStyles::count() {
    return 8;
  }

  unsigned long ArrowheadStyles::getNumVerts(ArrowheadStyle style) {
    switch (style) {
      case 0:
        return 0;
      case 1:
      case 2:
        return 4;
      case 3:
      case 4:
        return 4;
      case 5:
        return 2;
      case 6:
        return 4;
      case 7:
        return n_semicirc_segments;
      default:
        SBNW_THROW(InvalidParameterException, "Unknown style", "ArrowheadStyles::getNumVerts");
    }
  }

  bool ArrowheadStyles::isFilled(ArrowheadStyle style) {
    switch (style) {
      case 0:
        return false;
      case 1:
        return false;
      case 2:
        return true;
      case 3:
        return false;
      case 4:
        return true;
      case 5:
        return false;
      case 6:
        return false;
      case 7:
        return false;
      default:
        SBNW_THROW(InvalidParameterException, "Unknown style", "ArrowheadStyles::isFilled");
    }
  }

  Point ArrowheadStyles::getVert(ArrowheadStyle style, int n) {
    switch (style) {
      case 0:
        SBNW_THROW(InvalidParameterException, "No verts", "ArrowheadStyles::getVert");
      case 1:
      case 2:
        // wide arrow
        switch (n) {
          case 0:
            return Point(0, 1);
          case 1:
            return Point(1, 0);
          case 2:
            return Point(-1, 0);
          case 3:
            return Point(0, 1);
          default:
            SBNW_THROW(InvalidParameterException, "Index out of range", "ArrowheadStyles::getVert");
        }
      case 3:
      case 4:
        // narrow arrow
        switch (n) {
          case 0:
            return Point(0, 1);
          case 1:
            return Point(0.5, 0);
          case 2:
            return Point(-0.5, 0);
          case 3:
            return Point(0, 1);
          default:
            SBNW_THROW(InvalidParameterException, "Index out of range", "ArrowheadStyles::getVert");
        }
      case 5:
        // crossbar
        switch (n) {
          case 0:
            return Point(-1, 0);
          case 1:
            return Point(1, 0);
          default:
            SBNW_THROW(InvalidParameterException, "Index out of range", "ArrowheadStyles::getVert");
        }
      case 6:
        // open box
        switch (n) {
          case 0:
            return Point(-1, 0.5);
          case 1:
            return Point(-1, 0);
          case 2:
            return Point(1, 0);
          case 3:
            return Point(1, 0.5);
          default:
            SBNW_THROW(InvalidParameterException, "Index out of range", "ArrowheadStyles::getVert");
        }
      case 7:
        // semicircle
        {
          Real t = (Real)n/(Real)n_semicirc_segments;
          return Point(cos(t*pi), -sin(t*pi)+1.);
        }
      default:
        SBNW_THROW(InvalidParameterException, "Unknown style", "ArrowheadStyles::getVert");
    }
  }

  unsigned long SubstrateArrowhead::getNumVerts() const {
    return ArrowheadStyles::getNumVerts(ArrowheadStyleLookup(this));
  }

  Point SubstrateArrowhead::getVert(unsigned long n) const {
    return ArrowheadStyles::getVert(ArrowheadStyleLookup(this), n);
  }

  unsigned long ProductArrowhead::getNumVerts() const {
    return ArrowheadStyles::getNumVerts(ArrowheadStyleLookup(this));
  }

  Point ProductArrowhead::getVert(unsigned long n) const {
    return ArrowheadStyles::getVert(ArrowheadStyleLookup(this), n);
  }

  unsigned long ActivatorArrowhead::getNumVerts() const {
    return ArrowheadStyles::getNumVerts(ArrowheadStyleLookup(this));
  }

  Point ActivatorArrowhead::getVert(unsigned long n) const {
    return ArrowheadStyles::getVert(ArrowheadStyleLookup(this), n);
  }

  unsigned long InhibitorArrowhead::getNumVerts() const {
    return ArrowheadStyles::getNumVerts(ArrowheadStyleLookup(this));
  }

  Point InhibitorArrowhead::getVert(unsigned long n) const {
    return ArrowheadStyles::getVert(ArrowheadStyleLookup(this), n);
  }

  unsigned long ModifierArrowhead::getNumVerts() const {
    return ArrowheadStyles::getNumVerts(ArrowheadStyleLookup(this));
  }

  Point ModifierArrowhead::getVert(unsigned long n) const {
    return ArrowheadStyles::getVert(ArrowheadStyleLookup(this), n);
  }

  ArrowheadStyle sub_arrow_style_ = 0;
  ArrowheadStyle prod_arrow_style_ = 1;
  ArrowheadStyle act_arrow_style_ = 0;
  ArrowheadStyle inh_arrow_style_ = 0;
  ArrowheadStyle mod_arrow_style_ = 0;

}

#endif
