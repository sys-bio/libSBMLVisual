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

/** @file network.h
 * @brief The network model
  */

//== BEGINNING OF CODE ===============================================================

#ifndef __SBNW_NETWORK_H_
#define __SBNW_NETWORK_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"
#include "graphfab/layout/point.h"
#include "graphfab/layout/curve.h"
#include "graphfab/layout/box.h"
#include "graphfab/math/transform.h"

//-- C++ code --
#ifdef __cplusplus

#include "graphfab/sbml/autolayoutSBML.h"

#include "sbml/SBMLTypes.h"
#include "sbml/packages/layout/common/LayoutExtensionTypes.h"

#include <string>
#include <iostream>
#include <typeinfo>
#include <stdint.h>

using namespace libsbml;

namespace Graphfab {
    
    typedef enum {
        NET_ELT_TYPE_SPEC,
        NET_ELT_TYPE_RXN,
        NET_ELT_TYPE_COMP
    } NetworkEltType;
    
    std::string eltTypeToStr(const NetworkEltType t);
    
    void dumpEltType(std::ostream& os, const NetworkEltType t, uint32 ind);
    
    /// Returns true if either a or b equals z
    inline bool typeMatchEither(const NetworkEltType a, const NetworkEltType b, const NetworkEltType z) {
        if(a == z)
            return true;
        else if(b == z)
            return true;
        else
            return false;
    }
    
    class Compartment;

    bool haveDefaultCompartmentId();

    void setDefaultCompartmentId(const std::string& id);

    std::string getDefaultCompartmentId();

    /** @brief Curve for a substrate
     */
    class SubCurve : public RxnBezier {
        public:
            /// Get the role of this curve
            RxnCurveType getRole() const {
                return RXN_CURVE_SUBSTRATE;
            }

            Point getCentroidCP() const { return c2; }

            bool isStartNodeSide() const { return true; }

            virtual bool hasArrowhead() const { return true; }

            ArrowheadStyle getArrowheadStyle() const;

            /// Callee owns
            virtual Arrowhead* getArrowhead() {
              Arrowhead* result = new SubstrateArrowhead();
              transformArrowhead(*result);
              return result;
            }
    };

    /** @brief Curve for a product
     */
    class PrdCurve : public RxnBezier {
        public:
            /// Get the role of this curve
            RxnCurveType getRole() const {
                return RXN_CURVE_PRODUCT;
            }

            Point getCentroidCP() const { return c1; }

            bool isStartNodeSide() const { return false; }

            virtual bool hasArrowhead() const { return true; }

            ArrowheadStyle getArrowheadStyle() const;

            /// Callee owns
            virtual Arrowhead* getArrowhead() {
              Arrowhead* result = new ProductArrowhead();
              transformArrowhead(*result);
              return result;
            }
    };

    /** @brief Curve for an activator
     */
    class ActCurve : public RxnBezier {
        public:
            /// Get the role of this curve
            RxnCurveType getRole() const {
                return RXN_CURVE_ACTIVATOR;
            }

            Point getCentroidCP() const { return c2; }

            bool isStartNodeSide() const { return true; }

            virtual bool hasArrowhead() const { return true; }

            ArrowheadStyle getArrowheadStyle() const;

            /// Callee owns
            virtual Arrowhead* getArrowhead() {
              Arrowhead* result = new ActivatorArrowhead();
              transformArrowhead(*result);
              return result;
            }
    };

    /** @brief Curve for an inhibitor
     */
    class InhCurve : public RxnBezier {
        public:
            /// Get the role of this curve
            RxnCurveType getRole() const {
                return RXN_CURVE_INHIBITOR;
            }

            Point getCentroidCP() const { return c2; }

            bool isStartNodeSide() const { return true; }

            virtual bool hasArrowhead() const { return true; }

            ArrowheadStyle getArrowheadStyle() const;

            /// Callee owns
            virtual Arrowhead* getArrowhead() {
              Arrowhead* result = new InhibitorArrowhead();
              transformArrowhead(*result);
              return result;
            }
    };

    /** @brief Curve for a modifier
     */
    class ModCurve : public RxnBezier {
        public:
            /// Get the role of this curve
            RxnCurveType getRole() const {
                return RXN_CURVE_MODIFIER;
            }

            Point getCentroidCP() const { return c2; }

            bool isStartNodeSide() const { return true; }

            virtual bool hasArrowhead() const { return true; }

            ArrowheadStyle getArrowheadStyle() const;

            /// Callee owns
            virtual Arrowhead* getArrowhead() {
              Arrowhead* result = new ModifierArrowhead();
              transformArrowhead(*result);
              return result;
            }
    };

    extern ArrowheadStyle sub_arrow_style_;
    inline ArrowheadStyle ArrowheadStyleLookup(const SubCurve* ) {
      return sub_arrow_style_;
    }

