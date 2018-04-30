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

/** @file layout.h
 * @brief SBML layout interface in C
  */

//== BEGINNING OF CODE ===============================================================

#ifndef __SBNW_LAYOUT_H_
#define __SBNW_LAYOUT_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCore.h"
#include "graphfab/sbml/gf_autolayoutSBML.h"
#include "graphfab/diag/gf_error.h"

//-- C methods --

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void* n;
} gf_network;

typedef struct {
    void* n;
} gf_node;

typedef struct {
    void* r;
} gf_reaction;

typedef struct {
    void* c;
} gf_curve;

typedef struct {
    void* c;
} gf_compartment;

typedef struct {
    void* canv;
} gf_canvas;
    
typedef struct {
    /// Network*
    void* net;
    /// Canvas*
    void* canv;
    /// SBML content
    char* cont;

    // sbml opts
    int level;
    int version;
} gf_layoutInfo;

//DEPRECATED: rename to gf_point
typedef struct {
    Real x;
    Real y;
} CPoint;

typedef struct {
    Real x;
    Real y;
} gf_point;

typedef struct {
    void* tf;
} gf_transform;

typedef struct {
    /// Start
    gf_point s;
    /// Control 1
    gf_point c1;
    /// Control 2
    gf_point c2;
    /// End
    gf_point e;
} gf_curveCP;

typedef enum {
    GF_ROLE_SUBSTRATE,
    GF_ROLE_PRODUCT,
    GF_ROLE_SIDESUBSTRATE,
    GF_ROLE_SIDEPRODUCT,
    GF_ROLE_MODIFIER,
    GF_ROLE_ACTIVATOR,
    GF_ROLE_INHIBITOR
} gf_specRole;

/** @brief Call to clean up an instance of @ref gf_layoutInfo when it is no longer needed
 *  @param[in] l The layout info; Entire layout is freed
 *  \ingroup C_API
 */
_GraphfabExport void gf_freeLayoutInfo(gf_layoutInfo* l);

/** @brief Call to clean up an instance of @ref gf_layoutInfo and all contained data structures
 *  @param[in] l The layout info; Entire layout is freed
 *  \ingroup C_API
 */
_GraphfabExport void gf_freeLayoutInfoHierarch(gf_layoutInfo* l);

/** @brief Call to clean up an instance of @ref gf_layoutInfo and @ref gf_SBMLModel when they are no longer needed
 *  @param[in] mod The SBML model
 *  @param[in] l The layout info; Entire layout is freed
 *  \ingroup C_API
 */
_GraphfabExport void gf_freeModelAndLayout(gf_SBMLModel* mod, gf_layoutInfo* l);

// -- Input --

/** @brief Process the layout info from a document. The layout info holds the network (nodes, reactions, etc.), canvas, and SBML content.
 *  @param[in] lo The SBML model; the layout info is generated from the SBML model
 *  @param[out] l The layout info; pointer to the layout info generated from SBML model is returned
 *  \ingroup C_API
 */
_GraphfabExport gf_layoutInfo* gf_processLayout(gf_SBMLModel* lo);

/**
 *  @deprecated
 *  @brief [DEPRECATED] Load SBML document from memory buffer and process
 *  @param[in] buf The buffer containing the SBML file; used to create the SBML model
 *  @param[in] r A pointer to an empty SBML model; the model will be created from the buffer and used to build the layout
 *  @param[out] l The layout info; pointer to the layout info generated from SBML is returned 
 *  \ingroup C_API
 */
_GraphfabExport gf_layoutInfo* gf_loadSBMLIntoLayoutEngine(const char* buf, gf_SBMLModel* r);

// -- Configuration --

/**
 * @brief Set the level and version of the SBML
 * @param[out] l The SBML model with layout info
 * @param[in] level The SBML level
 * @param[in] version The SBML version
 *  \ingroup C_API
 */
_GraphfabExport void gf_setModelNamespace(gf_layoutInfo* l, unsigned long level, unsigned long version);

// -- Object API --

// Network

/** @brief Get the network associated with the model
 *  @param[in] l The layout info; contains the network
 *  \ingroup C_API
 */
