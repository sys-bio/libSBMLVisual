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
#include "graphfab/diag/gf_error.h"
#include "graphfab/network/gf_network.h"
#include "graphfab/io/gf_io.h"
#include "graphfab/math/gf_rand_unif.h"
#include "graphfab/math/gf_dist.h"
#include "graphfab/math/gf_allen.h"
#include "graphfab/math/gf_sign_mag.h"
#include "graphfab/math/gf_geom.h"

#include <exception>
#include <typeinfo>
#include <math.h>
#include <stdlib.h> //rand

namespace Graphfab {
    
    std::string eltTypeToStr(const NetworkEltType t) {
        switch(t) {
            case NET_ELT_TYPE_SPEC:
                return "Type Species";
            case NET_ELT_TYPE_RXN:
                return "Type Reaction";
            case NET_ELT_TYPE_COMP:
                return "Type Compartment";
            default:
                AN(0, "Unknown type");
        }
    }
    
    void dumpEltType(std::ostream& os, const NetworkEltType t, uint32 ind) {
        os << eltTypeToStr(t);
    }

    RxnRoleType SBMLRole2GraphfabRole(::SpeciesReferenceRole_t role) {
        switch (role) {
            case SPECIES_ROLE_SUBSTRATE:
                return RXN_ROLE_SUBSTRATE;
            case SPECIES_ROLE_PRODUCT:
                return RXN_ROLE_PRODUCT;
            case SPECIES_ROLE_SIDESUBSTRATE:
                return RXN_ROLE_SIDESUBSTRATE;
            case SPECIES_ROLE_SIDEPRODUCT:
                return RXN_ROLE_SIDEPRODUCT;
            case SPECIES_ROLE_MODIFIER:
                return RXN_ROLE_MODIFIER;
            case SPECIES_ROLE_ACTIVATOR:
                return RXN_ROLE_ACTIVATOR;
            case SPECIES_ROLE_INHIBITOR:
                return RXN_ROLE_INHIBITOR;
            case SPECIES_ROLE_UNDEFINED:
                AN(0, "Cannot convert role SPECIES_ROLE_UNDEFINED");
            default:
                AN(0, "Unknown role");
                return RXN_ROLE_SUBSTRATE;
        }
    }

    // true for substrates/products
    static bool isRoleActive(RxnRoleType role) {
        switch (role) {
            case RXN_ROLE_SUBSTRATE:
            case RXN_ROLE_PRODUCT:
            case RXN_ROLE_SIDESUBSTRATE:
            case RXN_ROLE_SIDEPRODUCT:
                return true;
            default:
                return false;
        }
    }

    // modifiers match activators and inhibitors
    static bool isGenericModifier(RxnRoleType role) {
      return role == RXN_ROLE_MODIFIER || role == RXN_ROLE_ACTIVATOR || role == RXN_ROLE_INHIBITOR;
    }

    // modifiers match activators and inhibitors
    static bool matchSBML_RoleGenericMod(RxnRoleType u, RxnRoleType v) {
      if (isGenericModifier(u) && isGenericModifier(v))
        return true;
      else
        return u == v;
    }

    //-- Reaction Curves --

    ArrowheadStyle SubCurve::getArrowheadStyle() const {
      return ArrowheadStyleLookup(this);
    }

    ArrowheadStyle PrdCurve::getArrowheadStyle() const {
      return ArrowheadStyleLookup(this);
    }

    ArrowheadStyle ActCurve::getArrowheadStyle() const {
      return ArrowheadStyleLookup(this);
    }

    ArrowheadStyle InhCurve::getArrowheadStyle() const {
      return ArrowheadStyleLookup(this);
    }

    ArrowheadStyle ModCurve::getArrowheadStyle() const {
      return ArrowheadStyleLookup(this);
    }
    
    //--CLASS NetworkElement--
    
    void NetworkElement::resetActivity() {
        _v = Point(0.,0.);
    }
    
    void NetworkElement::doMotion(const Real scale) {
        if(_lock)
            return;
        AT(_type != NET_ELT_TYPE_COMP);
//         _p = _p + _v*scale;
        if (_v.mag2() > 1e-6)
          _p = _p + _v.normed()*scale;
    }
    
    void NetworkElement::addDelta(const Point& d) {
        _v = _v + d;
    }
    
    void NetworkElement::capDelta(const Real cap) {
        _v = _v.capMag(cap);
    }
    
    void NetworkElement::capDelta2(const Real cap2) {
        _v.capMag2_(cap2);
    }
    
    void NetworkElement::setCentroid(const Point& p) {
        _p = p;
        _pset = 1;
        recalcExtents();
    }
    
    void NetworkElement::setGlobalCentroid(const Point& p) {
        _p = itf_*p;
        _pset = 1;
        recalcExtents();
    }
    
    Point NetworkElement::getCentroid(COORD_SYSTEM coord) const {
      if (coord == COORD_SYSTEM_LOCAL)
        return _p;
      else if (coord == COORD_SYSTEM_GLOBAL)
        return tf_*_p;
      else {
        AN(0, "Unknown coord system");
        return _p;
      }
    }
    
    Real NetworkElement::distance(const NetworkElement& e) const {
        NetworkEltShape sp1 = getShape(), sp2 = e.getShape();
        
        if(sp1 == sp2 && sp2 == ELT_SHAPE_ROUND) {
            Real r = euclidean2d(getCentroid(), e.getCentroid()) - radius() - e.radius();
            return max(r, 0.);
        } else {
            // works for boxes & mixed (well enough)
            Real u = allenDist(getMinX(), getMaxX(), e.getMinX(), e.getMaxX());
            Real v = allenDist(getMinY(), getMaxY(), e.getMinY(), e.getMaxY());
            return sqrt(u*u + v*v);
        }
    }
    
    bool NetworkElement::overlap(const NetworkElement& e) const {
        return (distance(e) == 0.);
    }
    
    Point NetworkElement::forceVec(const NetworkElement& e) const {
        NetworkEltShape sp1 = getShape(), sp2 = e.getShape();
        
        if(sp1 == sp2 && sp2 == ELT_SHAPE_ROUND) {
            return (getCentroid() - e.getCentroid()).normed();
        } else {
            if(overlap(e)) {
                //repel via centroids when elements are overlapping
                return (getCentroid() - e.getCentroid()).normed();
            }
            // works for boxes & mixed (well enough)
            Real u = -allenOrdered(getMinX(), getMaxX(), e.getMinX(), e.getMaxX());
            Real v = -allenOrdered(getMinY(), getMaxY(), e.getMinY(), e.getMaxY());
            return Point(u,v).normed();
        }
    }

    Point NetworkElement::centroidDisplacementFrom(const NetworkElement& e) const {
            return getCentroid() - e.getCentroid();
    }
    
    void NetworkElement::forceVec_(const NetworkElement& e, Point& p) const {
        NetworkEltShape sp1 = getShape(), sp2 = e.getShape();
        
        if(sp1 == sp2 && sp2 == ELT_SHAPE_ROUND) {
            p = (getCentroid() - e.getCentroid()).normed();
        } else {
            if(overlap(e)) {
                //repel via centroids when elements are overlapping
                p = (getCentroid() - e.getCentroid()).normed();
                return;
            }
            // works for boxes & mixed (well enough)
            Real u = -allenOrdered(getMinX(), getMaxX(), e.getMinX(), e.getMaxX());
            Real v = -allenOrdered(getMinY(), getMaxY(), e.getMinY(), e.getMaxY());
            p.x = u;
            p.y = v;
            p.norm_();
        }
    }
    
    //--CLASS Node--
    void Node::setName(const std::string& name) {
        _name = name;
    }
	
	const std::string& Node::getName() const {
        return _name;
    }
    
    const std::string& Node::getId() const {
        return _id;
    }
    
    void Node::setId(const std::string& id) {
        _id = id;
    }
    
    const std::string& Node::getGlyph() const {
        return _gly;
    }
    
    void Node::setGlyph(const std::string& id) {
        _gly = id;
    }