    extern ArrowheadStyle prod_arrow_style_;
    inline ArrowheadStyle ArrowheadStyleLookup(const PrdCurve* ) {
      return prod_arrow_style_;
    }

    extern ArrowheadStyle act_arrow_style_;
    inline ArrowheadStyle ArrowheadStyleLookup(const ActCurve* ) {
      return act_arrow_style_;
    }

    extern ArrowheadStyle inh_arrow_style_;
    inline ArrowheadStyle ArrowheadStyleLookup(const InhCurve* ) {
      return inh_arrow_style_;
    }

    extern ArrowheadStyle mod_arrow_style_;
    inline ArrowheadStyle ArrowheadStyleLookup(const ModCurve* ) {
      return mod_arrow_style_;
    }
    
    /// The shape of a visual network element can assume
    typedef enum {
        ELT_SHAPE_ROUND,
        ELT_SHAPE_RECT
    } NetworkEltShape;
    
    /** @brief An element that can be connected to other elements in the network
     */
    class NetworkElement {
        public:

            enum COORD_SYSTEM {
              COORD_SYSTEM_LOCAL,
              COORD_SYSTEM_GLOBAL
            };
            
            NetworkElement()
                : _pset(0), _v(0,0), _deg(0), _ldeg(0), _lock(0), networkEltBytePattern_(0x1199) {}
            
            /// Get the type
            NetworkEltType getType() const { return _type; }
            
            bool hasNetworkElementBase() {
                if(networkEltBytePattern_ == 0x1199)
                    return true;
                else
                    return false;
            }
            
            /// degree (number of connections)
            void set_degree(uint64 deg) { _deg = deg; }
            virtual uint64 degree() const { return _deg; }
            virtual uint64& degree() { return _deg; }
            
            /// Reset the force (delta)
            virtual void resetActivity();
            
            /// Adjust the velocity (set v = v + d)
            void addDelta(const Point& d);
            
            /// Cap the velocity
            void capDelta(const Real cap);
            
            /// Cap the velocity (faster)
            virtual void capDelta2(const Real cap2);
            
            /// Update the position
            virtual void doMotion(const Real scale);
            
            /// Set the centroid of the node
            virtual void setCentroid(const Point& p);
            virtual void setGlobalCentroid(const Point& p);
            void setCentroid(Real x, Real y) { setCentroid(Point(x,y)); }
            
            virtual bool isCentroidSet() const { return _pset; }
            
            /// Get the centroid of the node
            virtual Point getCentroid(COORD_SYSTEM coord = COORD_SYSTEM_LOCAL) const;
            
//             SAGITTARIUS_DEPRECATED(Point getGlobalCentroid() const) { return tf_*_p; }
            
            /// Extents functions that all elements must support
            Point getMin(COORD_SYSTEM coord = COORD_SYSTEM_LOCAL) const { return getExtents(coord).getMin(); }
            Point getMax(COORD_SYSTEM coord = COORD_SYSTEM_LOCAL) const { return getExtents(coord).getMax(); }
            
            Real getMinX() const { return getExtents().getMin().x; }
            Real getMaxX() const { return getExtents().getMax().x; }
            Real getMinY() const { return getExtents().getMin().y; }
            Real getMaxY() const { return getExtents().getMax().y; }
            
            /// With/height derived from extents
            Real getWidth() const { AT(getMaxX() >= getMinX()); return getMaxX() - getMinX(); }
            Real getHeight() const { AT(getMaxY() >= getMinY()); return getMaxY() - getMinY(); }
            
            /// With/height derived from extents
            Real getGlobalWidth() const { AT(getMaxX() >= getMinX()); return (getMaxX() - getMinX())*tf_.scaleFactor(); }
            Real getGlobalHeight() const { AT(getMaxY() >= getMinY()); return (getMaxY() - getMinY())*tf_.scaleFactor(); }
            
            /** @brief Get bounding box
             */
            virtual Box getExtents(COORD_SYSTEM coord = COORD_SYSTEM_LOCAL) const {
              switch (coord) {
                case COORD_SYSTEM_LOCAL:
                  return getLocalExtents();
                case COORD_SYSTEM_GLOBAL:
                  return tf_*getLocalExtents();
                default:
                  AN(0, "Unknown coord system");
                  return getLocalExtents();
              }
            }

            /** @brief Get bounding box w/o transform
             */
            virtual Box getLocalExtents() const {
              return _ext;
            }
//             virtual SAGITTARIUS_DEPRECATED(Box getGlobalExtents() const) { return tf_*_ext; }
            
            /// Set the extents of the compartment
            void setExtents(const Box& b) { _ext = b; recalcCentroid(); }
            
            Box getBoundingBox() const { return getExtents(); }
//             Box getBoundingBox() const { return Box(); }
            