_GraphfabExport gf_network gf_getNetwork(gf_layoutInfo* l);

/** @brief Get the network associated with the model
 *  @param[in] l The layout info; contains the network
 *  @return A pointer to the network
 *  \ingroup C_API
 */
_GraphfabExport gf_network* gf_getNetworkp(gf_layoutInfo* l);

/** @brief Clear the network - does not deallocate
 *  @param[in] n Network
 *  \ingroup C_API
 */
_GraphfabExport void gf_clearNetwork(gf_network* n);

/** @brief Release the network
 *  @param[in] n Network
 *  \ingroup C_API
 */
_GraphfabExport void gf_releaseNetwork(gf_network* n);

/** @brief Get the id, user frees memory
 *  @param[in] n Network
 *  \ingroup C_API
 */
_GraphfabExport char* gf_nw_getID(gf_network* n);

/** @brief Get the number of nodes
 *  @param[in] n Network
 *  \ingroup C_API
 */
_GraphfabExport size_t gf_nw_getNumNodes(const gf_network* n);

/** @brief Get the number of reactions
 *  @param[in] n Network
 *  \ingroup C_API
 */
_GraphfabExport size_t gf_nw_getNumRxns(const gf_network* n);

/** @brief Get the number of compartments
 *  @param[in] n Network
 *  \ingroup C_API
 */
_GraphfabExport size_t gf_nw_getNumComps(const gf_network* n);

/** @brief Get the node at index i
 *  @param[in] n Network
 *  @param[in] i Node index
 *  \ingroup C_API
 */
_GraphfabExport gf_node gf_nw_getNode(gf_network* n, size_t i);

/** @brief Get the node at index i
 *  @param[in] n Network
 *  @param[in] i Node index
 *  @return A pointer to the node
 *  \ingroup C_API
 */
_GraphfabExport gf_node* gf_nw_getNodep(gf_network* n, size_t i);

/** @brief Get the node at index i
 *  @param[in] n Network
 *  @param[in] i Node index
 *  \ingroup C_API
 */
_GraphfabExport gf_reaction gf_nw_getRxn(gf_network* n, size_t i);

/** @brief Get the node at index i
 *  @param[in] n Network
 *  @param[in] i Node index
 *  \ingroup C_API
 */
_GraphfabExport gf_reaction* gf_nw_getRxnp(gf_network* n, size_t i);

/** @brief Remove the given reaction
 *  @param[in] n Network
 *  @param[in] i Node index
 *  \ingroup C_API
 */
_GraphfabExport void gf_nw_removeRxn(gf_network* n, gf_reaction* r);

/** @brief Get the compartment at index i
 *  @param[in] n Network
 *  @param[in] i Node index
 *  \ingroup C_API
 */
_GraphfabExport gf_compartment gf_nw_getCompartment(gf_network* n, size_t i);

/** @brief Get the node at index i
 *  @param[in] n Network
 *  @param[in] i Node index
 *  \ingroup C_API
 */
_GraphfabExport gf_compartment* gf_nw_getCompartmentp(gf_network* n, size_t i);

/** @brief Rebuild the curves
 *  @param[in] n Network
 *  \ingroup C_API
 */
_GraphfabExport void gf_nw_rebuildCurves(gf_network* n);

/** @brief Recenter reaction junctions
 *  @param[in] n Network
 *  \ingroup C_API
 */
_GraphfabExport void gf_nw_recenterJunctions(gf_network* n);

/** @brief Add a new node to the network
 *  @param[in] nw Network
 *  @param[in] id The node's requested ID (or null to determine it automatically)
 *  @param[in] name The node's name
 *  @param[in] compartment The compartment to place the node in (may be NULL for no compartment)
 *  \ingroup C_API
 */
_GraphfabExport gf_node gf_nw_newNode(gf_network* nw, const char* id, const char* name, gf_compartment* compartment);

/** @brief Add a new node to the network
 *  @param[in] nw Network
 *  @param[in] id The node's requested ID (or null to determine it automatically)
 *  @param[in] name The node's name
 *  @param[in] compartment The compartment to place the node in (may be NULL for no compartment)
 *  \ingroup C_API
 */