    int Node::alias(Network* net) {
        if(!net->containsNode(this))
          SBNW_THROW(InvalidParameterException, "No such node in network", "Network::alias");

        net->clearExcludeFromSubgraphEnum();
        int nsub_before = net->getNumSubgraphs();

        net->clearExcludeFromSubgraphEnum();
        setExcludeFromSubgraphEnum();
        int nsub_after = net->getNumSubgraphs();

        if (nsub_before !=  nsub_after)
          return 1;

        for (Network::RxnIt i=net->RxnsBegin(); i!=net->RxnsEnd(); ++i) {
          Reaction* r = *i;
          int k = 0;

          typedef std::vector< std::pair<Reaction*, Node*> > RxnList;

          RxnList rxnlist;

          for (Reaction::CurveIt ci=r->CurvesBegin(); ci!=r->CurvesEnd(); ++ci, ++k) {
            RxnBezier* c = *ci;

            if (c->ns != this && c->ne != this)
              continue;

            Node* n = new Node();

            n->setName(getName());

            n->setWidth(getWidth());
            n->setHeight(getHeight());

            {
              std::stringstream ss;
              ss << getId() << "_alias" << k;
              n->setId(ss.str());
            }
            n->numUses() = 1;
            n->setAlias(true);

            if (Compartment* comp = net->findContainingCompartment(this))
              comp->addElt(n);
            n->set_i(net->getUniqueIndex());

            n->setCentroid(new2ndPos(c->getCentroidCP(), getCentroid(), 0., -50., false));
            n->setTransform(tf_,  false);
            n->setInverseTransform(itf_,  false);

            net->addNode(n);

            rxnlist.push_back(std::make_pair(r, n));
          }

          // add to reactions
          for (RxnList::iterator j = rxnlist.begin(); j != rxnlist.end(); ++j)
            j->first->addSpeciesRef(j->second, j->first->getSpeciesRole(this));

          // rebuild curves
          for (RxnList::iterator j = rxnlist.begin(); j != rxnlist.end(); ++j)
            j->first->rebuildCurves();

        }

        try {
          net->removeNode(this);
        } catch(...) {
          SBNW_THROW(InternalCheckFailureException, "Could not remove original node", "Network::alias");
        }

        return 0;
    }
    
    Point Node::getUpperLeftCorner() const {
        return _p - Point(40,20);
    }
    
    Point Node::getLowerRightCorner() const {
        return _p + Point(40,20);
    }
    
    void Node::setWidth(Real w) {
        Point d(w/2., getHeight()/2.);
        _ext.setMin(getCentroid() - d);
        _ext.setMax(getCentroid() + d);
    }
    
    void Node::setHeight(Real h) {
        Point d(getWidth()/2., h/2.);
        _ext.setMin(getCentroid() - d);
        _ext.setMax(getCentroid() + d);
    }
    
    void Node::affectGlobalWidth(Real ww) {
        Real w = ww/tf_.scaleFactor();
        Point d(w/2., getHeight()/2.);
        _ext.setMin(getCentroid() - d);
        _ext.setMax(getCentroid() + d);
    }
    
    void Node::affectGlobalHeight(Real hh) {
        Real h = hh/tf_.scaleFactor();
        Point d(getWidth()/2., h/2.);
        _ext.setMin(getCentroid() - d);
        _ext.setMax(getCentroid() + d);
    }
    
    void Node::dump(std::ostream& os, uint32 ind) {
        indent(os, ind);
        if(isAlias())
            os << "Alias ";
        os << "Node:\n";
        indent(os, ind+2);
        os << "Name: \"" << _name << "\"\n";
        indent(os, ind+2);
        os << "ID: \"" << _id << "\"\n";
        if(_comp) {
            indent(os, ind+2);
            os << "Compartment: " << _comp->getId() << "\n";
        }
        indent(os, ind+2);
        os << "Degree: " << _deg << "\n";
        indent(os, ind+2);
        os << "Local degree: " << _ldeg << "\n";
        indent(os, ind+2);
        os << "Glyph: \"" << _gly << "\"\n";
        indent(os, ind+2);
        os << "Bounding Box: " << getUpperLeftCorner() << ", " << getLowerRightCorner() << "\n";
    }
    
    void Node::dumpForces(std::ostream& os, uint32 ind) const {
        indent(os, ind);
        os << "Node forces: " << _v << "\n";
    }
    
    std::string rxnRoleToString(RxnRoleType role) {
        switch(role) {
            case RXN_ROLE_SUBSTRATE:
                return "substrate";
            case RXN_ROLE_PRODUCT:
                return "product";
            case RXN_ROLE_SIDESUBSTRATE:
                return "side substrate";
            case RXN_ROLE_SIDEPRODUCT:
                return "side product";
            case RXN_ROLE_MODIFIER:
                return "modifier";
            case RXN_ROLE_ACTIVATOR:
                return "activator";
            case RXN_ROLE_INHIBITOR:
                return "inhibitor";
        }
    }

    std::string CurveTypeToString(RxnCurveType t) {
        switch(t) {
            case RXN_CURVE_SUBSTRATE:
                return "Substrate";
            case RXN_CURVE_PRODUCT:
                return "Product";
            case RXN_CURVE_MODIFIER:
                return "Modifier";
            case RXN_CURVE_ACTIVATOR:
                return "Activator";
            case RXN_CURVE_INHIBITOR:
                return "Inhibitor";
            default:
                return "Unknown";
        }
    }

    // -- CLASS RxnCurveFactory

    RxnBezier* RxnCurveFactory::CreateCurve(RxnRoleType role) {
        switch(role) {
            case RXN_ROLE_SUBSTRATE:
            case RXN_ROLE_SIDESUBSTRATE:
                return new SubCurve();
            case RXN_ROLE_PRODUCT:
            case RXN_ROLE_SIDEPRODUCT:
                return new PrdCurve();
            case RXN_ROLE_MODIFIER:
                return new ModCurve();
            case RXN_ROLE_ACTIVATOR:
                return new ActCurve();
            case RXN_ROLE_INHIBITOR:
                return new InhCurve();
            default:
                AN(0, "Unrecognized species type");
        }
    }
    
    //--CLASS Reaction--
    
    void Reaction::hierarchRelease() {
        deleteCurves();
    }
    
    void Reaction::addSpeciesRef(Node* n, RxnRoleType role) {
        _spec.push_back(std::make_pair(n, role));
        // recompute curves
        _cdirty = 1;
        // increase degree
        ++_deg;
        ++_ldeg;
        ++n->_deg;
        ++n->_ldeg;
    }
    
    void Reaction::removeNode(Node* n) {
        bool rebuild=false;
        repeat:
        for(NodeVec::iterator i=_spec.begin(); i!=_spec.end(); ++i) {
            Node* x = i->first;
            if(x == n) {
                rebuild = true;
                std::cout << "Rxn: element erased\n";
                --_deg;
                --_ldeg;
                --n->_deg;
                --n->_ldeg;
                _spec.erase(i);
                goto repeat; // in case the species shows up multiple times
            }
        }
        if(rebuild)
            rebuildCurves();
    }
    
    Node* Reaction::findSpeciesById(const std::string& id) {
        for(NodeVec::iterator i=_spec.begin(); i!=_spec.end(); ++i) {
            Node* n = i->first;
            if(n->getId() == id)
                return n;
        }
        //not found
        return NULL;
    }
    
    bool Reaction::hasSpecies(const Node* n) const {
        for(ConstNodeIt i=NodesBegin(); i!=NodesEnd(); ++i) {
            const Node* nn = i->first;
            if(nn == n)
                return 1;
        }
        //not found
        return 0;
    }

    uint64 Reaction::degree(const Node* n) {
        unsigned long result = 0;
        for(ConstNodeIt i=NodesBegin(); i!=NodesEnd(); ++i) {
            const Node* nn = i->first;
            if(nn == n)
                ++result;
        }
        return result;
    }
    
    void Reaction::substituteSpeciesById(const std::string& id, Node* spec) {
        for(NodeVec::iterator i=_spec.begin(); i!=_spec.end(); ++i) {
            Node* n = i->first;
            if(n->getId() == id) {
                --n->_ldeg;
                ++spec->_ldeg;
                i->first = spec;
            }
        }
    }

    void Reaction::substituteSpeciesByIdwRole(const std::string& id, Node* spec, RxnRoleType role) {
        for(NodeVec::iterator i=_spec.begin(); i!=_spec.end(); ++i) {
            Node* n = i->first;
            if(n->getId() == id && matchSBML_RoleGenericMod(i->second, role)) {
                --n->_ldeg;
                ++spec->_ldeg;
                i->first = spec;
                // SBML inconsistency
                if ((i->second == RXN_ROLE_MODIFIER) && (role == RXN_ROLE_ACTIVATOR || role == RXN_ROLE_INHIBITOR) ) {
//                   std::cerr << "Set role for " << spec->getId() << " to " << rxnRoleToString(role) << "\n";
                  i->second = role;
                }
            }
        }
    }