            virtual void applyTransform(const Affine2d& t) {
                _ext = xformBox(_ext, t);
                _p = xformPoint(_p, t);
            }

            virtual void applyDisplacement(const Point& d) {
                _ext.displace(d);
                _p += d;
            }
            
            /// Calculate the centroid based on the extents
            void recalcCentroid() { _p = (_ext.getMin() + _ext.getMax())*0.5; }
            
            /// Recalculate the extents
            virtual void recalcExtents() = 0;
            
            /// Lock
            void lock() { _lock = 1; }
            
            /// Unlock
            void unlock() { _lock = 0; }

            // Is the node locked?
            bool isLocked() const { return _lock; }
            
            NetworkEltShape getShape() const { return _shape; }
            
            //TODO: cache in member & ditch v func
            virtual bool isContainer() const = 0;
            
            /// Get the radius (approx. for non-round)
            Real radius() const { return _r; }
            
            /// Get the distance to another element
            Real distance(const NetworkElement& e) const;
            
            /// Does this element overlap with e?
            bool overlap(const NetworkElement& e) const;
            
            /// Get the force vector between this and another element
            // TODO: rename; is actually displacement
            Point forceVec(const NetworkElement& e) const;

            /// Get the centroid displacement between this and another element
            Point centroidDisplacementFrom(const NetworkElement& e) const;
            
            /// Messier but perhaps slightly faster
            void forceVec_(const NetworkElement& e, Point& p) const;
            
            virtual void dump(std::ostream& os, uint32 ind) = 0;
            
            /// Dump info about forces
            virtual void dumpForces(std::ostream& os, uint32 ind) const = 0;
            
            virtual Affine2d getTransform() const { return tf_; }
            
            virtual void setTransform(const Affine2d& tf, bool recurse = true) { tf_ = tf; }
            
            virtual Affine2d getInverseTransform() const { return itf_; }
            
            virtual void setInverseTransform(const Affine2d& itf, bool recurse = true) { itf_ = itf; }
            
            /// Centroid
            Point _p;
            /// Degree
            uint64 _deg;
            /// Local degree (unique connections for each aliased copy)
            uint64 _ldeg;
        protected:
            /// True if centroid was set externally
            int _pset;
            /// Change in centroid (computed by layout algorithm)
            Point _v;
            /// Extents (should be updated by derived classes)
            Box _ext;
            /// Radius
            Real _r;
            /// Shape
            NetworkEltShape _shape;
            /// Type
            NetworkEltType _type;
            /// Locked?
            int _lock;
            /// Transform
            Affine2d tf_;
            /// Inverse transform
            Affine2d itf_;
            
            long networkEltBytePattern_;
    };

    class Network;

    /** @brief Node in a network
     */
    class Node : public NetworkElement {
        public:
            
            Node()
                : NetworkElement() {
                    _shape = ELT_SHAPE_RECT;
                    _comp = NULL;
                    _type = NET_ELT_TYPE_SPEC;
                    _ext = Box(0,0,40,20);
                    bytepattern = 0xc455;
                    isub_ = -1;
                }
            
            // Model:
            
            /// Set the species' name
            void setName(const std::string& name);

            /// Get the species' name
            const std::string& getName() const;
            
            /// Get the node id
            const std::string& getId() const;
            
            /// Set the node id
            void setId(const std::string& id);
            
            /// Get the species' reaction glyph (layout element)
            const std::string& getGlyph() const;
            
            /// Set the species' reaction glyph (layout element)
            void setGlyph(const std::string& id);
            
            // Alias info:
            
            /// accessor for _numUses
            uint32& numUses() { return _numUses; }
            uint32 numUses() const { return _numUses; }
            
            /// Is this node an alias node?
            //TODO: change to isAliased. There is no such thing as an "alias node"
            bool isAlias() const { return _isAlias; }

            /// Return true if both nodes are instances of the same species
            bool isCommonInstance(const Node* other) const;
            
            /// Specify if this node is an alias or not
            void setAlias(bool b) { _isAlias = b; }

            int alias(Network* net);

            int getSubgraphIndex() const {
                if (isub_ < 0)
                    SBNW_THROW(InvalidParameterException, "No subgraph index set", "Network::getSubgraphIndex");
                return isub_;
            }

            void setSubgraphIndex(int v) { isub_ = v; }

            bool isSetSubgraphIndex() { return isub_ < 0; }

            void clearSubgraphIndex() { isub_ = -1; }

            bool excludeFromSubgraphEnum() const { return exsub_; }

            bool setExcludeFromSubgraphEnum() { return exsub_ = true; }

            void clearExcludeFromSubgraphEnum() { exsub_ = false; }
            
            // Coordinates/dimensions:
            
            /// Get coords of upper left-hand corner
            Point getUpperLeftCorner() const;
            /// Get coords of lower right-hand corner
            Point getLowerRightCorner() const;
            