_GraphfabExport gf_node* gf_nw_newNodep(gf_network* nw, const char* id, const char* name, gf_compartment* compartment);

/** @brief Remove a node from the network
 *  @param[in] n Network
 *  \ingroup C_API
 */
_GraphfabExport int gf_nw_removeNode(gf_network* nw, gf_node* node);

/** @brief Did the SBML model include layout?
 *  @return 1 for yes, 0 for no
 *  @param[in] n Network
 *  \ingroup C_API
 */
_GraphfabExport int gf_nw_isLayoutSpecified(gf_network* nw);

// Node

/** @brief Add a node to a compartment
 *  @param[in] c Compartment
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport void gf_node_setCompartment(gf_node* n, gf_compartment* c);

/** @brief Clear the node - does not deallocate
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport void gf_clearNode(gf_node* n);

/** @brief Release the node
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport void gf_releaseNode(const gf_node* n);

/** @brief Is the node locked?
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport int gf_node_isLocked(gf_node* n);

/** @brief Lock the node
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport void gf_node_lock(gf_node* n);

/** @brief Unlock the node
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport void gf_node_unlock(gf_node* n);

/** @brief Alias the node
 *  @param[in] n The node to alias
 *  \ingroup C_API
 */
_GraphfabExport int gf_node_alias(gf_node* n, gf_network* m);

/** @brief Is the node aliased?
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport int gf_node_isAliased(gf_node* n);

/** @brief Get the centroid of the node
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport gf_point gf_node_getCentroid(gf_node* n);

/** @brief Get the centroid of the node
 *  @param[in] n Node
 *  @param[out] x X coord of centroid
 *  @param[out] y Y coord of centroid
 *  \ingroup C_API
 */
_GraphfabExport void gf_node_getCentroidXY(gf_node* n, double* x, double* y);

/** @brief Set the centroid of the node
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport void gf_node_setCentroid(gf_node* n, gf_point p);

/** @brief Get the width of the node
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport double gf_node_getWidth(gf_node* n);

/** @brief Set the width of the node
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport void gf_node_setWidth(gf_node* n, double width);

/** @brief Get the height of the node
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport double gf_node_getHeight(gf_node* n);

/** @brief Set the height of the node
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport void gf_node_setHeight(gf_node* n, double height);

/** @brief Get the id, user frees memory
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport char* gf_node_getID(gf_node* n);

/** @brief Get the name
 *  @param[in] n Node
 *  \ingroup C_API
 */
_GraphfabExport const char* gf_node_getName(gf_node* n);

/** @brief Get a list of all reactions connected to the node
 *  @param[in] n Node
 *  @param[out] num The number of reactions
 *  @param[out] rxns The reaction array (callee must free with @ref gf_free)
 *  @return Reserved
 *  \ingroup C_API
 */
_GraphfabExport int gf_node_getConnectedReactions(gf_node* n, gf_network* m, unsigned int* num, gf_reaction** rxns);

/** @brief Get a list of all curves connected to the node
 *  @param[in] n Node
 *  @param[out] num The number of curves
 *  @param[out] rxns The curve array (callee must free with @ref gf_free)
 *  @return Reserved
 *  \ingroup C_API
 */
_GraphfabExport int gf_node_getAttachedCurves(gf_node* n, gf_network* m, unsigned int* num, gf_curve** curves);

// Reaction

/** @brief Release the reaction
 *  @param[in] r Reaction
 *  \ingroup C_API
 */
_GraphfabExport void gf_releaseRxn(const gf_reaction* r);

/** @brief Get the id, user frees memory
 *  @param[in] r Reaction
 *  \ingroup C_API
 */
_GraphfabExport char* gf_reaction_getID(gf_reaction* r);

/** @brief Get the centroid of the reaction
 *  @param[in] r Reaction
 *  \ingroup C_API
 */
_GraphfabExport gf_point gf_reaction_getCentroid(gf_reaction* r);