    RxnRoleType Reaction::getSpeciesRole(Node* x) {
        for(NodeVec::iterator i=_spec.begin(); i!=_spec.end(); ++i) {
            Node* n = i->first;
            if(n == x) {
                return i->second;
            }
        }
        SBNW_THROW(InternalCheckFailureException, "No such node", "Reaction::getSpeciesRole");
    }
    
    void Reaction::substituteSpecies(Node* before, Node* after) {
        for(NodeVec::iterator i=_spec.begin(); i!=_spec.end(); ++i) {
            Node* n = i->first;
            if(n == before) {
                --n->_ldeg;
                ++after->_ldeg;
                i->first = after;
            }
        }
    }
    
    Reaction::CurveVec& Reaction::getCurves() {
        curveGuard();
        return _curv;
    }

#define REBUILD_CURVES_DIAG 0
    
    void Reaction::rebuildCurves() {
        deleteCurves();

# if REBUILD_CURVES_DIAG
        std::cerr << "Rebuild curves\n";
# endif
        
        for(ConstNodeIt i=NodesBegin(); i!=NodesEnd(); ++i) {
            Node* n = i->first;
            RxnRoleType r = i->second;
            // the curve
            RxnBezier* curv = NULL;
# if REBUILD_CURVES_DIAG
            std::cerr << "  Role: " << rxnRoleToString(r) << "\n";
# endif
            switch(r) {
                case RXN_ROLE_SUBSTRATE:
                case RXN_ROLE_SIDESUBSTRATE:
                    curv = new SubCurve();
                    curv->as = &n->_p;
                    curv->ns = n;
                    curv->owns = 0; //weak ref
                    curv->ae = &_p;
                    curv->owne = 0; //weak ref
                    break;
                case RXN_ROLE_PRODUCT:
                case RXN_ROLE_SIDEPRODUCT:
                    curv = new PrdCurve();
                    curv->as = &_p;
                    curv->owns = 0; //weak ref
                    curv->ae = &n->_p;
                    curv->ne = n;
                    curv->owne = 0; //weak ref
                    break;
                case RXN_ROLE_MODIFIER:
                    curv = new ModCurve();
                    curv->as = &n->_p;
                    curv->ns = n;
                    curv->owns = 0; //weak ref
                    curv->ae = &_p;
                    curv->owne = 0; //weak ref
                    break;
                case RXN_ROLE_ACTIVATOR:
                    curv = new ActCurve();
                    curv->as = &n->_p;
                    curv->ns = n;
                    curv->owns = 0; //weak ref
                    curv->ae = &_p;
                    curv->owne = 0; //weak ref
                    break;
                case RXN_ROLE_INHIBITOR:
                    curv = new InhCurve();
                    curv->as = &n->_p;
                    curv->ns = n;
                    curv->owns = 0; //weak ref
                    curv->ae = &_p;
                    curv->owne = 0; //weak ref
                    break;
                default:
                    std::cerr << "Unrecognized role type\n";
                    AN(0, "Unrecognized role type");
            }
            AN(curv, "Failed to create curve");
# if REBUILD_CURVES_DIAG
            std::cerr << "  Curve type: " << CurveTypeToString(curv->getRole()) << "\n";
# endif
            
            curv->setTransform(tf_);
            curv->setInverseTransform(itf_);
            _curv.push_back(curv);
        }
        
        recalcCurveCPs();
        
        _cdirty = 0;

//         std::cerr << "Done rebuilding curves\n";
    }

#define PRINT_CURVE_DIAG 0

# if PRINT_CURVE_DIAG
    static bool filterRxn(Reaction* rxn) {
      for(Reaction::NodeIt i=rxn->NodesBegin(); i!=rxn->NodesEnd(); ++i) {
        Node* n = i->first;
        if(n->getId() == "glyceraldehyde_3_phosphate" && i->second == RXN_ROLE_PRODUCT)
          return false;
      }
      return true;
    }
# endif

