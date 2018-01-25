/*== SAGITTARIUS =====================================================================
 * Copyright (c) 2014, Jesse K Medley
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
 * @file arrowhead.h
 * @date 12/21/2014
 * @copyright BSD 3-clause (details in source)
 * @brief Arrowhead primitive
  */

//== BEGINNING OF CODE ===============================================================

#ifndef __SBNW_LAYOUT_ARROWHEAD_H_
#define __SBNW_LAYOUT_ARROWHEAD_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"
#include "graphfab/layout/point.h"
#include "graphfab/math/transform.h"

//-- C++ code --
#ifdef __cplusplus

#include <string>

#include <iostream>

namespace Graphfab {

  typedef int ArrowheadStyle;

  /// Arrowheads for showing directionality of kinetic law
  class Arrowhead {
  public:
    virtual ~Arrowhead() {}

    virtual unsigned long getNumVerts() const = 0;
    virtual Point getVert(unsigned long n) const = 0;

    Point getTransformedVert(unsigned long n) const {
      return tf_*getVert(n);
    }

    Affine2d getTransform() const { return tf_; }

    void setTransform(const Affine2d& tf, bool recurse = true) { tf_ = tf; }

    Affine2d getInverseTransform() const { return itf_; }

    void setInverseTransform(const Affine2d& itf, bool recurse = true) { itf_ = itf; }

  protected:
    /// Transform
    Affine2d tf_;
    /// Inverse transform
    Affine2d itf_;

  };

  class PlainArrowhead : public Arrowhead {
  public:
    virtual unsigned long getNumVerts() const {
      return 3;
    }

    virtual Point getVert(unsigned long n) const {
      switch (n) {
        case 0:
          return Point(0, 1);
        case 1:
          return Point(1, 0);
        case 2:
          return Point(-1, 0);
        default:
          SBNW_THROW(InvalidParameterException, "Index out of range", "Arrowhead::getVert");
      }
    }

  };

  class SubstrateArrowhead : public Arrowhead {
  public:
    virtual unsigned long getNumVerts() const;

    virtual Point getVert(unsigned long n) const;
  };

  class ProductArrowhead : public Arrowhead {
  public:
    virtual unsigned long getNumVerts() const;

    virtual Point getVert(unsigned long n) const;
  };

  class ActivatorArrowhead : public Arrowhead {
  public:
    virtual unsigned long getNumVerts() const;

    virtual Point getVert(unsigned long n) const;
  };

  class InhibitorArrowhead : public Arrowhead {
  public:
    virtual unsigned long getNumVerts() const;

    virtual Point getVert(unsigned long n) const;
  };

  class ModifierArrowhead : public Arrowhead {
  public:
    virtual unsigned long getNumVerts() const;

    virtual Point getVert(unsigned long n) const;
  };

  class ArrowheadStyles {
  public:
    static unsigned long count();

    static bool isFilled(ArrowheadStyle style);

    static unsigned long getNumVerts(ArrowheadStyle style);

    static Point getVert(ArrowheadStyle style, int n);

  };

  extern ArrowheadStyle sub_arrow_style_;
  inline ArrowheadStyle ArrowheadStyleLookup(const SubstrateArrowhead* ) {
    return sub_arrow_style_;
  }

  extern ArrowheadStyle prod_arrow_style_;
  inline ArrowheadStyle ArrowheadStyleLookup(const ProductArrowhead* ) {
    return prod_arrow_style_;
  }

  extern ArrowheadStyle act_arrow_style_;
  inline ArrowheadStyle ArrowheadStyleLookup(const ActivatorArrowhead* ) {
    return act_arrow_style_;
  }
//
  extern ArrowheadStyle inh_arrow_style_;
  inline ArrowheadStyle ArrowheadStyleLookup(const InhibitorArrowhead* ) {
    return inh_arrow_style_;
  }
//
  extern ArrowheadStyle mod_arrow_style_;
  inline ArrowheadStyle ArrowheadStyleLookup(const ModifierArrowhead* ) {
    return mod_arrow_style_;
  }

  // set arrowhead style based on type
  template<typename ArrowheadT>
  class ArrowheadStyleControl {
  public:
  };

  template<>
  class ArrowheadStyleControl<SubstrateArrowhead> {
  public:
    static void set(ArrowheadStyle val) {
//       std::cerr << "graphfab: set style for substrate arrowhead\n";
      sub_arrow_style_ = val;
    }

    static ArrowheadStyle get() {
      return sub_arrow_style_;
    }
  };

  template<>
  class ArrowheadStyleControl<ProductArrowhead> {
  public:
    static void set(ArrowheadStyle val) {
//       std::cerr << "graphfab: set style for product arrowhead\n";
      prod_arrow_style_ = val;
    }

    static ArrowheadStyle get() {
      return prod_arrow_style_;
    }
  };

  template<>
  class ArrowheadStyleControl<ActivatorArrowhead> {
  public:
    static void set(ArrowheadStyle val) {
//       std::cerr << "graphfab: set style for activator arrowhead\n";
      act_arrow_style_ = val;
    }

    static ArrowheadStyle get() {
      return act_arrow_style_;
    }
  };

  template<>
  class ArrowheadStyleControl<InhibitorArrowhead> {
  public:
    static void set(ArrowheadStyle val) {
//       std::cerr << "graphfab: set style for inhibitor arrowhead\n";
      inh_arrow_style_ = val;
    }

    static ArrowheadStyle get() {
      return inh_arrow_style_;
    }
  };

  template<>
  class ArrowheadStyleControl<ModifierArrowhead> {
  public:
    static void set(ArrowheadStyle val) {
//       std::cerr << "graphfab: set style for modifier arrowhead\n";
      mod_arrow_style_ = val;
    }

    static ArrowheadStyle get() {
      return mod_arrow_style_;
    }
  };

}

#endif

#endif