/** @brief Set the centroid of the reaction
 *  @param[in] r Reaction
 *  \ingroup C_API
 */
_GraphfabExport void gf_reaction_setCentroid(gf_reaction* r, gf_point p);

/** @brief Get the number of species in the reaction
 *  @param[in] r Reaction
 *  \ingroup C_API
 */
_GraphfabExport size_t gf_reaction_getNumSpec(const gf_reaction* r);

/** @brief Return true if the reaction has the given species
 *  @param[in] r Reaction
 *  \ingroup C_API
 */
_GraphfabExport int gf_reaction_hasSpec(const gf_reaction* r, const gf_node* n);

/** @brief Get the role for spec i
 *  @param[in] r Reaction
 *  \ingroup C_API
 */
_GraphfabExport gf_specRole gf_reaction_getSpecRole(const gf_reaction* r, size_t i);

/**
 * @brief Convert role to string
 * @param[in] role Role
 * @return Static string indicating the role
 *  \ingroup C_API
 */
_GraphfabExport const char* gf_roleToStr(gf_specRole role);

/**
 * @brief Convert string to role
 * @param[in] str String returned from @ref gf_roleToStr
 * @return Role
 *  \ingroup C_API
 */
_GraphfabExport gf_specRole gf_strToRole(const char* str);

/** @brief Get the index of the species in the network
 *  @param[in] r Reaction
 *  \ingroup C_API
 */
_GraphfabExport size_t gf_reaction_specGeti(const gf_reaction* r, size_t i);

/** @brief Get the number of curves in the reaction
 *  @param[in] r Reaction
 *  \ingroup C_API
 */
_GraphfabExport size_t gf_reaction_getNumCurves(const gf_reaction* r);

/** @brief Get the curve i
 *  @param[in] r Reaction
 *  \ingroup C_API
 */
_GraphfabExport gf_curve gf_reaction_getCurve(const gf_reaction* r, size_t i);

/** @brief Get the curve i
 *  @param[in] r Reaction
 *  \ingroup C_API
 */
_GraphfabExport gf_curve* gf_reaction_getCurvep(const gf_reaction* r, size_t i);

/** @brief Recenter reaction centroid
 *  @param[in] n Reaction
 *  \ingroup C_API
 */
_GraphfabExport void gf_reaction_recenter(gf_reaction* r);

/** @brief Recalculate the curve CPs, don't recenter
 *  @param[in] n Reaction
 *  \ingroup C_API
 */
_GraphfabExport void gf_reaction_recalcCurveCPs(gf_reaction* r);

// Curve

/** @brief Release the curve
 *  @param[in] c Curve
 *  \ingroup C_API
 */
_GraphfabExport void gf_releaseCurve(const gf_curve* c);

/** @brief Get the id, user frees memory
 *  @param[in] c Curve
 *  \ingroup C_API
 */
// _GraphfabExport char* gf_curve_getID(gf_curve* c);

/**
 * @brief Get the role of the species with the given curve
 * @param[in] c Curve
 * @return The index of the matching species in the reaction
 *  \ingroup C_API
 */
_GraphfabExport gf_specRole gf_curve_getRole(gf_curve* c);

/** @brief Get the CPs for the curve
 *  @param[in] c Curve
 *  \ingroup C_API
 */
_GraphfabExport gf_curveCP gf_getCurveCPs(const gf_curve* c);

/** @brief Returns true if the given curve should be drawn with an arrowhead
 *  @param[in] c Curve
 *  \ingroup C_API
 */
_GraphfabExport int gf_curve_hasArrowhead(const gf_curve* c);


/** @brief Get the vertices for the curve's arrowhead
 *  @param[in] c Curve
 *  @param[out] n Number of arrowhead verts
 *  @param[out] v The vertices (new arrow, callee owns)
 *  \ingroup C_API
 */
_GraphfabExport int gf_curve_getArrowheadVerts(const gf_curve* c, unsigned int* n, gf_point** v);

// Comparment

/** @brief Release the comp
 *  @param[in] c Comp
 *  \ingroup C_API
 */
