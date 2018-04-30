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

//== BEGINNING OF CODE ===============================================================

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"
#include "graphfab/draw/gf_tikz.h"
#include "graphfab/util/gf_string.h"

#include <sstream>

const char* gf_renderTikZ(gf_layoutInfo* l) {
  using namespace Graphfab;

  Network* net = (Network*)l->net;
  AN(net, "No network");
  Canvas* can = (Canvas*)l->canv;
  AN(can, "No canvas");

  Graphfab::Real cmscale = 50.;
  TikZRenderer renderer(can->getBox(), can->getWidth()/cmscale, can->getHeight()/cmscale);
  return gf_strclone(renderer.str(net, can).c_str());
}

int gf_renderTikZFile(gf_layoutInfo* l, const char* filename) {
//   fprintf(stderr, "Saving to TikZ file %s\n", filename);
  FILE* f = fopen(filename, "w");
  if (!f)
    return 1;

  const char* buf = gf_renderTikZ(l);
  if (!buf)
    return 1;

  fprintf(f, "%s", buf);

  fclose(f);

  return 0;
}

namespace Graphfab {

  std::string replaceSubstr(const std::string& input, const std::string& src, const std::string& dst) {
    // http://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c
    std::string result(input);
    std::size_t index = 0;
    while (true) {
      index = result.find(src, index);
      if (index == std::string::npos) break;

      result.replace(index, src.size(), dst);
      index += dst.size();
    }
    return result;
  }

  TikZRenderer::TikZRenderer(Box extents, Real widthcm, Real heightcm)
    : extents_(extents), widthcm_(widthcm), heightcm_(heightcm) {}

  std::string TikZRenderer::process(Point p) const {
    p.y = extents_.height() - p.y;
    return (0.01*p).rep();
  }

  std::string TikZRenderer::formatNodeText(const std::string& text) const {
    return replaceSubstr(text, "_", "\\_");
  }

  std::string TikZRenderer::str(Network* net, Canvas* can) {
    std::stringstream ss;
    ss << "\\begin{tikzpicture}\n";

    ss << "\\definecolor{jdorange}{rgb}{0.8, 0.5, 0.5}\n";

    ss << "\\definecolor{jdzero}{rgb}{1.0, 0.5, 0.5}\n";

    ss << "\n";

//     ss <<
//       "\\begin{axis}[\n"
//       "xmin=" << extents_.getMin().x << ", xmax=" << extents_.getMax().x << ",\n"
//       "ymin=" << extents_.getMin().y << ", ymax=" << extents_.getMax().y << ",\n"
//       "axis on top,\n"
//       "width=" << widthcm_ << "cm,\n"
//       "height=" <<  heightcm_ <<  "cm  \n"
//       "]\n";

    for (Network::RxnIt i=net->RxnsBegin(); i!=net->RxnsEnd(); ++i) {
      Reaction* r = *i;
      //  rebuilds curves
      r->getNumCurves();
      for (Reaction::CurveIt ci=r->CurvesBegin(); ci!=r->CurvesEnd(); ++ci) {
         RxnBezier* c = *ci;
         ss << "\\draw " <<
           process(c->s)  << " .. controls " <<
           process(c->c1)  << " and " <<
           process(c->c2) << " .. " <<
           process(c->e) <<
           ";\n";
      }
    }

    ss << "\n\n";

    for(Network::NodeIt i=net->NodesBegin(); i!=net->NodesEnd(); ++i) {
      Node* n = *i;
      Box b = n->getExtents();
//       ss <<
//        "\\draw " <<
//         process(b.getFirstQuadCorner())  << " -- " <<
//         process(b.getSecondQuadCorner()) << " -- " <<
//         process(b.getThirdQuadCorner())  << " -- " <<
//         process(b.getFourthQuadCorner()) << " -- " <<
//         process(b.getFirstQuadCorner())  << ";\n";

      ss <<
        "\\node[rounded corners=2pt, draw=jdorange, left color=jdzero,  right color=white] at " << process(n->getCentroid()) << "[\n" <<
        "  scale=0.45,\n" <<
        "  text=black,\n" <<
        "  rotate=0.0\n" <<
        "]{" << formatNodeText(n->getId()) << "};\n";
    }

    ss << "\n";

//     ss << "\\end{axis}\n";
    ss << "\\end{tikzpicture}\n";

    return ss.str();
  }

  std::ostream& operator<<(std::ostream& o, const TikZRenderer& r) {
    return o;
  }

}