            void recalcExtents() {
                Real width = _ext.width();
                Real height = _ext.height();
                Point del(0.5*width, 0.5*height);
                
                _ext = Box(_p - del, _p + del);
                _r = _ext.maxDim()*0.5;
            }
            
            /// Set width
            void setWidth(Real w);
            
            /// Set height
            void setHeight(Real h);
            
            /// Set width
            void affectGlobalWidth(Real w);
            
            /// Set height
            void affectGlobalHeight(Real h);
            
            void set_i(size_t i) { i_ = i; } 
            size_t get_i() const { return i_; }
            
            // Layout:
            
            bool isContainer() const { return false; }
            
            // IO:
            
            /// Dump to a stream
            void dump(std::ostream& os, uint32 ind);
            
            /// Dump info about forces
            void dumpForces(std::ostream& os, uint32 ind) const;
            
            Compartment* _comp;
            
            bool doByteCheck() { if(bytepattern == 0xc455) return true; else return false; }
            
            long bytepattern;
        protected:
            // model info:
            std::string _name, _id;
            /// Reaction glyph
            std::string _gly;
            // aliasing:
            uint32 _numUses;
            bool _isAlias;
            // rendering:
            /// Half the width and height, resp
//             Real _hemiw, _hemih;
            
            // index in network
            size_t i_;
            int isub_;
            bool exsub_;
    };
    
    /// Does runtime type checking
    inline Node* CastToNode(void* p) {
        NetworkElement* e = (NetworkElement*)p;
        AN(e->hasNetworkElementBase(), "Runtime type check failed");
//         std::cout << typeid((Node&)*e).name() << std::endl;
        AN(dynamic_cast<Node*>(e), "Runtime type check failed");
        return dynamic_cast<Node*>(e);
//         Node* x = (Node*)e;
//         AN(x->doByteCheck(), "Runtime type check failed");
//         return x;
    }
    
    typedef enum {
        RXN_ROLE_SUBSTRATE,
        RXN_ROLE_PRODUCT,
        RXN_ROLE_SIDESUBSTRATE,
        RXN_ROLE_SIDEPRODUCT,
        RXN_ROLE_MODIFIER,
        RXN_ROLE_ACTIVATOR,
        RXN_ROLE_INHIBITOR,
    } RxnRoleType;

    class RxnCurveFactory {
        public:
            static RxnBezier* CreateCurve(RxnRoleType role);
    };
    
    /** @brief Represents a single reaction
     */
    class Reaction : public NetworkElement {
        public:
            // Exposed types:
            typedef std::pair<Node*, RxnRoleType> SpeciesElt;
            /// Contains species and their respective roles
            typedef std::vector< SpeciesElt > NodeVec;
            /// Role container
            //typedef std::vector<RxnRoleType> RoleVec;
            /// Curve container
            typedef std::vector<RxnBezier*> CurveVec;
            
            // Iterators:
            typedef NodeVec::iterator NodeIt;
            typedef NodeVec::const_iterator ConstNodeIt;
            
            /*typedef RoleVec::iterator RoleIt;
            typedef RoleVec::const_iterator ConstRoleIt;*/
            
            typedef CurveVec::iterator CurveIt;
            typedef CurveVec::const_iterator ConstCurveIt;
            
            NodeIt NodesBegin() { return _spec.begin(); }
            NodeIt NodesEnd() { return _spec.end(); }
            
            ConstNodeIt NodesBegin() const { return _spec.begin(); }
            ConstNodeIt NodesEnd() const { return _spec.end(); }
            
            CurveIt CurvesBegin() { return _curv.begin(); }
            CurveIt CurvesEnd() { return _curv.end(); }
            
            ConstCurveIt CurvesBegin() const { return _curv.begin(); }
            ConstCurveIt CurvesEnd() const { return _curv.end(); }
            
            // Methods:
            
            Reaction()
                : NetworkElement() {
                    _shape = ELT_SHAPE_ROUND;
                    _type = NET_ELT_TYPE_RXN;
                    bytepattern = 0xff83;
                }
            
            void hierarchRelease();
            
            // Model:
            
            /// Get ID
            const std::string& getId() const { return _id; }
            
            /// Set ID
            void setId(const std::string& id) { _id = id; }

            void setName(const std::string& name) { name_ = name; }
            
            // Species:
            
            /// Get the number of species
            uint64 numSpecies() const { return _spec.size(); }
            
            /// Include species in reaction (weak ref)
            void addSpeciesRef(Node* n, RxnRoleType role);
            
            /// Remove the node if it is part of the reaction (do nothing otherwise)
            void removeNode(Node* n);
            
            /// Find the species by specified ID. Returns NULL if no such species exists
            Node* findSpeciesById(const std::string& id);
            
            /// Determine if the reaction has a given species
            bool hasSpecies(const Node* n) const;