_GraphfabExport void gf_releaseCompartment(const gf_compartment* c);

/** @brief Get the id, user frees memory
 *  @param[in] c Compartment
 *  \ingroup C_API
 */
_GraphfabExport char* gf_compartment_getID(gf_compartment* c);

/** @brief Get the "upper left" corner
 *  @param[in] c Compartment
 *  \ingroup C_API
 */
_GraphfabExport gf_point gf_compartment_getMinCorner(gf_compartment* c);

/** @brief Set the "upper left" corner
 *  @param[in] c Compartment
 *  @param[in] p Corner
 *  \ingroup C_API
 */
_GraphfabExport void gf_compartment_setMinCorner(gf_compartment* c, gf_point p);

/** @brief Get the "lower right" corner
 *  @param[in] c Compartment
 *  \ingroup C_API
 */
_GraphfabExport gf_point gf_compartment_getMaxCorner(gf_compartment* c);

/** @brief Set the "lower right" corner
 *  @param[in] c Compartment
 *  @param[in] p Corner
 *  \ingroup C_API
 */
_GraphfabExport void gf_compartment_setMaxCorner(gf_compartment* c, gf_point p);

/** @brief Get the width of the compartment
 *  @param[in] c Compartment
 *  \ingroup C_API
 */
_GraphfabExport double gf_compartment_getWidth(gf_compartment* c);

/** @brief Get the height of the compartment
 *  @param[in] c Compartment
 *  \ingroup C_API
 */
_GraphfabExport double gf_compartment_getHeight(gf_compartment* c);

/** @brief Get the number of species in the compartment
 *  @param[in] c Compartment
 *  \ingroup C_API
 */
_GraphfabExport size_t gf_compartment_getNumElt(gf_compartment* c);

// Transform

/** @brief Fit to the specified window
 *  @param[in] l Layout
 *  @param[in] left Left edge of screen (other args similar)
 *  \ingroup C_API
 */
_GraphfabExport void gf_fit_to_window(gf_layoutInfo* l, double left, double top, double right, double bottom);

/** @brief Fit to the specified window (do not apply transform)
 *  @param[in] l Layout
 *  @param[in] left Left edge of screen (other args similar)
 *  @return The new transform (not applied)
 *  \ingroup C_API
 */
_GraphfabExport gf_transform* gf_tf_fitToWindow(gf_layoutInfo* l, double left, double top, double right, double bottom);

/** @brief Apply transform to point
 *  @param[in] tf Transform
 *  @param[in] p Point
 *  \ingroup C_API
 */
_GraphfabExport CPoint gf_tf_apply_to_point(gf_transform* tf, CPoint p);

/** @brief Get the scale of the transform
 *  @param[in] tf Transform
 *  \ingroup C_API
 */
_GraphfabExport gf_point gf_tf_getScale(gf_transform* tf);

/** @brief Get the displacement of the transform
 *  @param[in] tf Transform
 *  \ingroup C_API
 */
_GraphfabExport gf_point gf_tf_getDisplacement(gf_transform* tf);

/** @brief Get the displacement of the transform
 *  @param[in] tf Transform
 *  @return The displacement with the transform applied
 *  \ingroup C_API
 */
_GraphfabExport gf_point gf_tf_getPostDisplacement(gf_transform* tf);

/** @brief Dump transform
 *  @param[in] tf Transform
 *  \ingroup C_API
 */
_GraphfabExport void gf_dump_transform(gf_transform* tf);

/** @brief Release transform
 *  @param[in] tf Transform
 *  \ingroup C_API
 */
_GraphfabExport void gf_release_transform(gf_transform* tf);

// Canvas

/** @brief Get the canvas associated with the model
 *  @param[in] l The layout info
 *  \ingroup C_API
 */
_GraphfabExport gf_canvas gf_getCanvas(gf_layoutInfo* l);

/** @brief Get the canvas associated with the model
 *  @param[in] l The layout info
 *  \ingroup C_API
 */
_GraphfabExport gf_canvas* gf_getCanvasp(gf_layoutInfo* l);