    void Reaction::recalcCurveCPs() {
//         std::cerr << "recalcCurveCPs\n";
        uint64 csub=0;
        Point ctrlCent(0,0);
        Point loopPt;
        bool looped = false;

        for(ConstNodeIt i=NodesBegin(); i!=NodesEnd(); ++i) {
            Node* n = i->first;
            RxnRoleType r = i->second;
            AN(n);

            switch(r) {
                case RXN_ROLE_SUBSTRATE:
                case RXN_ROLE_SIDESUBSTRATE:
                    // control pt stuff
                    ctrlCent += n->getCentroid();
#if PRINT_CURVE_DIAG
                    if (!filterRxn(this))
                      std::cerr << "  ctrlCent substrate used: " << n->getCentroid() << "\n";
#endif
                    csub++;
                    for(ConstNodeIt j=NodesBegin(); j!=NodesEnd(); ++j) {
                        Node* nn = j->first;
                        if(nn == n && r !=  j->second) {
                            looped = true;
                            loopPt = nn->getCentroid();
                        }
                    }
                    break;
                case RXN_ROLE_PRODUCT:
                case RXN_ROLE_SIDEPRODUCT:
                case RXN_ROLE_MODIFIER:
                case RXN_ROLE_ACTIVATOR:
                case RXN_ROLE_INHIBITOR:
                    break;
                default:
                    std::cerr << "Unrecognized role type2\n";
                    AN(0, "Unrecognized species type");
            }

        }
        
#if PRINT_CURVE_DIAG
        if (!filterRxn(this)) {
          //  use mock centroid position from C# version
          setCentroid(Point(359, 258));
          std::cerr << "centroid pos: " << getCentroid() << "\n";
        }
#endif

        ctrlCent = (ctrlCent+_p) * (1. / (csub+1));
        
#if PRINT_CURVE_DIAG
        if (!filterRxn(this))
          std::cerr << "ctrlCent first value: " << ctrlCent << "\n";
#endif

        if(looped) {
            const Real d = -25.;
            ctrlCent = _p + (_p - loopPt);

            ctrlCent = new2ndPos(loopPt, _p, 0., d, false);

            ctrlCent = new2ndPos(_p, ctrlCent, -90., 0., false);
        }
        
#if PRINT_CURVE_DIAG
        if (!filterRxn(this))
          std::cerr << "ctrlCent loop correction: " << ctrlCent << "\n";
#endif

        // Correction applied to uni-uni reactions
        if (NetworkElement::degree() == 2) {
            double d = -(_p - ctrlCent).mag();
//             std::cerr << "  uni-uni dist: " << d << "\n";
            Point p1, p2;

            for(ConstNodeIt i=NodesBegin(); i!=NodesEnd(); ++i) {
                Node* n = i->first;
                RxnRoleType r = i->second;

                switch(r) {
                    case RXN_ROLE_SUBSTRATE:
                    case RXN_ROLE_SIDESUBSTRATE:
                        p2 = n->getMin();
//                         std::cerr << "  uni-uni sub: " << n->getMin() << "\n";
                        break;
                    case RXN_ROLE_PRODUCT:
                    case RXN_ROLE_SIDEPRODUCT:
                        p1 = n->getMin();
//                         std::cerr << "  uni-uni prod: " << n->getMin() << "\n";
                        break;
                    case RXN_ROLE_MODIFIER:
                    case RXN_ROLE_ACTIVATOR:
                    case RXN_ROLE_INHIBITOR:
                        break;
                    default:
#if PRINT_CURVE_DIAG
                        if (!filterRxn(this))
                          std::cerr << "Unrecognized species type\n";
#endif
                        AN(0, "Unrecognized species type");
                }
            }

            ctrlCent = _p + (p2 - p1);
            ctrlCent = new2ndPos(ctrlCent, _p, 0, d, false);
        }
        
#if PRINT_CURVE_DIAG
        if (!filterRxn(this))
          std::cerr << "ctrlCent uni-uni correction: " << ctrlCent << "\n";
#endif

        // keep dir, subtract 25 from length
        ctrlCent = new2ndPos(ctrlCent, _p, 0., -25., false);
//         ctrlCent = new2ndPos(ctrlCent, _p, 180., 0., false);
//         ctrlCent = new2ndPos(ctrlCent, _p, 0., 50., false);
        
#if PRINT_CURVE_DIAG
        if (!filterRxn(this))
          std::cerr << "ctrlCent adjust length: " << ctrlCent << "\n";
#endif

        // control points
        for(CurveIt i=CurvesBegin(); i!=CurvesEnd(); ++i) {
            RxnBezier* c = *i;
            AN(c);
            RxnCurveType role = c->getRole();

            Box bs(*c->as-Point(30,20), *c->as+Point(30,20));
            Box be(*c->ae-Point(30,20), *c->ae+Point(30,20));

            switch(role) {
                case RXN_CURVE_SUBSTRATE:
#if PRINT_CURVE_DIAG
                    if (!filterRxn(this))
                      std::cerr << "SUBSTRATE\n";
#endif
                    c->s = calcCurveBackup(ctrlCent, *c->as, c->ns ? c->ns->getBoundingBox() : bs, 10.);
                    c->c1 = new2ndPos(_p, c->s, 0., -20., false);
                    c->e = *c->ae;
//                     std::cerr << "* Substrate endpoint: " << c->e << "\n";
                    c->c2 = ctrlCent;
                    break;
                case RXN_CURVE_PRODUCT:
#if PRINT_CURVE_DIAG
                    if (!filterRxn(this)) {
                      std::cerr << "PRODUCT\n";
                      std::cerr << "  rxn " << getId() <<  ", ctrlCent: " << ctrlCent << ", _p: " << _p << ", *c->ae: " << *c->ae << ", ne: " << c->ne->getId() << ", ne coords: " << c->ne->getCentroid() << ", ne corner: " << c->ne->getBoundingBox().getMin() << "\n";
                    }
#endif
                    c->s = *c->as;
//                     std::cerr << "* Product startpoint: " << c->s << "\n";
                    c->c1 = new2ndPos(ctrlCent, _p, 0., 1., true);
                    c->e = calcCurveBackup(c->c1, *c->ae, c->ne ? c->ne->getBoundingBox() : be, 10.);
                    c->c2 = new2ndPos(_p, c->e, 0., -20., false);
                    break;
                case RXN_CURVE_ACTIVATOR:
                case RXN_CURVE_INHIBITOR:
                case RXN_CURVE_MODIFIER:
#if PRINT_CURVE_DIAG
                    if (!filterRxn(this))
                      std::cerr << "MODIFIER\n";
#endif
                    c->s  = calcCurveBackup(_p, *c->as, c->ns ? c->ns->getBoundingBox() : bs, 10.);
                    c->c1 = new2ndPos(*c->as, _p, 0., -15., false);
                    c->e = c->c1;
                    c->c2 = new2ndPos(*c->as, _p, 0., -20., false);
                    break;
                default:
                    AN(0, "Unrecognized curve type");
                    c->s = calcCurveBackup(_p, *c->as, c->ns ? c->ns->getBoundingBox() : bs, 10.);
                    c->c1 = c->s;
                    c->e = _p;
                    c->c2 = _p;
                    break;
            }
#if PRINT_CURVE_DIAG
            if (!filterRxn(this))
              std::cout << "curve " << *c << "\n";
#endif
        }
        
#if PRINT_CURVE_DIAG
        if (!filterRxn(this))
          std::cerr << "ctrlCent after curves: " << ctrlCent << "\n";
#endif
//         for(CurveIt i=CurvesBegin(); i!=CurvesEnd(); ++i) {
//             RxnBezier* c = *i;
//             AN(c);
//             RxnCurveType role = c->getRole();
//             switch(role) {
//                 case RXN_CURVE_SUBSTRATE:
//                     std::cerr << "Substrate curve: " << c->s << " to " << c->e << "\n";
//                     break;
//                 case RXN_CURVE_PRODUCT:
//                     std::cerr << "Product curve: " << c->s << " to " << c->e << "\n";
//                     break;
//                 default:
//                     break;
//             }
//         }
        int k_i = 0;
        for(CurveIt i=CurvesBegin(); i!=CurvesEnd(); ++i) {
          RxnBezier* c1 = *i;
          AN(c1);

          CurveIt j=i;
          ++j;
          for(; j!=CurvesEnd(); ++j) {
            RxnBezier* c2 = *j;
            AN(c2);

            if (c1->getNodeUsed() != NULL && c1->getNodeUsed() == c2->getNodeUsed() && c1->getRole() == c2->getRole()) {

#if PRINT_CURVE_DIAG
              if (!filterRxn(this)) {
                std::cerr << "c1 node min: " << c1->getNodeUsed()->getMin() << ", c1 node cent: " << c1->getNodeUsed()->getCentroid() << ", c1 node: " << c1->getNodeSide() << ", c1 node cp: " << c1->getNodeSideCP() << "\n";
                std::cerr << "c2 node min: " << c2->getNodeUsed()->getMin() << ", c2 node cent: " << c2->getNodeUsed()->getCentroid() << ", c2 node: " << c2->getNodeSide() << ", c2 node cp: " << c2->getNodeSideCP() << "\n";
              }
#endif

              c1->setNodeSideCP(new2ndPos(c1->getNodeUsed()->getCentroid(), c1->getNodeSideCP(),  20., 10., false));
              c2->setNodeSideCP(new2ndPos(c2->getNodeUsed()->getCentroid(), c2->getNodeSideCP(), -20., 10., false));

              c1->setNodeSide(new2ndPos(c1->getNodeSideCP(), c1->getNodeSide(), -20., 0., false));
              c2->setNodeSide(new2ndPos(c2->getNodeSideCP(), c2->getNodeSide(),  10., 0., false));
            }
          }
#if PRINT_CURVE_DIAG
          if (!filterRxn(this))
            std::cerr << "curve " << k_i++ << " getNodeSide: " << c1->getNodeSide() << ", getNodeSideCP: " << c1->getNodeSideCP() << "\n";
#endif
        }
    }
    
    void Reaction::recenter() {
//         std::cerr << "RECENTER\n";
        uint32 count=0;
        _p = Point(0.,0.);
        for(ConstNodeIt i=NodesBegin(); i!=NodesEnd(); ++i) {
            Node* n = i->first;
            _p = _p + n->getCentroid();
            ++count;
        }
        // normalize
        _p = _p*(1./count);
        rebuildCurves();
    }
    
    void Reaction::recompCentroid() {
//         std::cerr << "RECOMP CENTROID\n";
        if(isCentroidSet())
            return;
        doCentroidCalc();
    }
    
    void Reaction::forceRecalcCentroid() {
//         std::cerr << "RECALC CENTROID\n";
        doCentroidCalc();
        _pset = 1;
    }

    void Reaction::doCentroidCalc() {
      uint32 count=0;
      _p = Point(0.,0.);
      for(ConstNodeIt i=NodesBegin(); i!=NodesEnd(); ++i) {
        // detect duplicates
        for(ConstNodeIt j=NodesBegin(); j!=i; ++j)
          if (i->first == j->first)
            goto doCentroidCalc_skip;

        {
          Node* n = i->first;
          _p = _p + n->getCentroid();
          ++count;
        }

        doCentroidCalc_skip:;
      }
      // normalize
      _p = _p*(1./count);
    }
    
    void Reaction::deleteCurves() {
        for(CurveVec::iterator i=_curv.begin(); i!=_curv.end(); ++i) {
            delete *i;
        }
        _curv.clear();
    }
    
    void Reaction::dump(std::ostream& os, uint32 ind) {
        indent(os, ind);
        os << "Reaction:\n";
        indent(os, ind+2);
        os << "ID: \"" << _id << "\"\n";
        indent(os, ind+2);
        os << "Degree: " << _deg << "\n";
        indent(os, ind+2);
        os << "Local degree: " << _ldeg << "\n";
        indent(os, ind+2);
        os << "Species: \n";
        for(ConstNodeIt i = _spec.begin(); i!= _spec.end(); ++i) {
            indent(os, ind+4);
            os << i->first->getId() << "(" << i->first->getGlyph() << "), role: " << rxnRoleToString(i->second) << "\n";
        }
    }
    
    void Reaction::dumpForces(std::ostream& os, uint32 ind) const {
        indent(os, ind);
        os << "Reaction forces: " << _v << "\n";
    }
    
    //--CLASS Compartment--
    
    void Compartment::addElt(NetworkElement* e) {
        _elt.push_back(e);
    }
    