            /// Return number of times @ref n participates in this reaction
            uint64 degree(const Node* n);
            
            RxnRoleType getSpeciesRole(size_t i) { return _spec.at(i).second; }

            RxnRoleType getSpeciesRole(Node* n);
            
            Node* getSpecies(size_t i) { return _spec.at(i).first; }
            
            /** @brief Substitute the new node for any species with given id
             * @details Redirects the edges currently pointing to the node with
             * @a id to the node @a spec
             */
            void substituteSpeciesById(const std::string& id, Node* spec);

            /** @brief Same as @ref substituteSpeciesById but overwrite role
             */
            void substituteSpeciesByIdwRole(const std::string& id, Node* spec, RxnRoleType role);
            
            /** @brief Substitute the new node for any species
             */
            void substituteSpecies(Node* before, Node* after);
            
            /// Get the curves
            CurveVec& getCurves();
            // this space intentionally left blank
            
            /// Get number of curves
            size_t getNumCurves() { curveGuard(); return _curv.size(); }
            
            
            /// Get curve at index
            RxnBezier* getCurve(size_t i) { curveGuard(); return _curv.at(i); }
            
            /// Get the nodes
            NodeVec& getSpec() { return _spec; }
            const NodeVec& getSpec() const { return _spec; }
            
            /// Recomputes the centroid
            void forceRecalcCentroid();
            
            /** @brief Rebuild the curves
             * @details WARNING: This invalidates any curve iterators
             */
            void rebuildCurves();
            
            /**
             * @brief Only recalculates control points, does not rebuild curves
             * @details Second half of rebuildCurves
             */
            void recalcCurveCPs();
            
            /** @brief Recenter at the mean centroid of connected nodes
             */
            void recenter();
            
            // Layout
            
            bool isContainer() const { return false; }
            
            /// Get the radius (approx. for non-round)
            void recalcExtents() {
                _r = 10.;
                _ext = Box(_p - Point(_r,_r), _p + Point(_r,_r));
            }

            virtual Box getLocalExtents() const {
              return Box(getCentroid() - Point(5, 5), getCentroid() + Point(5, 5));
            }
            
            virtual void applyTransform(const Affine2d& t) {
                NetworkElement::applyTransform(t);
                for(CurveIt i = CurvesBegin(); i != CurvesEnd(); ++i) {
                    (*i)->applyTransform(t);
                }
            }
            
            virtual void setTransform(const Affine2d& tf, bool recurse = true) {
                tf_ = tf;
                for(CurveIt i = CurvesBegin(); i != CurvesEnd(); ++i) {
                    (*i)->setTransform(tf);
                }
            }
            
            virtual void setInverseTransform(const Affine2d& itf, bool recurse = true) {
                itf_ = itf;
                for(CurveIt i = CurvesBegin(); i != CurvesEnd(); ++i) {
                    (*i)->setInverseTransform(itf);
                }
            }
            
            // IO
            void dump(std::ostream& os, uint32 ind);
            
            /// Dump info about forces
            void dumpForces(std::ostream& os, uint32 ind) const;
            
            bool doByteCheck() { if(bytepattern == 0xff83) return true; else return false; }

            void clearDirtyFlag() { _cdirty = false; }

            /// Returns weak ref
            RxnBezier* addCurve(RxnRoleType role) {
                _curv.push_back(RxnCurveFactory::CreateCurve(role));
                _curv.back()->setTransform(itf_);
                _curv.back()->setInverseTransform(itf_);
                return _curv.back();
            }

            /// Delete the curves
            void deleteCurves();
            
        protected:
            // Methods:
            
            /// Rebuilds curves & recomputes centroid
            void rebuildAll();
            
            /// Recomputes the centroid
            void recompCentroid();

            /// Numeric centroid computation, no other side effects
            void doCentroidCalc();
            
            void curveGuard() {
                if(_cdirty && _spec.size()) {
                    rebuildCurves();
//                     recenter();
                }
            }
            
            // Variables:
            // model:
            std::string _id;
            std::string name_;
            // dims:
            /// Force (translates to velocity, not accel, due to layout algo.)
            Point _v;
            /// Reactants (weak ref)
            //NodeVec _rct;
            /// Products (weak ref)
            //NodeVec _prd;
            /// Participating species
            NodeVec _spec;
            /// Species' roles
            //RoleVec _role;
            /// Curves
            CurveVec _curv;
            /// Do curves need to be rebuilt?
            bool _cdirty;
            
            long bytepattern;
    };
    
    /// Does runtime type checking
    inline Reaction* CastToReaction(void* p) {
        NetworkElement* e = (NetworkElement*)p;
        AN(e->hasNetworkElementBase(), "Runtime type check failed");
        AN(dynamic_cast<Reaction*>(e), "Runtime type check failed");
        return dynamic_cast<Reaction*>(e);
    }
    