/** @brief Get the canvas associated with the model
 *  @param[in] l The layout info
 *  \ingroup C_API
 */
_GraphfabExport void gf_clearCanvas(gf_canvas* c);

/** @brief Release the canvas
 *  @param[in] c The canvas
 *  \ingroup C_API
 */
_GraphfabExport void gf_releaseCanvas(gf_canvas* c);

/** @brief Get the width of the canvas
 *  @param[in] c The canvas
 *  \ingroup C_API
 */
_GraphfabExport unsigned int gf_canvGetWidth(gf_canvas* c);

/** @brief Get the height of the canvas
 *  @param[in] c The canvas
 *  \ingroup C_API
 */
_GraphfabExport unsigned int gf_canvGetHeight(gf_canvas* c);

/** @brief Set the width of the canvas
 *  @param[in] c The canvas
 *  @param[out] width The width
 *  \ingroup C_API
 */
_GraphfabExport void gf_canvSetWidth(gf_canvas* c, unsigned long width);

/** @brief Set the height of the canvas
 *  @param[in] c The canvas
 *  @param[in] height The height
 *  \ingroup C_API
 */
_GraphfabExport void gf_canvSetHeight(gf_canvas* c, unsigned long height);

// -- Queries --

/** @brief Get the centroid of a node
 *  @param[in] l The layout info; contains the network that holds the list of nodes and their centroids
 *  @param[in] id The node id; used to find the correct node in the layout
 *  @param[in] p The point; used to store the coordinates of the node centroid
 *  \ingroup C_API
 */
_GraphfabExport void gf_getNodeCentroid(gf_layoutInfo* l, const char* id, CPoint* p);

// -- Configuration

/** @brief Lock a node by id
 *  @param[in] l The layout info; contains the network that holds the list of nodes to be searched
 *  @param[in] id The node id; used to find the correct node in the layout
 *  @param[out] 0 if success
 *  @param[out] 1 if failure
 *  @deprecated
 *  \ingroup DEPRECATED
 */
_GraphfabExport int gf_lockNodeId(gf_layoutInfo* l, const char* id);

/** @brief Unlock a node by id
 *  @param[in] l The layout info; contains the network with the nodes to be searched
 *  @param[in] id The node id; used to find the correct node in the layout
 *  @param[out] 0 if success
 *  @param[out] 1 if failure
 *  @deprecated
 *  \ingroup DEPRECATED
 */
_GraphfabExport int gf_unlockNodeId(gf_layoutInfo* l, const char* id);

/** @brief Create node aliases
 *  @param[in] l The layout info; contains the network which holds the list of nodes to be searched
 *  @param[in] id The node id; used to find the correct node in the layout
 *  @param[out] 0 if success
 *  @param[out] 1 if failure
 *  @deprecated
 *  \ingroup DEPRECATED
 */
_GraphfabExport int gf_aliasNodeId(gf_layoutInfo* l, const char* id);

/** @brief Create node aliases by node degree
 *  @param[in] l The layout info; contains the network that holds the list of nodes and reactions necessary for aliasing
 *  @param[in] minDegree The minimum node degree; Nodes that are of the minimum degree or higher will be aliased
 *  \ingroup C_API
 */
_GraphfabExport void gf_aliasNodebyDegree(gf_layoutInfo* l, const int minDegree);

// -- Processing --

/** @brief Randomize node positions
 *  @param[in] m The layout info; contains the network of nodes and reactions, the coordinates of which will be randomized
 *  \ingroup C_API
 */
_GraphfabExport void gf_randomizeLayout(gf_layoutInfo* m);

/** @brief Randomize node positions for a given network & canvas
 *  @param[in] n Network
 *  @param[in] c Canvas
 *  \ingroup C_API
 */
_GraphfabExport void gf_randomizeLayout2(gf_network* n, gf_canvas* c);

/** @brief Randomize node positions for a given network & extents
 *  @param[in] n Network
 *  @param[in] left The minimum X coord of the bounding box
 *  @param[in] top The minimum Y coord of the bounding box
 *  @param[in] right The maximum X coord of the bounding box
 *  @param[in] bottom The maximum Y coord of the bounding box
 *  \ingroup C_API
 */