    bool Compartment::containsElt(const NetworkElement* e) const {
        for(ConstEltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            const NetworkElement* x = *i;
            if(x == e)
                return true;
        }
        return false;
    }
    
    void Compartment::removeElt(NetworkElement* e) {
        for(EltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            if(*i == e) {
                printf("Element erased\n");
                _elt.erase(i);
                return;
            }
        }
    }
    
    void Compartment::setRestExtents(const Box& ext) {
        _ext = ext;
        _ra = _ext.area();
    }
    
    void Compartment::resizeEnclose() {
        Real minx, miny, maxx, maxy;
        EltIt i=EltsBegin();
        if(i != EltsEnd()) {
            NetworkElement* e = *i;
            minx = e->getMinX();
            miny = e->getMinY();
            maxx = e->getMaxX();
            maxy = e->getMaxY();
            ++i;
        }
        for(; i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            minx = min(minx, e->getMinX());
            maxx = max(maxx, e->getMaxX());
            miny = min(miny, e->getMinY());
            maxy = max(maxy, e->getMaxY());
        }
        _ext = Box(Point(minx,miny), Point(maxx,maxy));
        _ra = _ext.area();
    }
    
    void Compartment::autoSize() {
        uint64 count = _elt.size();
        Real dim = 350*sqrt((Real)count);
        // avoid singularities in layout algo
        Point shake((rand()%1000)/100.,(rand()%1000)/100.);
        _ext = Box(Point(0,0) + shake, Point(dim,dim) + shake);
		//_ext = Box(Point(0, 0), Point(dim, dim));
        _ra = _ext.area();
    }
    
    void Compartment::resetActivity() {
        _v = Point(0,0);
        // now calculate stress due to being stretched beyond rest area
        // this stress always acts to shrink the comp
        Real w = _ext.width(), h = _ext.height();
        Real d2 = _ext.area()-_ra;
        // strain (liberally speaking), evenly distributed along all axes
        Real strain = sign(d2)*sqrt(mag(d2) / _ra);
        _fx1 = _res*_E*strain*w;
        _fy1 = _res*_E*strain*h;
        _fx2 = -_res*_E*strain*w;
        _fy2 = -_res*_E*strain*h;
    }
    
    void Compartment::applyBoundaryForce(const Real fx1, const Real fy1, const Real fx2, const Real fy2) {
        _fx1 += fx1;
        _fy1 += fy1;
        _fx2 += fx2;
        _fy2 += fy2;
    }
    
    void Compartment::doInternalForce(NetworkElement* e, const Real f, const Real t) {
        Real x1=_ext.getMin().x, y1 = _ext.getMin().y, x2 = _ext.getMax().x, y2 = _ext.getMax().y;
        Real invt = 1./t;
        
        Real eminx = e->getMinX();
        Real eminy = e->getMinY();
        Real emaxx = e->getMaxX();
        Real emaxy = e->getMaxY();
        
        // compute forces
        Real fx1 = f*exp((x1-eminx)*invt);
        Real fx2 = -f*exp((emaxx-x2)*invt);
        Real fy1 = f*exp((y1-eminy)*invt);
        Real fy2 = -f*exp((emaxy-y2)*invt);
        
        // do forces on element
        e->addDelta(Point(fx1+fx2, fy1+fy2));
        
        // do forces on container
        applyBoundaryForce(-fx1, -fx2, -fy1, -fy2);
        addDelta(-Point(fx1+fx2,fy1+fy2));
    }
    
    void Compartment::doInternalForceAll(const Real f, const Real t) {
        for(EltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            doInternalForce(e, f, t);
        }
    }
    
    void Compartment::doMotion(const Real scale_) {
        if(_lock)
            return;
        const Real scale = 0.2*scale_;
        Real w = _ext.width(), h = _ext.height();
        // adjust the extents based on Hooke's law of elasticity
        // forces -> stress -> strain -> displacement
        _ext.setMin(_ext.getMin() + (scale/_E)*Point(_fx1*w/h, _fy1*h/w) + scale*_v);
        _ext.setMax(_ext.getMax() + (scale/_E)*Point(_fx2*w/h, _fy2*h/w) + scale*_v);
        if(_ext.width() < 10.)
            _ext.setWidth(10.);
        if(_ext.height() < 10.)
            _ext.setHeight(10.);
        //recalc centroid?
    }
    
    void Compartment::capDelta2(const Real cap2) {
        _v.capMag2_(cap2);
        const Real cap = sqrt(cap2);
        if(mag(_fx1) > cap)
            _fx1 = sign(_fx1)*cap;
        if(mag(_fy1) > cap)
            _fy1 = sign(_fy1)*cap;
        if(mag(_fx2) > cap)
            _fx2 = sign(_fx2)*cap;
        if(mag(_fy2) > cap)
            _fy2 = sign(_fy2)*cap;
    }
    
    bool Compartment::contains(const NetworkElement* e) const {
        for(ConstEltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            if(*i == e)
                return true;
        }
        return false;
    }
    
    void Compartment::dump(std::ostream& os, uint32 ind) {
        indent(os, ind);
        os << "Compartment:\n";
        indent(os, ind+2);
        os << "ID: \"" << _id << "\"\n";
        indent(os, ind+2);
        os << "Glyph: \"" << _gly << "\"\n";
        indent(os, ind+2);
        os << "Extents: " << _ext << "\n";
    }
    
    void Compartment::dumpForces(std::ostream& os, uint32 ind) const {
        indent(os, ind);
        os << "Compartment forces: " << _fx1 << ", " << _fy1 << ", " << _fx2 << ", " << _fy2 << "), Centroid forces: " << _v << "\n";
    }
    
    //--CLASS Network--
    
    void Network::hierarchRelease() {
        // FIXME: replace with hierarch free
        for(NodeVec::iterator i=_nodes.begin(); i!=_nodes.end(); ++i) {
            // (*i)->hierarchRelease();
            delete *i;
        }
        for(RxnVec::iterator i=_rxn.begin(); i!=_rxn.end(); ++i) {
            (*i)->hierarchRelease();
            delete *i;
        }
        for(CompVec::iterator i=_comp.begin(); i!=_comp.end(); ++i) {
            // (*i)->hierarchRelease();
            delete *i;
        }
    }
    
    void Network::addNode(Node* n) {
        AN(n, "No node to add");
        _nodes.push_back(n);
        addElt(n);
    }
    
    void Network::removeReactionsForNode(Node* n) {
        for(RxnVec::iterator i=_rxn.begin(); i!=_rxn.end(); ++i) {
            (*i)->removeNode(n);
        }
    }
    
    void Network::removeNode(Node* n) {
        AN(n, "No node to remove");
        // remove from element container
        removeElt(n);
        // remove from compartments
        for(CompIt i=CompsBegin(); i!=CompsEnd(); ++i) {
            Compartment* c = *i;
            c->removeElt(n);
        }
        removeReactionsForNode(n);
        for(NodeVec::iterator i=_nodes.begin(); i!=_nodes.end(); ++i) {
            Node* x = *i;
            if(x == n) {
                _nodes.erase(i);
                std::cout << "Removed node " << n << "\n";
                return;
            }
        }
        SBNW_THROW(InvalidParameterException, "No such node", "Network::removeNode");
    }
    
    Node* Network::findNodeById(const std::string& id) {
        for(NodeVec::iterator i=_nodes.begin(); i!=_nodes.end(); ++i) {
            Node* n = *i;
            if(n->getId() == id)
                return n;
        }
        //not found
        return NULL;
    }
    
    const Node* Network::findNodeById(const std::string& id) const {
        for(NodeVec::const_iterator i=_nodes.begin(); i!=_nodes.end(); ++i) {
            const Node* n = *i;
            if(n->getId() == id)
                return n;
        }
        //not found
        return NULL;
    }
    
    std::string Network::getUniqueId() const {
        std::size_t k=0;
        std::string id;
        const Node* n=NULL;
        
        do {
            ++k;
            std::stringstream ss;
            ss << "Node_" << k;
            id = ss.str();
            std::cout << "Trying " << id << "\n";
        } while(findNodeById(id));
        
        std::cout << "Unique ID: " << id << "\n";
        
        return id;
    }
    
    std::size_t Network::getUniqueIndex() const {
        std::cout << "getUniqueIndex started\n";
        std::size_t k=0;
        
        repeat:
        for(NodeVec::const_iterator i=_nodes.begin(); i!=_nodes.end(); ++i) {
            const Node* node = *i;
            if(node->get_i() == k) {
                ++k;
                goto repeat;
            }
        }
        
        return k;
    }
    