    typedef enum {
        COMP_EDGE_TYPE_TOP,
        COMP_EDGE_TYPE_LEFT,
        COMP_EDGE_TYPE_BOTTOM,
        COMP_EDGE_TYPE_RIGHT
    } CompartmentEdgeType;
    
    /** @brief Compartment (for holding species)
     */
    class Compartment : public NetworkElement {
        public:
            
            // Exposed types:
            
            /// Element container (weak refs)
            typedef std::vector<Graphfab::NetworkElement*> EltVec;
            
            typedef EltVec::iterator EltIt;
            typedef EltVec::const_iterator ConstEltIt;
            
            EltIt EltsBegin() { return _elt.begin(); }
            EltIt EltsEnd() { return _elt.end(); }
            
            ConstEltIt EltsBegin() const { return _elt.begin(); }
            ConstEltIt EltsEnd() const { return _elt.end(); }
            
            Graphfab::NetworkElement* getElt(const uint64 i) { return _elt.at(i); }
            
            const Graphfab::NetworkElement* getElt(const uint64 i) const { return _elt.at(i); }
            
            uint64 getNElts() const { return _elt.size(); }
            
            Compartment()
                : /*_nu(0.3),*/ _ra(50.*50.), _E(10.), _res(0.25), bytepattern(0xffae11), NetworkElement() {
                    _shape = ELT_SHAPE_RECT;
                    _type = NET_ELT_TYPE_COMP;
                }
            
            /// Get the compartment's id
            const std::string& getId() const { return _id; }
            
            /// Set the compartment's id
            void setId(const std::string& id) { _id = id; }

            /// Set the compartment's name
            void setName(const std::string& name) { name_ = name; }
            
            /// Get the compartment's glyph (layout element)
            const std::string& getGlyph() const { return _gly; }
            
            /// Set the compartment's glyph (layout element)
            void setGlyph(const std::string& glyph) { _gly = glyph; }
            
            void setCentroid(const Point& p) {
                AN(0, "setCentroid should not be called on a compt");
            }
            
            /// Approximate size; used by distance algorithms etc.
            void recalcExtents() {
                _r = _ext.maxDim()*0.5;
                _p = (_ext.getMin() + _ext.getMax())*0.5;
            }
            
            // Elements
            
            /// Add an element to the compartment
            void addElt(NetworkElement* e);
            
            bool containsElt(const NetworkElement* e) const;
            
            /// Remove an element from the compartment (does not call destructor)
            void removeElt(NetworkElement* e);
            
            /// Manually size the compartment
            void setRestExtents(const Box& ext);
            
            /// Permanently resize extents based on distribution contained elements
            void resizeEnclose(double padding = 0);
            
            /// Used when no layout information is available; sizes to square with area based on number of elts
            void autoSize();
            
            /// Rest area
            Real restArea() const { return _ra; }
            
            void setMin(const Point& p) { _ext.setMin(p); }
            void setMax(const Point& p) { _ext.setMax(p); }

            virtual Point getCentroid(COORD_SYSTEM coord = COORD_SYSTEM_LOCAL) const { return getExtents(coord).getCenter(); }
            
            // Layout engine:
            
            /// Reset the force (delta)
            virtual void resetActivity();
            
            /** @brief Applies a force to an edge of the compartment
             * @param[in] fx1 Force on leftmost vertical edge
             * @param[in] fy1 Force on leftmost horizontal edge
             * @param[in] fx2 Force on rightmost vertical edge
             * @param[in] fy2 Force on rightmost horizontal edge
             */
            void applyBoundaryForce(const Real fx1, const Real fy1, const Real fx2, const Real fy2);
            
            /** @brief Apply contact force for a particular element
             * @param[in] e The internal element
             * @param[in] f The magnitude of the force
             * @param[in] t The falloff
             */
            void doInternalForce(NetworkElement* e, const Real f, const Real t);
            
            /// Apply contact force for all internal elements
            void doInternalForceAll(const Real f, const Real t);
            
            /// Update the dynamics
            void doMotion(const Real scale);
            
            void capDelta2(const Real cap2);
            
            bool isContainer() const { return true; }
            
            /// Does this compartment contain a certain element?
            bool contains(const NetworkElement* e) const;
            
            /// Is the container empty?
            bool empty() const { return _elt.size() ? false : true; }
            
            /// Dump to a stream
            void dump(std::ostream& os, uint32 ind);
            
            /// Dump info about forces
            void dumpForces(std::ostream& os, uint32 ind) const;

            bool doByteCheck() { if(bytepattern == 0xffae11) return true; else return false; }
            
        protected:
            /// ID
            std::string _id;
            /// name
            std::string name_;
            /// glyph
            std::string _gly;
            /// Elements
            EltVec _elt;
            /// Rest area
            Real _ra;
            /// Young's modulus
            Real _E;
            /// Poisson's ratio
            //Real _nu;
            /// Forces
            Real _fx1, _fy1, _fx2, _fy2;
            /// Restoring force multiplier
            Real _res;