_GraphfabExport void gf_randomizeLayout_fromExtents(gf_network* n, double left, double top, double right, double bottom);

// -- Output --

/** @brief Write an SBML file, including layout
 * @returns 0 for success
 * @param[in] filename The output file
 * @param[in] m The SBML model; required since this tool only handles layout
 * @param[in] l The layout info; replaces any currently existing SBML layout in the model in the generated file
 *  \ingroup C_API
 */
_GraphfabExport int gf_writeSBMLwithLayout(const char* filename, gf_SBMLModel* m, gf_layoutInfo* l);

/** @brief Write an SBML file (does not include layout
 * @returns 0 for success
 * @param[in] filename The output file
 * @param[in] m The SBML model; required since this tool only handles layout
 *  \ingroup C_API
 */
_GraphfabExport int gf_writeSBML(const char* filename, gf_SBMLModel* m);

/** @brief String version of writeSBMLwithLayout
 *  @param[in] m The SBML model; required since this tool only handles layout
 *  @param[in] l The layout info; the SBML in string form is written to the layout
 *  @return Raw SBML as UTF-8 string (owned by callee)
 *  \ingroup C_API
 */
_GraphfabExport const char* gf_getSBMLwithLayoutStr(gf_SBMLModel* m, gf_layoutInfo* l);

/** @brief Returns the current version of the library
 *  @return Static string
 *  \ingroup C_API
 */
_GraphfabExport const char* gf_getCurrentLibraryVersion(void);

/** @brief Frees the memory block at x
 *  @note For strings (char*), use @ref gf_strfree
 *  \ingroup C_API
 */
_GraphfabExport void gf_free(void* x);

/** @brief Compute a point on the parametric curve
 *  @param[in] c Cubic Bezier control points
 *  @param[in] t Cubic Bezier parameter
 *  \ingroup C_API
 */
_GraphfabExport gf_point gf_computeCubicBezierPoint(gf_curveCP* c, Real t);

/** @brief Compute the intersection between a cubic Bezier and a line
 *  @param[in] c Cubic Bezier control points
 *  @param[in] line_start The start of the line
 *  @param[in] line_end The end of the line
 *  @return An array of intersection points with a 0, 0 sentinel
 *  \ingroup C_API
 */
_GraphfabExport gf_point* gf_computeCubicBezierLineIntersec(gf_curveCP* c, gf_point* line_start, gf_point* line_end);

/** @brief Get the number of vertices in the arrowhead polygon
 *  @param[in] style Arrowhead style number
 *  @return The arrowhead polygon vertex count
 *  \ingroup C_API
 */
_GraphfabExport int gf_arrowheadStyleGetNumVerts(int style);

/** @brief Get the nth vertex of the arrow polygon
 *  @param[in] style Arrowhead style number
 *  @param[in] n The vertex index
 *  @return The nth vertex of the polygon
 *  \ingroup C_API
 */
_GraphfabExport gf_point gf_arrowheadStyleGetVert(int style, int n);

/** @brief Is the given style filled?
 *  @param[in] style Arrowhead style number
 *  @return Nonzero if the arrow polygon should be filled
 *  \ingroup C_API
 */
_GraphfabExport int gf_arrowheadStyleIsFilled(int style);

/** @brief Get the number of arrowhead styles
 *  @return The number of arrowhead styles
 *  \ingroup C_API
 */
_GraphfabExport unsigned long gf_arrowheadNumStyles();

/** @brief Set the arrowhead style for a specific role type
 *  @param[in] role Role type
 *  @param[in] style Arrowhead style number
 *  \ingroup C_API
 */
_GraphfabExport void gf_arrowheadSetStyle(gf_specRole role, int style);

/** @brief Get the style for the role type
 *  @param[in] role Role type
 *  \ingroup C_API
 */
_GraphfabExport int gf_arrowheadGetStyle(gf_specRole role);

#ifdef __cplusplus
}//extern "C"
#endif

//-- C++ methods --
#ifdef __cplusplus



#endif

#endif