    Node* Network::findNodeByGlyph(const std::string& gly) {
        for(NodeVec::iterator i=_nodes.begin(); i!=_nodes.end(); ++i) {
            Node* n = *i;
            if(n->getGlyph() == gly)
                return n;
        }
        //not found
        return NULL;
    }
    
    bool Network::containsNode(const Node* n) const {
        for(NodeVec::const_iterator i=_nodes.begin(); i!=_nodes.end(); ++i) {
            const Node* x = *i;
            if(x == n)
                return true;
        }
        return false;
    }

    Network::AttachedRxnList Network::getConnectedReactions(const Node* n) {
        AttachedRxnList result;
        for(ConstRxnIt i = RxnsBegin(); i != RxnsEnd(); ++i) {
            Reaction* x = *i;
            if(x->hasSpecies(n))
                result.push_back(x);
        }
        return result;
    }

    Network::AttachedCurveList Network::getAttachedCurves(const Node* n) {
        AttachedRxnList rxns = getConnectedReactions(n);
        AttachedCurveList result;
        for (AttachedRxnList::iterator i = rxns.begin(); i != rxns.end(); ++i) {
            Reaction* r = *i;
            for (Reaction::CurveIt j = r->CurvesBegin(); j != r->CurvesEnd(); ++j) {
                RxnBezier* c = *j;
                if (c->includes(n))
                    result.push_back(c);
            }
        }
        return result;
    }

    int Network::getNumSubgraphs() {
        enumerateSubgraphs();
        return nsub_;
    }

    void Network::enumerateSubgraphs() {
        nsub_ = 0;
        loop:
        for(NodeVec::const_iterator i=_nodes.begin(); i!=_nodes.end(); ++i) {
            Node* x = *i;
            if(!x->isSetSubgraphIndex()) {
                propagateSubgraphIndex(x, nsub_++);
                goto loop;
            }
        }
    }

    void Network::propagateSubgraphIndex(Node* x, int isub) {
        AT(!x->isSetSubgraphIndex(), "Subgraph index is already set");
        x->setSubgraphIndex(isub);
        for(RxnVec::iterator i=_rxn.begin(); i!=_rxn.end(); ++i) {
            Reaction* r = *i;
            if(r->hasSpecies(x)) {
                for(Reaction::NodeIt j=r->NodesBegin(); j!=r->NodesEnd(); ++i) {
                    if (!j->first->isSetSubgraphIndex())
                        propagateSubgraphIndex(j->first, isub);
                }
            }
        }
    }

    void Network::clearSubgraphInfo() {
        for(NodeVec::const_iterator i=_nodes.begin(); i!=_nodes.end(); ++i) {
            Node* x = *i;
            x->clearSubgraphIndex();
        }
    }

    void Network::clearExcludeFromSubgraphEnum() {
        for(NodeVec::const_iterator i=_nodes.begin(); i!=_nodes.end(); ++i) {
            Node* x = *i;
            x->clearExcludeFromSubgraphEnum();
        }
    }
    
    Reaction* Network::findReactionById(const std::string& id) {
        for(RxnVec::iterator i=_rxn.begin(); i!=_rxn.end(); ++i) {
            Reaction* r = *i;
            if(r->getId() == id)
                return r;
        }
        //not found
        return NULL;
    }
    
    Compartment* Network::findCompById(const std::string& id) {
        for(CompVec::iterator i=_comp.begin(); i!=_comp.end(); ++i) {
            Compartment* c = *i;
            if(c->getId() == id)
                return c;
        }
        //not found
        return NULL;
    }
    
    Compartment* Network::findCompByGlyph(const std::string& gly) {
        for(CompVec::iterator i=_comp.begin(); i!=_comp.end(); ++i) {
            Compartment* c = *i;
            if(c->getGlyph() == gly)
                return c;
        }
        //not found
        return NULL;
    }
    
    void Network::resetUsageInfo() {
        for(NodeIt i=NodesBegin(); i!=NodesEnd(); ++i) {
            Node* n = *i;
            n->numUses() = 0;
        }
    }
    
    void Network::addReaction(Reaction* rxn) {
        AN(rxn);
        _rxn.push_back(rxn);
        addElt(rxn);
    }
    
    void Network::removeReaction(Reaction* r) {
        AN(r, "No reaction to remove");
        // remove from element container
        removeElt(r);
        for(RxnVec::iterator i=_rxn.begin(); i!=_rxn.end(); ++i) {
            Reaction* x = *i;
            if(x == r) {
                _rxn.erase(i);
                std::cout << "Removed reaction " << r << "\n";
                return;
            }
        }
        SBNW_THROW(InvalidParameterException, "No such reaction", "Network::removeReaction");
    }
    
    void Network::elideEmptyComps() {
        // replace in elt vec
        EltVec w;
        for(EltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            if(e->getType() == NET_ELT_TYPE_COMP) {
                Graphfab::Compartment* c = (Graphfab::Compartment*)e;
                if(!c->empty())
                    w.push_back(c);
            } else
                w.push_back(e);
        }
        _elt.swap(w);
        
        // replace in comp vec & delete empty ones
        CompVec v;
        for(CompIt i=CompsBegin(); i!=CompsEnd(); ++i) {
            Compartment* c = *i;
            if(!c->empty())
                v.push_back(c);
            else
                delete c;
        }
        _comp.swap(v);
    }
    
    Compartment* Network::findContainingCompartment(const NetworkElement* e) {
        for(CompIt i=CompsBegin(); i!=CompsEnd(); ++i) {
            Compartment* c = *i;
            if(c->containsElt(e))
                return c;
        }
        return NULL;
    }
    
    Box Network::getBoundingBox() const {
        Box b;
        {
            ConstEltIt i=EltsBegin();
            if(i == EltsEnd())
                return b;
            NetworkElement* e = *i;
            b = e->getBoundingBox();
        }
//         std::cerr << "Network initial bounding box: " << b << "\n";
        for(ConstEltIt i=EltsBegin()+1; i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            b.expandx(e->getBoundingBox());
//             std::cerr << "  Expand by: " << e->getBoundingBox() << "\n";
//             std::cerr << "  new bounding box: " << b << "\n";
        }
//         std::cerr << "Network bounding box: " << b << "\n";
        return b;
    }
    
    void Network::fitToWindow(const Box& w) {
        Graphfab::Affine2d tf = Graphfab::Affine2d::FitToWindow(getBoundingBox(), w);
//         std::cerr << "Applying tf:\n" << tf;
        setTransform(tf);
        setInverseTransform(tf.inv());
    }
    
    void Network::applyTransform(const Affine2d& t) {
        for(EltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            e->applyTransform(t);
        }
    }
    
    void Network::setTransform(const Affine2d& t, bool recurse) {
        for(EltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            e->setTransform(t, recurse);
        }
    }
    
    void Network::setInverseTransform(const Affine2d& it, bool recurse) {
        for(EltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            e->setInverseTransform(it, recurse);
        }
    }
    
    void Network::resetActivity() {
        for(EltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            e->resetActivity();
        }
    }

    void Network::updatePositions(const Real scale) {
        for(EltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            e->doMotion(scale);
        }
    }
    
    void Network::resizeCompsEnclose() {
        for(CompIt i=CompsBegin(); i!= CompsEnd(); ++i) {
            Compartment* c = *i;
            c->resizeEnclose();
        }
    }
    
    void Network::autosizeComps() {
        for(CompIt i=CompsBegin(); i!= CompsEnd(); ++i) {
            Compartment* c = *i;
            c->autoSize();
        }
    }
    
    void Network::updateExtents() {
        for(EltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            e->recalcExtents();
        }
    }
    
    void Network::capDeltas(const Real cap) {
        for(EltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            e->capDelta2(cap*cap);
        }
    }
    
    Point Network::pmean() const {
        Point m(0.,0.);
        uint64 c=0;
        for(ConstEltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            m = m + e->getCentroid();
            ++c;
        }
        m = m * (1./c);
        return m;
    }
    
    Point Network::center() const {
        return getExtents().getCenter();
    }
    