            uint64_t bytepattern;
    };

    /** @brief Network topology
     */
    class Network : public Compartment {
        public:
            // Exposed types:
            
            /// Node container
            typedef std::vector<Node*> NodeVec;
            /// Reaction container
            typedef std::vector<Graphfab::Reaction*> RxnVec;
            /// Compartment container
            typedef std::vector<Graphfab::Compartment*> CompVec;
            
            //iterators
            typedef NodeVec::iterator NodeIt;
            typedef NodeVec::const_iterator ConstNodeIt;
            
            typedef RxnVec::iterator RxnIt;
            typedef RxnVec::const_iterator ConstRxnIt;
            
            typedef CompVec::iterator CompIt;
            typedef CompVec::const_iterator ConstCompIt;
            
            // Constructors:
            
            Network() {
                bytepattern = 0x3355;
                layoutspecified_ = false;
            }
            
            // Methods:
            
            /// Destructor
            void hierarchRelease();
            
            // Nodes:
            
            /// Add an unlinked node to the network
            void addNode(Node* n);
            
            /** Remove a node and all reaction connections to/from 
                the node from the network (does not free memory) */
            void removeNode(Node* n);

            /** Connect a node to a reaction */
            void connectNode(Node* n, Reaction* r, RxnRoleType role);

            /** Return true if the node is already connected */
            bool isNodeConnected(Node* n, Reaction* r) const;
            
            /// Find the node by specified ID. Returns NULL if no such node exists
            Node* findNodeById(const std::string& id);
            const Node* findNodeById(const std::string& id) const;
            
            /// Generated unique ID for creating new nodes
            std::string getUniqueId() const;

            std::string getUniqueGlyphId(const Node& src) const;
            
            /// Generated unique index for creating new nodes
            std::size_t getUniqueIndex() const;
            
            /// Find the node by specified reaction glyph (from layout package)
            Node* findNodeByGlyph(const std::string& gly);
            
            Node* getNodeAt(const size_t i) { return _nodes.at(i); }

            Node* getUniqueNodeAt(const size_t n);

            size_t getNumInstances(const Node* u);

            /// Get a node in an alias group by instance index
            Node* getInstance(const Node* u, const size_t n);
            
            bool containsNode(const Node* n) const;

            bool containsReaction(const Reaction* r) const;

            typedef std::vector<Reaction*> AttachedRxnList;

            AttachedRxnList getConnectedReactions(const Node* n);

            typedef std::vector<RxnBezier*> AttachedCurveList;

            AttachedCurveList getAttachedCurves(const Node* n);

            int getNumSubgraphs();

            /// Enumerates all the subgraphs of the network and assigns each a unique index
            void enumerateSubgraphs();

            /// Assigns the index to all nodes in the subgraph containing @ref x
            void propagateSubgraphIndex(Node* x, int isub);

            void clearSubgraphInfo();

            void clearExcludeFromSubgraphEnum();
            
            /// Find the reaction by specified ID. Returns NULL if no such reaction exists
            Reaction* findReactionById(const std::string& id);
            
            Reaction* getRxnAt(const size_t i) { return _rxn.at(i); }
            
            /// Resets _numUses on each node
            void resetUsageInfo();
            
            /// Get the network's id
            const std::string& getId() const { return _id; }

            void setId(const std::string& id) { _id = id; }

            bool isSetId() const { return _id.size(); }
            
            bool isLayoutSpecified() const { return layoutspecified_; }
            
            void setLayoutSpecified(bool value) {
                layoutspecified_ = value;
            }
            
            // Reactions:
            
            /// Add a reaction
            void addReaction(Reaction* rxn);
            
            /// Remove a reaction
            void removeReaction(Reaction* r);
            
            // Compartments:
            
            /// Add a compartment
            void addCompartment(Compartment* c) { _comp.push_back(c); addElt(c); }
            
            /** @brief Find a compartment by id
             * @param[in] id Id of compartment elt
             */
            Compartment* findCompById(const std::string& id);
            
            /** @brief Find the compartment associated with a given glyph
             * @param[in] gly Id of compartment glyph
             */
            Compartment* findCompByGlyph(const std::string& gly);
            
            Compartment* getCompAt(const size_t i) { return _comp.at(i); }
            
            Compartment* findContainingCompartment(const NetworkElement* e);
            
            // Layout:
            
            uint64 getTotalNumComps() const { return _comp.size(); }
            
            /// Returns # of species + rxns
            uint64 getTotalNumPts() const { return _nodes.size() + _rxn.size(); }

            /// Returns # of rxns
            uint64 getTotalNumRxns() const { return _rxn.size(); }