    Box Network::getExtents() const {
        if(EltsBegin() == EltsEnd()) return Box();
        Box m((*EltsBegin())->getExtents());
        
        for(ConstEltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            if(e->getMinX() < m.getMinX())
                m.setMinX(e->getMinX());
            if(e->getMinY() < m.getMinY())
                m.setMinY(e->getMinY());
            if(e->getMaxX() > m.getMaxX())
                m.setMaxX(e->getMaxX());
            if(e->getMaxY() > m.getMaxY())
                m.setMaxY(e->getMaxY());
        }
        
        return m;
    }
    
    void Network::recenter(const Point& p) {
        Point m(pmean());
        Point d = p-m;
        for(EltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            e->setCentroid(e->getCentroid() + d);
        }
    }
    
    Point Network::pvariance() const {
        Point m(pmean());
        Point d(0.,0.);
        uint64 c=0;
        for(ConstEltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            d = d + Point(e->getCentroid() - m).squareTerms();
            ++c;
        }
        d = d.sqrtTerms() * (1./c);
        return d;
    }
    
    void Network::randomizePositions(const Box& b) {
        for(NodeVec::iterator i=_nodes.begin(); i!=_nodes.end(); ++i) {
            Node* n = *i;
            if(n->isLocked())
                break;
            n->setCentroid(rand_range(b.getMin().x, b.getMax().x),
                           rand_range(b.getMin().y, b.getMax().y));
        }
        for(RxnVec::iterator i=_rxn.begin(); i!=_rxn.end(); ++i) {
            Reaction* r = *i;
            if(r->isLocked())
                break;
            r->setCentroid(Point(rand_range(b.getMin().x, b.getMax().x),
                            rand_range(b.getMin().y, b.getMax().y)));
        }
        for(CompIt i=CompsBegin(); i!=CompsEnd(); ++i) {
            Graphfab::Compartment* c = *i;
            if(c->isLocked())
                break;
            Real d = sqrt(c->restArea());
            Point p(rand_range(b.getMin().x, b.getMax().x),
                    rand_range(b.getMin().y, b.getMax().y));
            Point dim(d, d);
            c->setExtents(Box(p-dim, p+dim));
        }
        recalcCurveCPs();
        //dump(std::cout, 0);
    }
    
    void Network::rebuildCurves() {
        for(RxnIt i=RxnsBegin(); i!=RxnsEnd(); ++i) {
            Reaction* r = *i;
            r->rebuildCurves();
        }
    }

    void Network::recalcCurveCPs() {
        for(RxnIt i=RxnsBegin(); i!=RxnsEnd(); ++i) {
            Reaction* r = *i;
            r->recalcCurveCPs();
        }
    }
    
    void Network::recenterJunctions() {
//         std::cerr << "Recenter junctions\n";
        for(RxnIt i=RxnsBegin(); i!=RxnsEnd(); ++i) {
            Reaction* r = *i;
            r->recenter();
        }
    }
    
    // IO/Diagnostics:
    
    void Network::dump(std::ostream& os, uint32 ind) {
        indent(os, ind);
        os << "Network:\n";
        for(ConstEltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            e->dump(os, ind+2);
        }
    }
    
    void Network::dumpEltForces(std::ostream& os, uint32 ind) const {
        for(ConstEltIt i=EltsBegin(); i!=EltsEnd(); ++i) {
            NetworkElement* e = *i;
            e->dumpForces(os, ind+2);
        }
    }
    
    //--GLOBAL--

    Network* networkFromLayout(const Layout& lay, const Model& mod) {
        Network* net= networkFromModel(mod);
        
        // used to compute aliases
        net->resetUsageInfo();
        
        //add additional information from layout
        // for compartments
        for(int i=0; i<lay.getNumCompartmentGlyphs(); ++i) {
            const CompartmentGlyph* cg = lay.getCompartmentGlyph(i);
            
            Graphfab::Compartment* c = net->findCompById(cg->getCompartmentId());
            
            c->setGlyph(cg->getId());
            
            const BoundingBox* bbox = cg->getBoundingBox();
            
            c->setRestExtents(Box(Point(bbox->x(), bbox->y()), Point(bbox->x()+bbox->width(), bbox->y()+bbox->height())));
        }

        // place elements inside parent compartments
        for(Network::NodeIt z=net->NodesBegin(); z!=net->NodesEnd(); ++z) {
          Node* n = *z;
          Compartment* c = net->findContainingCompartment(n);
          if (c)
            n->setCentroid(c->getCentroid());
        }
        for(Network::RxnIt z=net->RxnsBegin(); z!=net->RxnsEnd(); ++z) {
          Reaction* r = *z;
          Compartment* c = net->findContainingCompartment(r);
          if (c)
            r->setCentroid(c->getCentroid());
        }
        
        // for nodes
        for(int i=0; i<lay.getNumSpeciesGlyphs(); ++i) {
            const SpeciesGlyph* sg = lay.getSpeciesGlyph(i);
            
            Node* n = net->findNodeById(sg->getSpeciesId());
            AN(n, "No such node exists");
            
//             std::cerr << "Species glyph: " << sg->getSpeciesId() << "(" << sg->getId() << ")\n";
            
            //increment usage counter (used to find aliases)
            if(n->numUses() == 0) {
                n->numUses()++;
                n->setGlyph(sg->getId());
            } else {
                //create an alias node
                n->setAlias(true);
                n = new Node(*n);
                n->setGlyph(sg->getId());
                n->_ldeg = 0;
                //add alias node to the network
                net->addNode(n);
            }
            
            const BoundingBox* bb = sg->getBoundingBox();
            
            n->setCentroid(Point(bb->x() + bb->width()/2., bb->y() + bb->height()/2.));
            n->setWidth(bb->width());
            n->setHeight(bb->height());
        }
        
        // for reactions
        for(int i=0; i<lay.getNumReactionGlyphs(); ++i) {
            const ReactionGlyph* rg = lay.getReactionGlyph(i);
//             std::cerr << "Read ReactionGlyph " << rg->getId() << "\n";
            
            Reaction* r = net->findReactionById(rg->getReactionId());
            AN(r, "No such reaction");
            
            for(int i_spc=0; i_spc<rg->getNumSpeciesReferenceGlyphs(); ++i_spc) {
                const SpeciesReferenceGlyph* srg = rg->getSpeciesReferenceGlyph(i_spc);

                //get the alias
//                 std::cerr << "Searching for glyph " << srg->getSpeciesGlyphId() << "\n";
                Node* alias = net->findNodeByGlyph(srg->getSpeciesGlyphId());
                AN(alias, "Unable to find alias node");

                //fix the reference to point to the alias
//                 r->substituteSpeciesById(srg->getSpeciesReferenceId(), alias);
//                 std::cerr << srg->getId() << ": ";
//                 if (!r->hasSpecies(alias))
//                   std::cerr << "Not present\n";
//                 else
//                   std::cerr << "Is present\n";
                RxnRoleType role = SBMLRole2GraphfabRole(srg->getRole());
//                 if (isRoleActive(role))
                    r->substituteSpeciesByIdwRole(srg->getSpeciesReferenceId(), alias, role);
//                 else
//                     r->addSpeciesRef(alias, role);

//                 for (Reaction::CurveIt ci = r->CurvesBegin(); ci != r->CurvesEnd(); ++ci) {
//                   RxnBezier* c = *ci;
//                   std::cerr << "  ci\n";
//                   if (c->includes(alias)) {
//                     std::cerr << "  includes node\n";
//                   }
//                 }
            }

            // delete preexisting curves
            r->deleteCurves();

            ::Curve const* curve = rg->getCurve();
            ::BoundingBox const* sbml_bb = rg->getBoundingBox();
            
            // calculate the centroid
//             if (!curve)
//               std::cerr << rg->getId() << ": no curve\n";
//             else
//               std::cerr << rg->getId() << ": have curve, n segs = " << curve->getNumCurveSegments() << "\n";

            // first try bounding box (the proper method, which none of the models use)
            if (sbml_bb &&
             !(sbml_bb->getPosition()->x() == 0 && sbml_bb->getPosition()->y() == 0 &&
             sbml_bb->getDimensions() && sbml_bb->getDimensions()->getWidth() == 0 && sbml_bb->getDimensions()->getHeight() == 0) ) {

                Real x_offset = 0, y_offset = 0;
                if (sbml_bb->getDimensions())
                  x_offset = sbml_bb->getDimensions()->getWidth() * 0.5, y_offset = sbml_bb->getDimensions()->getHeight() * 0.5;
                r->setCentroid(sbml_bb->getPosition()->x(), sbml_bb->getPosition()->y());
            } else if (curve && curve->getNumCurveSegments() > 0) {
                // next try using preexisting centroid coords via reaction curve
//                 std::cerr << "manual centroid\n";
                r->setCentroid(curve->getCurveSegment(0)->getEnd()->x(), curve->getCurveSegment(0)->getEnd()->y());

                for (unsigned int j = 0; j < rg->getNumSpeciesReferenceGlyphs(); ++j) {
                    RxnBezier* c = r->addCurve(SBMLRole2GraphfabRole(rg->getSpeciesReferenceGlyph(j)->getRole()));
//                     std::cerr << "  Created curve with role " << rxnRoleToString(SBMLRole2GraphfabRole(rg->getSpeciesReferenceGlyph(j)->getRole())) << "\n";
                    Node* target = net->findNodeByGlyph(rg->getSpeciesReferenceGlyph(j)->getSpeciesGlyphId());

                    if (c->getRole() == RXN_CURVE_PRODUCT) {
                        c->as = &r->_p;
                        c->ae = &target->_p;
                        c->ne = target;
                    } else {
                        c->as = &target->_p;
                        c->ns = target;
                        c->ae = &r->_p;
                    }
                    c->owne = 0;
                    c->owns = 0;
                }

                r->recalcCurveCPs();

                r->clearDirtyFlag();

                // Try to fill in the CP data from layout info
                for (unsigned int j = 0; j < rg->getNumSpeciesReferenceGlyphs(); ++j) {
                  SpeciesReferenceGlyph const* srg = rg->getSpeciesReferenceGlyph(j);
                  RxnBezier* c = r->getCurve(j);
                  // use the first curve segment
                  ::Curve const* sr_curve = srg->getCurve();
                  ::LineSegment const* sr_line = sr_curve->getCurveSegment(0);
                  ::CubicBezier const* sr_bez = dynamic_cast< ::CubicBezier const* >(sr_line);
                  if (sr_bez) {
                    //std::cerr << "sr_bez\n";
                    c->s.x = sr_bez->getStart()->x();
                    c->s.y = sr_bez->getStart()->y();
                    c->e.x = sr_bez->getEnd()->x();
                    c->e.y = sr_bez->getEnd()->y();

                    c->c1.x = sr_bez->getBasePoint1()->x();
                    c->c1.y = sr_bez->getBasePoint1()->y();
                    c->c2.x = sr_bez->getBasePoint2()->x();
                    c->c2.y = sr_bez->getBasePoint2()->y();
                  } else if (sr_line) {
                    //std::cerr << "no sr_bez\n";
                    c->s.x = sr_line->getStart()->x();
                    c->s.y = sr_line->getStart()->y();
                    c->e.x = sr_line->getEnd()->x();
                    c->e.y = sr_line->getEnd()->y();

                    c->c1.x = sr_line->getStart()->x();
                    c->c1.y = sr_line->getStart()->y();
                    c->c2.x = sr_line->getEnd()->x();
                    c->c2.y = sr_line->getEnd()->y();
                    //  CPs should be separated from endpoints for endcap orientation
                    Point ctmp = c->c1;
                    c->c1 = 0.9*c->c1 + 0.1*c->c2;
                    c->c2 = 0.9*c->c2 + 0.1*ctmp;
                  }
                }
            } else {
                // poor results
                if (false) {
                  // try to get the centroid from the reaction curves
                  for(int i_spc=0; i_spc<rg->getNumSpeciesReferenceGlyphs(); ++i_spc) {
                      // spec ref
                      const SpeciesReferenceGlyph* srg = rg->getSpeciesReferenceGlyph(i_spc);
                      // role
                      RxnRoleType role = SBMLRole2GraphfabRole(srg->getRole());
                      // curve
                      ::Curve const* crv = srg->getCurve();
                      if (!crv)
                        break;

                      for (int i_seg=0; i_seg<crv->getNumCurveSegments(); ++i_seg) {
                        ::LineSegment const* seg = crv->getCurveSegment(i_seg);
                        if (role == RXN_ROLE_PRODUCT || role == RXN_ROLE_SIDEPRODUCT)
                          r->setCentroid(seg->getStart()->x(), seg->getStart()->y());
                        else
                          r->setCentroid(seg->getEnd()->x(), seg->getEnd()->y());
                        goto skip_recalc_centroid;
                      }
                  }
                }

                // if all else fails average node coords
                r->forceRecalcCentroid();

                skip_recalc_centroid:;
            }

//             r->rebuildCurves();
//             std::cerr << "Reaction centroid: " << r->getCentroid(NetworkElement::COORD_SYSTEM_GLOBAL) << "\n";
        }
        
        net->setLayoutSpecified(true);
        
        return net;
    }

    Network* networkFromModel(const Model& mod) {
        Network* net = new Network();
        
        // add compartments
        for(int i=0; i<mod.getNumCompartments(); ++i) {
            const ::Compartment* comp = mod.getCompartment(i);
            
            // elide "default" compartments based on SBO
            if(comp->isSetSBOTerm() && comp->getSBOTerm() == 410) {
                continue;
            }
            
            // assume a compartment with the id "default" or "compartment" represents
            // a default, non-visual compartment, so discard it from the model
            if(comp->getId() != "default" && comp->getId() != "compartment" && comp->getId() != "graphfab_default_compartment") {
                Graphfab::Compartment* c = new Compartment();
                
                // set id
                c->setId(comp->getId());
                
                // add to network
                net->addCompartment(c);
            }
        }
        
        // add nodes
        //printf("# floating = %d\n", floating);
        for(int i=0; i<mod.getNumSpecies(); ++i) {
            Node* n = new Node();
            
            const Species* s = mod.getSpecies(i);
            
            AN(s, "Failed to get species");
            #if SAGITTARIUS_DEBUG_LEVEL >= 2
//             std::cout << "Species: name: " << s->getName() << ", id: " << s->getId() << "\n";
            #endif
            
            n->setName(s->getName());
            n->setId(s->getId());
            
            //no alias info
            n->numUses() = 1;
            n->setAlias(false);
            
            // associate compartment (if one exists)
            Graphfab::Compartment* c = net->findCompById(s->getCompartment());
            if(c) {
                c->addElt(n);
                n->_comp = c;
            }
            
            // set index
            n->set_i((size_t)i);
            
            // add to network
            net->addNode(n);
        }
        
        // remove empty compartments
        net->elideEmptyComps();
        
        // resize compartments to enclose contents
        //NOTE: nodes do not yet have positions, can't do this
        //net->resizeCompartments();
        net->autosizeComps();
        
        // add connections
        for(int i_rxn=0; i_rxn<mod.getNumReactions(); ++i_rxn) {
            const ::Reaction* rxn = mod.getReaction(i_rxn);
            Reaction* r = new Reaction();
            
            r->setId(rxn->getId());
            
            AN(rxn, "Failed to get reaction");

            // associate compartment (if one exists)
            Graphfab::Compartment* c = net->findCompById(rxn->getCompartment());
            if(c) {
                c->addElt(r);
            }
            
            // get reactants
            for(int i_spc=0; i_spc<rxn->getNumReactants(); ++i_spc) {
                //get the reference
                const SpeciesReference* spc = rxn->getReactant(i_spc);
                Node* src = net->findNodeById(spc->getSpecies());
                AN(src, "Invalid species reference");
                r->addSpeciesRef(src, RXN_ROLE_SUBSTRATE);
            }
            
            // get products
            for(int i_spc=0; i_spc<rxn->getNumProducts(); ++i_spc) {
                //get the reference
                const SpeciesReference* spc = rxn->getProduct(i_spc);
                Node* src = net->findNodeById(spc->getSpecies());
                AN(src, "Invalid species reference");
                r->addSpeciesRef(src, RXN_ROLE_PRODUCT);
            }
            
            // get modifiers
            for(int i_spc=0; i_spc<rxn->getNumModifiers(); ++i_spc) {
                //get the reference
                const ModifierSpeciesReference* spc = rxn->getModifier(i_spc);
                Node* src = net->findNodeById(spc->getSpecies());
                AN(src, "Invalid species reference");
                r->addSpeciesRef(src, RXN_ROLE_MODIFIER);
            }
            
            net->addReaction(r);
        }
        
        #if SAGITTARIUS_DEBUG_LEVEL >= 3
        net->dump(std::cout, 0);
        #endif
        
        return net;
    }

}