            /// Returns # of species
            uint64 getTotalNumNodes() const { return _nodes.size(); }

            uint64 getNumUniqueNodes() const;

            Box getBoundingBox() const;
            
            void fitToWindow(const Box& w);
            
            void applyTransform(const Affine2d& t);
            
            void setTransform(const Affine2d& t, bool recurse = true);
            
            void setInverseTransform(const Affine2d& it, bool recurse = true);

            void applyDisplacement(const Point& d);
            
            /// Discard any empty compartments
            void elideEmptyComps();
            
            /// Reset displacement deltas for all nodes & rxns
            void randomizePositions(const Box& bounds);
            
            /// Rebuild curves
            void rebuildCurves();

            /** @brief Recalc the CPs for all curves
             */
            void recalcCurveCPs();
            
            /// Reposition the junctions at the mean centroid of connected nodes
            void recenterJunctions();
            
            /// Reset displacement deltas for all nodes & rxns
            void resetActivity();
            
            /** @brief Apply restoring force to keep nodes in a box
             * @param[in] b The box
             * @param[in] f The force
             * @param[in] t The edge tolerance (distance at which force starts kicking in)
             */
            //void doNodeBoxContactForce(const Box& b, const Real f, const Real t);
            
            /** @brief Limits the maximum displacement
             * @param[in] cap Magnitude of maximum displacement
             */
            void capDeltas(const Real cap);
            
            /** @brief Apply the deltas to positions
             * @param[in] scale Scaling factor
             */
            void updatePositions(const Real scale);
            
            /// Update extents on all elements
            void updateExtents();
            
            /// Resize compartments to enclose contents
            void resizeCompsEnclose(double padding = 0);
            
            /// Autosize compartments when layout info is not available
            void autosizeComps();
            
            /** @brief Compute the mean node position
             */
            Point pmean() const;
            
            /** @brief Compute center of bounding box
             */
            Point center() const;
            
            /** @brief Get bounding box
             */
            Box getExtents() const;
            
            /** @brief Centers the view around p
             * @param[in] p The point to center about
             */
            void recenter(const Point& p);
            
            /** @brief Compute the diameter
             */
            //Real diam(const Point& p);
            
            /** @brief Compute the variance in positions of nodes
             */
            Point pvariance() const;
            
            // IO/Diagnostics:
            
            /// Dump to a stream
            void dump(std::ostream& os, uint32 ind);
            
            /// Dump element force info
            void dumpEltForces(std::ostream& os, uint32 ind) const;

			Node* getNodeAtIndex(int index) { return _nodes[index]; }
            
            //iterators
            
            NodeIt NodesBegin() { return _nodes.begin(); }
            NodeIt NodesEnd() { return _nodes.end(); }
            
            ConstNodeIt NodesBegin() const { return _nodes.begin(); }
            ConstNodeIt NodesEnd() const { return _nodes.end(); }
            
            RxnIt RxnsBegin() { return _rxn.begin(); }
            RxnIt RxnsEnd() { return _rxn.end(); }
            
            ConstRxnIt RxnsBegin() const { return _rxn.begin(); }
            ConstRxnIt RxnsEnd() const { return _rxn.end(); }
            
            CompIt CompsBegin() { return _comp.begin(); }
            CompIt CompsEnd() { return _comp.end(); }
            
            ConstCompIt CompsBegin() const { return _comp.begin(); }
            ConstCompIt CompsEnd() const { return _comp.end(); }
            
            bool doByteCheck() const { if(bytepattern == 0x3355) return true; else return false; }
        protected:
            
            void removeReactionsForNode(Node* n);
            
            /// Nodes (strong reference)
            NodeVec _nodes;
            /// Reactions
            RxnVec _rxn;
            /// Compartments
            CompVec _comp;
            
            long bytepattern;
            bool layoutspecified_;

            /// Number of subgraphs
            int nsub_;
    };
    
    /// Does runtime type checking
    inline Network* CastToNetwork(void* p) {
        NetworkElement* e = (NetworkElement*)p;
        AN(e->hasNetworkElementBase(), "Runtime type check failed");
        return dynamic_cast<Network*>(e);
    }
    
    // Methods:
    
    /** @brief Construct network topology from the given layout
     * @param[in] lay
     * @returns A @ref Network object representing the topology
     * @details The network will be initialized with the spatial
     * configuration as specified by the layout model
    */
    Network* networkFromLayout(const Layout& lay, const Model& mod);
    
    /** @brief Construct network topology from the given model
     * @param[in] lay
     * @returns A @ref Network object representing the topology
     * @details Useful when layout information isn't present
    */
    Network* networkFromModel(const Model& mod);
    
    /** @brief Gets the number of non-locked nodes
     */
    int layout_getNumFloatingSpecies(const Layout& lay, const Model& mod);
    
}

#endif

#endif
