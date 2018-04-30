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
#include "graphfab/sbml/gf_layout.h"
#include "graphfab/diag/gf_error.h"
#include "graphfab/network/gf_network.h"
#include "graphfab/layout/gf_canvas.h"
#include "graphfab/layout/gf_box.h"
#include "graphfab/layout/gf_point.h"
#include "graphfab/math/gf_round.h"
#include "graphfab/math/gf_geom.h"
#include "graphfab/math/gf_transform.h"
#include "graphfab/util/gf_string.h"

#include "sbml/SBMLTypes.h"
#include "sbml/packages/layout/common/LayoutExtensionTypes.h"

#include <exception>
#include <typeinfo>

#include <stdlib.h> // free SBML strings

using namespace Graphfab;

void gf_freeLayoutInfo(gf_layoutInfo* l) {
    AN(l, "gf_freeLayoutInfo: unexpected null ptr");
    
    if(l->cont)
        free(l->cont);
    free(l);
}

void gf_freeLayoutInfoHierarch(gf_layoutInfo* l) {
    AN(l, "gf_freeLayoutInfo: unexpected null ptr");
    
    Network* net = (Network*)l->net;
    net->hierarchRelease();
    delete net;
    Canvas *canv = (Canvas*)l->canv;
    if(canv)
        delete canv;
    if(l->cont)
        free(l->cont);
    free(l);
}

void gf_freeModelAndLayout(gf_SBMLModel* mod, gf_layoutInfo* l) {
    if(!mod)
        AN(0, "Not a valid model pointer"); //null
    SBMLDocument* doc = (SBMLDocument*)mod->pdoc;
    delete doc;
    free(mod);

    if(!l)
        AN(0, "Not a valid layout pointer"); //null
    Network* net = (Network*)l->net;
    delete net;
    Canvas *canv = (Canvas*)l->canv;
    if(canv)
        delete canv;
    if(l->cont)
        free(l->cont);
    free(l);
}

void gf_initLayoutInfo(gf_layoutInfo* l) {
    l->cont = NULL;
}

gf_layoutInfo* gf_processLayout(gf_SBMLModel* lo) {
    gf_layoutInfo* l;
    SBMLDocument* doc = (SBMLDocument*)lo->pdoc;

    // enable the layout package if it is not already
    if (!doc->isPkgEnabled("layout")) {
        if (doc->getLevel() == 2) {
            doc->enablePackage(LayoutExtension::getXmlnsL2(), "layout",  true);
        } else if (doc->getLevel() == 3) {
            doc->enablePackage(LayoutExtension::getXmlnsL3V1V1(), "layout",  true);
        }
    }

    // ensure layout package is enabled
    AT(doc->isPkgEnabled("layout"), "Layout package not enabled");
    
    // get the model
    Model* mod = doc->getModel();
    AN(mod, "Failed to load model");
    
    // layout plugin ptr
    SBasePlugin* layoutBase = mod->getPlugin("layout");
    AN(layoutBase, "No plugin named \"layout\"");
    
    // cast to derived
    LayoutModelPlugin* layoutPlugin=NULL;
    try {
        layoutPlugin = dynamic_cast<LayoutModelPlugin*>(layoutBase);
    } catch(std::bad_cast e) {
        gf_emitError("Unable to get layout information");
        AN(0);
    }
    
    //determine if there is layout information present
    int have_layout;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     printf("Number of layouts: %d\n", layoutPlugin->getNumLayouts());
    #endif
    if(layoutPlugin->getNumLayouts() == 0)
        have_layout = 0;
    else
        have_layout = 1;
    if(layoutPlugin->getNumLayouts() > 1)
        gf_emitWarn("Warning: multiple layouts. Using first");
    const Layout* layout = layoutPlugin->getLayout(0);
    
    //construct the network
    Network* net=NULL;
    if(have_layout)
        net = networkFromLayout(*layout, *mod);
    else
        net = networkFromModel(*mod);
    AN(net, "Failed to construct network");
    
    //get the canvas
    Canvas* canv;
    if(have_layout) {
        canv = new Canvas();
        //get dimensions from SBML layout
        const Dimensions* dims = layout->getDimensions();
        canv->setWidth(dims->getWidth());
        canv->setHeight(dims->getHeight());
//         std::cerr << "Canvas width = " << canv->getWidth() << ", height = " << canv->getHeight() << "\n";
    } else {
        canv = new Canvas();
        //get dimensions from SBML layout
        canv->setWidth(1024);
        canv->setHeight(1024);
    }
    #if SAGITTARIUS_DEBUG_LEVEL >= 3
        //print
        net->dump(std::cout,0);
    #endif
    
    l = (gf_layoutInfo*)malloc(sizeof(gf_layoutInfo));
    gf_initLayoutInfo(l);
    l->level = doc->getLevel();
    l->version = doc->getVersion();
    
    l->net = net;
    
    l->canv = canv;
    
    return l;
}

void gf_getNodeCentroid(gf_layoutInfo* l, const char* id, CPoint* p) {
    Network* net = (Network*)l->net;
    AN(net, "No network");
    
    Graphfab::Point pp(0,0);
    Node* n = net->findNodeById(id);
    if(!n)
        return;
    pp = n->getCentroid();
    
    p->x = pp.x;
    p->y = pp.y;
}

int gf_lockNode(gf_layoutInfo* l, const char* id) {
    Network* net = (Network*)l->net;
    AN(net, "No network");
    
    Node* n = net->findNodeById(id);
    if(!n)
        return 1;
    n->lock();
    return 0;
}

int gf_unlockNode(gf_layoutInfo* l, const char* id) {
    Network* net = (Network*)l->net;
    AN(net, "No network");
    
    Node* n = net->findNodeById(id);
    if(!n)
        return 1;
    n->unlock();
    return 0;
}

int gf_aliasNode(gf_layoutInfo* l, const char* id) {
    Network* net = (Network*)l->net;
    AN(net, "No network");
    
    Node* n = net->findNodeById(id);
    if(!n)
        return 1;
    n->setAlias(true);
    for(Network::RxnIt i=net->RxnsBegin(); i!=net->RxnsEnd(); ++i) {
        Graphfab::Reaction* r = *i;
        if(r->hasSpecies(n)) {
            Node* w = new Node(*n);
            w->setGlyph(w->getGlyph() + "_" + r->getId());
            w->setCentroid(new2ndPos(r->getCentroid(), w->getCentroid(), 0., -25., false));
            net->addNode(w);
            r->substituteSpecies(n, w);
        }
    }
    return 0;
}

void gf_aliasNodebyDegree(gf_layoutInfo* l, int minDegree) {
    Network* net = (Network*)l->net;
    AN(net, "No network");
    
    int a, b, nodecount1, nodecount2, size = net->getTotalNumNodes(), i = 0, aliasCount = 0;
    char aliasCountString[33];
    sprintf(aliasCountString, "%d", aliasCount);
    std::vector<Node *> foundNodes;
    std::vector<Graphfab::Reaction *> Rxns;
    
    //Iterator does not work because nodes are added to the list, had to use while loop instead
    //for(Network::NodeIt i = net->NodesBegin(); i < net->NodesEnd(); ++i) {
    while(i < size) {
        Graphfab::Node* n = net->getNodeAtIndex(i);
        
        //If the node is the required minimum degree or greater and is not an alias
        if(n->degree() >= minDegree && !n->isCentroidSet() && !n->isAlias()) {
            
            for(Network::RxnIt c=net->RxnsBegin(); c!=net->RxnsEnd(); ++c) {
                Graphfab::Reaction* r = *c;

                if(r->hasSpecies(n)) {
                    if(n->degree() > 1) {
                        
                        //Create a temp copy of all reactions
                        for(Network::RxnIt d=net->RxnsBegin(); d!=net->RxnsEnd(); ++d) {
                            Graphfab::Reaction* react = *d;
                            Rxns.push_back(react);
                        }
                        
                        foundNodes.push_back(n);

                        //Find all nodes that are in the subgraph with node n
                        a = 0;
                        while(a < foundNodes.size()) {
                            b = 0;
                            while(b < Rxns.size()) {
                                if(Rxns[b]->hasSpecies(foundNodes[a])) {
                                    for(Graphfab::Reaction::NodeIt j=Rxns[b]->NodesBegin(); j!=Rxns[b]->NodesEnd(); ++j) {
                                        Graphfab::Node* node = j->first;
                                        for(int m = 0; m < foundNodes.size(); ++m) {
                                            if(node == foundNodes[m]) break;
                                            else if(m == foundNodes.size() - 1) foundNodes.push_back(node);
                                        }
                                    }
                                    Rxns.erase(Rxns.begin() + b);
                                }
                                else {
                                    b++;
                                }
                            }
                            a++;
                        }

                        nodecount1 = foundNodes.size();

                        //Create the alias node
                        Node* w = new Node(*n);
                        w->setGlyph(w->getGlyph() + "_" + r->getId() + "_alias_" + aliasCountString);
                        w->set_degree(1);
                        w->setCentroid(new2ndPos(r->getCentroid(), w->getCentroid(), 0., -25., false));
                        w->setAlias(true);
                        //Substitute the alias into the current reaction, but don't add to the network
                        r->substituteSpecies(n, w);
                        n->set_degree(n->degree() - 1);
                        
                        Rxns.clear();
                        foundNodes.clear();

                        //Create a temp copy of all reactions
                        for(Network::RxnIt d=net->RxnsBegin(); d!=net->RxnsEnd(); ++d) {
                            Graphfab::Reaction* react = *d;
                            Rxns.push_back(react);
                        }
                        
                        foundNodes.push_back(w);
                        
                        //Find all nodes that are in the subgraph with the alias node
                        a = 0;
                        while(a < foundNodes.size()) {
                            b = 0;
                            while(b < Rxns.size()) {
                                if(Rxns[b]->hasSpecies(foundNodes[a])) {
                                    for(Graphfab::Reaction::NodeIt j=Rxns[b]->NodesBegin(); j!=Rxns[b]->NodesEnd(); ++j) {
                                        Graphfab::Node* node = j->first;
                                        for(int m = 0; m < foundNodes.size(); ++m) {
                                            if(node == foundNodes[m]) break;
                                            else if(m == foundNodes.size() - 1) foundNodes.push_back(node);
                                        }
                                    }
                                    Rxns.erase(Rxns.begin() + b);
                                }
                                else {
                                    b++;
                                }
                            }
                            a++;
                        }
                        
                        nodecount2 = foundNodes.size();

                        
                        if(nodecount1 > nodecount2) {

                            //If we lost a node(s), reset the connection to the original
                            
                            r->substituteSpecies(w, n);
                            n->set_degree(n->degree() + 1);
                            delete(w);

                        } else {
                            //The node counts are equal. The alias can be kept
                            net->addNode(w); 
                            aliasCount++;
                            sprintf(aliasCountString, "%d", aliasCount);
                        }
                    }
                    
                }
            }
        }
        i++;
    }
    //printf("Aliases created: %d\n", aliasCount);  
}



SBMLDocument* populateSBMLdoc(gf_SBMLModel* m, gf_layoutInfo* l) {
//     SBMLDocument* doc = (SBMLDocument*)m->pdoc;
    SBMLNamespaces sbmlns(l->level ? l->level : 3, l->version ? l->version : 1, "layout", 1);
    SBMLDocument* doc = new SBMLDocument(&sbmlns);
    AN(doc, "No SBML document");
    AT(doc->isPkgEnabled("layout"), "Layout package not enabled");
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     std::cout << "doc->isPkgEnabled(\"layout\") = " << doc->isPkgEnabled("layout") << std::endl;
    #endif
    
    // get the model
//     Model* mod = doc->getModel();
//     AN(mod, "Failed to load model");
    
    bool create_default_compartment = false;

    Model* model = doc->createModel();
    doc->setPkgRequired("layout", false); // libSBML will refuse to open the file if required=true
    doc->setModel(model);
    
    // layout plugin
    LayoutPkgNamespaces layoutns(l->level ? l->level : 3, l->version ? l->version : 1, 1);
//     if (doc->getLevel() == 2)
//       doc->enablePackage(LayoutExtension::getXmlnsL2(),"layout", true);
//     else if (doc->getLevel() == 3)
//       doc->enablePackage(LayoutExtension::getXmlnsL3V1V1(),"layout", true);
    SBasePlugin* layoutBase = model->getPlugin("layout");
    AN(layoutBase, "No plugin named \"layout\"");
    
    // cast to derived
    LayoutModelPlugin* layoutPlugin=NULL;
    try {
        layoutPlugin = dynamic_cast<LayoutModelPlugin*>(layoutBase);
    } catch(std::bad_cast) {
        gf_emitError("Unable to get layout information");
        AN(0);
    }
    
    // clear all previous annotations
    while(layoutPlugin->getNumLayouts())
        layoutPlugin->removeLayout(0);
    // clear non-standard annotations
//     if(mod->getAnnotation())
//         mod->getAnnotation()->removeChildren();
    
    // add one layout element
    Layout* lay(layoutPlugin->createLayout());
    
    Canvas* can = NULL;
    if(l)
        can = (Canvas*)l->canv;
    Dimensions dims;
    if(can) {
        dims.setWidth(can->getWidth());
        dims.setHeight(can->getHeight());
    } else {
        dims.setWidth(1024.);
        dims.setHeight(1024.);
    }
    lay->setDimensions(&dims);
    
    // set id
    lay->setId("Graphfab_Layout");
    
    // get the network model
    Network* net = NULL;
    if(l) {
        net = (Network*)l->net;
        AT(net->doByteCheck(), "Network has wrong type");
    }
    
    if(net) {
        // rebuild the curves as we will need them shortly
        net->rebuildCurves();
        
        // add compartments
        for(Network::ConstCompIt i=net->CompsBegin(); i!=net->CompsEnd(); ++i) {
            const Graphfab::Compartment* c = *i;
            
            // create glyph
            CompartmentGlyph* cg = new CompartmentGlyph();
            
            // id
            if(c->getGlyph() != "")
                cg->setId(c->getGlyph());
            else
                cg->setId(c->getId() + "_Glyph");
            
            // set model reference
            cg->setCompartmentId(c->getId());
            
            // do bounding box
            BoundingBox bb;
            bb.setX(sround(c->getMinX()));
            bb.setY(sround(c->getMinY()));
            bb.setWidth(sround(c->getWidth()));
            bb.setHeight(sround(c->getHeight()));
            // apply bb to glyph
            cg->setBoundingBox(&bb);
            
            lay->addCompartmentGlyph(cg);
            
            delete cg;
            
            // add compartment
            ::Compartment* compartment = model->createCompartment();
            compartment->setId(c->getId());
            compartment->setSize(1.);
            compartment->setConstant(false);
        }
        
        // add species
        uint64 calias=0;
        for(Network::NodeIt i=net->NodesBegin(); i!=net->NodesEnd(); ++i) {
            Node* n = *i;
            AN(n, "Empty node");
            
            SpeciesGlyph* sg = new SpeciesGlyph();
            
            // id
            if(n->getGlyph() == "") {
                // empty glyph: populate it
                if(!n->isAlias())
                    n->setGlyph(n->getId() + "_Glyph");
                else {
                    std::stringstream ss;
                    ss << n->getId() << "_Alias" << ++calias << "_Glyph";
                    n->setGlyph(ss.rdbuf()->str()); //TODO: try replacing with ss.str()
                }
            }
            sg->setId(n->getGlyph());
            
            // set model reference
            sg->setSpeciesId(n->getId());
            
            // do bounding box
            BoundingBox bb;
            bb.setX(sround(n->getMinX()));
            bb.setY(sround(n->getMinY()));
            bb.setWidth(sround(n->getWidth()));
            bb.setHeight(sround(n->getHeight()));
            // apply bb to glyph
            sg->setBoundingBox(&bb);
            
            lay->addSpeciesGlyph(sg);
            
            delete sg;
            
            // add species
            ::Species* species = model->createSpecies();
            species->setId(n->getId());
            Graphfab::Compartment* com = net->findContainingCompartment(n);
            if(com)
                species->setCompartment(com->getId());
            else {
                species->setCompartment("graphfab_default_compartment");
                create_default_compartment = true;
            }
            species->setInitialConcentration(0.);
            species->setBoundaryCondition(0.);
            species->setHasOnlySubstanceUnits(false);
            species->setConstant(false);
        }

        if (create_default_compartment) {
            for(Network::ConstCompIt i=net->CompsBegin(); i!=net->CompsEnd(); ++i) {
                const Graphfab::Compartment* c = *i;
                if (c->getId() == "graphfab_default_compartment")
                    // already exists
                    goto skip_default_comp;
            }
            ::Compartment* compartment = model->createCompartment();
            compartment->setId("graphfab_default_compartment");
            compartment->setSize(1.);
            compartment->setConstant(false);
            int sbo_result = compartment->setSBOTerm(410);
//             int sbo_result = compartment->setSBOTerm("0000410");
            switch (sbo_result) {
              case LIBSBML_INVALID_ATTRIBUTE_VALUE:
                std::cerr << "SBO term invalid\n";
                break;
              case LIBSBML_UNEXPECTED_ATTRIBUTE:
                std::cerr << "SBO term unexpected\n";
                break;
              case LIBSBML_OPERATION_SUCCESS:
//                 std::cerr << "SBO term success";
                break;
              default:
//                 std::cerr << "SBO term default " << sbo_result << "\n";
//                 std::cerr << "LIBSBML_UNEXPECTED_ATTRIBUTE " << LIBSBML_UNEXPECTED_ATTRIBUTE << "\n";
                break;
            }
        }
        skip_default_comp:;
        
            // add species' text glyphs
        for(Network::NodeIt i=net->NodesBegin(); i!=net->NodesEnd(); ++i) {
            Node* n = *i;
            AN(n, "Empty node");
            
            TextGlyph* tg = new TextGlyph();
            
            //// id
            tg->setId("t" + n->getGlyph());

            // link to species glyph
            tg->setGraphicalObjectId(n->getGlyph());

            // set text to be displayed
            if(n->getName() != "")
                tg->setText(n->getName());
            
            // if no name use id
            else
                tg->setText(n->getId());
            
            // do bounding box
            BoundingBox bb;
            bb.setX(sround(n->getMinX()));
            bb.setY(sround(n->getMinY()));
            bb.setWidth(sround(n->getWidth()));
            bb.setHeight(sround(n->getHeight()));
            // apply bb to glyph
            tg->setBoundingBox(&bb);
            
            lay->addTextGlyph(tg);
            
            delete tg;
        }
        
        // add reactions
        for(Network::ConstRxnIt i=net->RxnsBegin(); i!=net->RxnsEnd(); ++i) {
            const Graphfab::Reaction* r = *i;
            AN(r, "Empty reaction");
            
            ReactionGlyph* rg = new ReactionGlyph();
            
            // id
            rg->setId(r->getId() + "_Glyph");
            
            // model reference
            rg->setReactionId(r->getId());
            
            // do species
            uint64 sref=0;
            Graphfab::Reaction::ConstNodeIt in=r->NodesBegin();
            Graphfab::Reaction::ConstCurveIt ic=r->CurvesBegin();
            for(;in != r->NodesEnd() && ic != r->CurvesEnd(); ++in, ++ic) {
                const Node* n = in->first;
                AN(n, "Empty species reference");
                const RxnBezier* c = *ic;
                AN(n, "Empty curve reference");
                
                SpeciesReferenceGlyph* srg = rg->createSpeciesReferenceGlyph();
                
                // set id
                {
                    std::stringstream ss;
                    ss << r->getId() << "_SpeciesRef" << ++sref;
                    srg->setId(ss.rdbuf()->str());
                }
                
                // set reference & glyph
                srg->setSpeciesReferenceId(n->getId());
                srg->setSpeciesGlyphId(n->getGlyph());
                
                // set role
                switch(in->second) {
                    case RXN_ROLE_SUBSTRATE:
                        srg->setRole("substrate");
                        break;
                    case RXN_ROLE_PRODUCT:
                        srg->setRole("product");
                        break;
                    case RXN_ROLE_SIDESUBSTRATE:
                        srg->setRole("sidesubstrate");
                        break;
                    case RXN_ROLE_SIDEPRODUCT:
                        srg->setRole("sideproduct");
                        break;
                    case RXN_ROLE_MODIFIER:
                        srg->setRole("modifier");
                        break;
                    case RXN_ROLE_ACTIVATOR:
                        srg->setRole("activator");
                        break;
                    case RXN_ROLE_INHIBITOR:
                        srg->setRole("inhibitor");
                        break;
                    default:
                        AN(0, "Unrecognized role");
                }
                
                // setup the SBML curve
                ::Curve curv;
                // cubic Bezier
                CubicBezier* cb = curv.createCubicBezier();
                
                ::Point p;
                
                // end-points
                p.setX(c->s.x);
                p.setY(c->s.y);
                cb->setStart(&p);
                p.setX(c->e.x);
                p.setY(c->e.y);
                cb->setEnd(&p);
                
                // control points
                p.setX(c->c1.x);
                p.setY(c->c1.y);
                cb->setBasePoint1(&p);
                p.setX(c->c2.x);
                p.setY(c->c2.y);
                cb->setBasePoint2(&p);
                
                // set curve
                srg->setCurve(&curv);
            }
            
            lay->addReactionGlyph(rg);
            
            delete rg;
            
            ::Reaction* reaction = model->createReaction();
            reaction->setId(r->getId());
            reaction->setReversible(false);
            reaction->setFast(false);
            ::KineticLaw* kine = reaction->createKineticLaw();
            kine->setFormula("1");
            for(Graphfab::Reaction::ConstNodeIt inode = r->NodesBegin();inode != r->NodesEnd(); ++inode) {
                switch(inode->second) {
                    case RXN_ROLE_SUBSTRATE: {
                        ::SpeciesReference* sref = reaction->createReactant();
                        sref->setSpecies((inode->first)->getId());
                        sref->setConstant(false);
                        sref->setStoichiometry(1.);
                        break;}
                    case RXN_ROLE_PRODUCT: {
                        ::SpeciesReference* sref = reaction->createProduct();
                        sref->setSpecies((inode->first)->getId());
                        sref->setConstant(false);
                        sref->setStoichiometry(1.);
                        break;}
                    case RXN_ROLE_SIDESUBSTRATE:{
                        ::SpeciesReference* sref = reaction->createReactant();
                        sref->setSpecies((inode->first)->getId());
                        sref->setConstant(false);
                        sref->setStoichiometry(1.);
                        break;}
                    case RXN_ROLE_SIDEPRODUCT:{
                        ::SpeciesReference* sref = reaction->createProduct();
                        sref->setSpecies((inode->first)->getId());
                        sref->setConstant(false);
                        sref->setStoichiometry(1.);
                        break;}
                    case RXN_ROLE_MODIFIER:{
                        ::ModifierSpeciesReference* sref = reaction->createModifier();
                        sref->setSpecies((inode->first)->getId());
                        break;}
                    case RXN_ROLE_ACTIVATOR:{
                        ::ModifierSpeciesReference* sref = reaction->createModifier();
                        sref->setSpecies((inode->first)->getId());
                        break;}
                    case RXN_ROLE_INHIBITOR:{
                        ::ModifierSpeciesReference* sref = reaction->createModifier();
                        sref->setSpecies((inode->first)->getId());
                        break;}
                    default:
                        AN(0, "Unrecognized role");
                }
            }
        }
    }
    
    return doc;
}

// DEPRECATED
gf_layoutInfo* gf_loadSBMLIntoLayoutEngine(const char* buf, gf_SBMLModel* r) {
	r=(gf_SBMLModel*)malloc(sizeof(gf_SBMLModel));
    SBMLReader reader;
    SBMLDocument* document = reader.readSBMLFromString(buf);
    
    AN(document, "Failed to parse SBML"); //not libSBML's documented way of failing, but just in case...
    
    if(document->getNumErrors()) {
        fprintf(stderr, "Failed to parse SBML\n");
        return NULL;
    }
    
    r->pdoc = document;

	gf_layoutInfo* l;
    SBMLDocument* doc = (SBMLDocument*)r->pdoc;
    AT(doc->isPkgEnabled("layout"), "Layout package not enabled");
    
    // get the model
    Model* mod = doc->getModel();
    AN(mod, "Failed to load model");
    
    // layout plugin ptr
    SBasePlugin* layoutBase = mod->getPlugin("layout");
    AN(layoutBase, "No plugin named \"layout\"");
    
    // cast to derived
    LayoutModelPlugin* layoutPlugin=NULL;
    try {
        layoutPlugin = dynamic_cast<LayoutModelPlugin*>(layoutBase);
    } catch(std::bad_cast e) {
        gf_emitError("Unable to get layout information");
        AN(0);
    }
    
    //determine if there is layout information present
    int have_layout;
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
    printf("Number of layouts: %d\n", layoutPlugin->getNumLayouts());
    #endif
    if(layoutPlugin->getNumLayouts() == 0)
        have_layout = 0;
    else
        have_layout = 1;
    if(layoutPlugin->getNumLayouts() > 1)
        gf_emitWarn("Warning: multiple layouts. Using first");
    const Layout* layout = layoutPlugin->getLayout(0);
    
    //construct the network
    Network* net=NULL;
    if(have_layout)
        net = networkFromLayout(*layout, *mod);
    else
        net = networkFromModel(*mod);
    AN(net, "Failed to construct network");
    
    //get the canvas
    Canvas* canv;
    if(have_layout) {
        canv = new Canvas();
        //get dimensions from SBML layout
        const Dimensions* dims = layout->getDimensions();
        canv->setWidth(dims->getWidth());
        canv->setHeight(dims->getHeight());
        #if SAGITTARIUS_DEBUG_LEVEL >= 2
        std::cout << "Canvas width = " << canv->getWidth() << ", height = " << canv->getHeight() << "\n";
        #endif
    } else {
        canv = new Canvas();
        //get dimensions from SBML layout
        canv->setWidth(1024);
        canv->setHeight(1024);
    }
    #if SAGITTARIUS_DEBUG_LEVEL >= 3
    //print
    net->dump(std::cout,0);
    #endif
    
    l = (gf_layoutInfo*)malloc(sizeof(gf_layoutInfo));
    gf_initLayoutInfo(l);
    
    l->net = net;
    
    l->canv = canv;
    
    return l;
}

void gf_setModelNamespace(gf_layoutInfo* l, unsigned long level, unsigned long version) {
  l->level = level;
  l->version = version;
}

void gf_layout_fit_to_window(gf_layoutInfo* l, double left, double top, double right, double bottom) {
    Network* net = (Network*)l->net;
    AN(net, "No network");
    net->fitToWindow(Box(left, top, right, bottom));
}

gf_network gf_getNetwork(gf_layoutInfo* l) {
    gf_network n;
    n.n = l->net;
    AN(n.n, "No network");
    return n;
}

gf_network* gf_getNetworkp(gf_layoutInfo* l) {
    gf_network* n = (gf_network*)malloc(sizeof(gf_network));
    n->n = l->net;
    AN(n->n, "No network");
    return n;
}

void gf_clearNetwork(gf_network* n) {
    n->n = NULL;
}

void gf_releaseNetwork(gf_network* n) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    
    delete net;
}

char* gf_nw_getID(gf_network* n) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    
    return gf_strclone(net->getId().c_str());
}

size_t gf_nw_getNumNodes(const gf_network* n) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    
    return net->getTotalNumNodes();
}

size_t gf_nw_getNumRxns(const gf_network* n) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    
    return net->getTotalNumRxns();
}

size_t gf_nw_getNumComps(const gf_network* n) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    
    return net->getTotalNumComps();
}

gf_node gf_nw_getNode(gf_network* n, size_t i) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    gf_node node;
    node.n = net->getNodeAt(i);
    return node;
}

gf_node* gf_nw_getNodep(gf_network* n, size_t i) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    gf_node* node = (gf_node*)malloc(sizeof(gf_node));
    node->n = net->getNodeAt(i);
    return node;
}

gf_reaction gf_nw_getRxn(gf_network* n, size_t i) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    gf_reaction r;
    r.r = net->getRxnAt(i);
    // optional
    Graphfab::Reaction* rxn = (Graphfab::Reaction*)r.r;
    AT(rxn->doByteCheck(), "Type verification failed");
    
    return r;
}

gf_reaction* gf_nw_getRxnp(gf_network* n, size_t i) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    gf_reaction* r = (gf_reaction*)malloc(sizeof(gf_reaction));
    r->r = net->getRxnAt(i);
    // optional
    Graphfab::Reaction* rxn = (Graphfab::Reaction*)r->r;
    AT(rxn->doByteCheck(), "Type verification failed");

    return r;
}

void gf_nw_removeRxn(gf_network* nw, gf_reaction* r) {
    Network* net = CastToNetwork(nw->n);
    Graphfab::Reaction* rx = CastToReaction(r->r);
    AN(net, "No network");
    AN(rx, "No reaction");
    
    net->removeReaction(rx);
}

gf_compartment gf_nw_getCompartment(gf_network* n, size_t i) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    gf_compartment c;
    c.c = net->getCompAt(i);
    return c;
}

gf_compartment* gf_nw_getCompartmentp(gf_network* n, size_t i) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    gf_compartment* c = (gf_compartment*)malloc(sizeof(gf_compartment));
    c->c = net->getCompAt(i);
    return c;
}

void gf_nw_rebuildCurves(gf_network* n) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    net->rebuildCurves();
}

void gf_nw_recenterJunctions(gf_network* n) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    net->recenterJunctions();
}

gf_node gf_nw_newNode(gf_network* nw, const char* id, const char* name, gf_compartment* compartment) {
    Network* net = CastToNetwork(nw->n);
    gf_node nd;
    nd.n = NULL;
    AN(net, "No network");
    
    std::cout << "gf_nw_newNode started\n";
    Node* n = new Node();
    
    std::cout << "gf_nw_newNode setting id\n";
    n->setName(name);
    if(id) {
        if(!net->findNodeById(id))
            n->setId(id);
        else {
            #if SAGITTARIUS_DEBUG_LEVEL >= 1
            fprintf(stderr, "A node with the specified id already exists\n");
            #endif
            return nd;
        }
    } else
        n->setId(net->getUniqueId());
    n->numUses() = 1;
    n->setAlias(false);
    
    if(compartment) {
        Graphfab::Compartment* c = (Graphfab::Compartment*)compartment->c;
        c->addElt(n);
        n->_comp = c;
    }
    
    // set index
    n->set_i(net->getUniqueIndex());
    
    net->addNode(n);
    
//     std::cout << "gf_nw_newNode: node = " << n << ", index " << n->get_i() << "\n";
    
    nd.n = n;
    return nd;
}

gf_node* gf_nw_newNodep(gf_network* nw, const char* id, const char* name, gf_compartment* compartment) {
  gf_node* r = (gf_node*)malloc(sizeof(gf_node));
  gf_node q = gf_nw_newNode(nw,  id,  name, compartment);
  r->n = q.n;
  return r;
}

int gf_nw_removeNode(gf_network* nw, gf_node* n) {
    Network* net = CastToNetwork(nw->n);
    Node* node = CastToNode(n->n);
    
    if(!net->containsNode(node)) {
        #if SAGITTARIUS_DEBUG_LEVEL >= 1
        fprintf(stderr, "gf_nw_removeNode: no such node in network\n");
        #endif
        return -1;
    }
    
    try {
        net->removeNode(node);
    } catch(...) {
        #if SAGITTARIUS_DEBUG_LEVEL >= 1
        fprintf(stderr, "gf_nw_removeNode: unable to remove node from network\n");
        #endif
        return -1;
    }
    
    return 0;
}

int gf_nw_isLayoutSpecified(gf_network* nw) {
    Network* net = CastToNetwork(nw->n);
    
    if(net->isLayoutSpecified())
        return 1;
    else
        return 0;
}

// Node

void gf_node_setCompartment(gf_node* n, gf_compartment* c) {
  Graphfab::Compartment* comp = (Graphfab::Compartment*)c->c;
  AN(comp, "No comp");
  Node* node = CastToNode(n->n);

  comp->addElt(node);
}

void gf_clearNode(gf_node* n) {
    n->n = NULL;
}

void gf_releaseNode(const gf_node* n) {
    Node* node = CastToNode(n->n);
    AN(node, "No node");
    
    delete node;
}

CPoint Point2CPoint(const Graphfab::Point& p) {
    CPoint q;
    q.x = p.x;
    q.y = p.y;
    return q;
}

gf_point Point2gf_point(const Graphfab::Point& p) {
    gf_point q;
    q.x = p.x;
    q.y = p.y;
    return q;
}

Graphfab::Point CPoint2Point(const CPoint& p) {
    Graphfab::Point q(p.x, p.y);
    return q;
}

Graphfab::Point gf_point2Point(const gf_point& p) {
    Graphfab::Point q(p.x, p.y);
    return q;
}

// alias a node
int gf_node_alias(gf_node* n, gf_network* m) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    Network* net = (Network*)m->n;
    AT(net->doByteCheck(), "Network has wrong type");

    return node->alias(net);
}

// is aliased?
int gf_node_isAliased(gf_node* n) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    return node->isAlias();
}

// is locked?
int gf_node_isLocked(gf_node* n) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    return node->isLocked();
}

// lock
void gf_node_lock(gf_node* n) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    node->lock();
}

// unlock
void gf_node_unlock(gf_node* n) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    node->unlock();
}

// node.centroid
gf_point gf_node_getCentroid(gf_node* n) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    gf_point p = Point2gf_point(node->getCentroid(NetworkElement::COORD_SYSTEM_GLOBAL));
    
    return p;
}

void gf_node_setCentroid(gf_node* n, gf_point p) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    
    node->setGlobalCentroid(gf_point2Point(p));
}

// node.width
double gf_node_getWidth(gf_node* n) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    return node->getGlobalWidth();
}

void gf_node_setWidth(gf_node* n, double width) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    node->affectGlobalWidth(width);
}

// node.height
double gf_node_getHeight(gf_node* n) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    return node->getGlobalHeight();
}

void gf_node_setHeight(gf_node* n, double height) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    node->affectGlobalHeight(height);
}

char* gf_node_getID(gf_node* n) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    
    return gf_strclone(node->getId().c_str());
}

const char* gf_node_getName(gf_node* n) {
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");
    
    return node->getId().c_str();
}

int gf_node_getConnectedReactions(gf_node* n, gf_network* m, unsigned int* num, gf_reaction** rxns) {
    size_t k;
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");

    Network* net = CastToNetwork(m->n);
    AN(net && net->doByteCheck(), "No network");

    Graphfab::Network::AttachedRxnList rx = net->getConnectedReactions(node);

    *num = rx.size();

    *rxns = (gf_reaction*)malloc((*num)*sizeof(gf_reaction));

    for (k = 0; k < rx.size(); ++k) {
        (*rxns)[k].r = (void*)rx.at(k);
    }

    return 0;
}

int gf_node_getAttachedCurves(gf_node* n, gf_network* m, unsigned int* num, gf_curve** curves) {
    size_t k;
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");

    Network* net = CastToNetwork(m->n);
    AN(net && net->doByteCheck(), "No network");

    Graphfab::Network::AttachedCurveList rc = net->getAttachedCurves(node);

    *num = rc.size();

    *curves = (gf_curve*)malloc((*num)*sizeof(gf_curve));

    for (k = 0; k < rc.size(); ++k) {
        (*curves)[k].c = (void*)rc.at(k);
    }

    return 0;
}

// Reaction

void gf_releaseRxn(const gf_reaction* r) {
    Graphfab::Reaction* rxn = (Graphfab::Reaction*) r->r;
    AN(rxn, "No rxn");
    AT(rxn->doByteCheck(), "Type verification failed");
    
    delete rxn;
}

char* gf_reaction_getID(gf_reaction* r) {
    Graphfab::Reaction* rxn = (Graphfab::Reaction*) r->r;
    AN(rxn, "No rxn");
    AT(rxn->doByteCheck(), "Type verification failed");
    
    return gf_strclone(rxn->getId().c_str());
}

// reaction.centroid
gf_point gf_reaction_getCentroid(gf_reaction* r) {
    Graphfab::Reaction* rxn = (Graphfab::Reaction*) r->r;
    AN(rxn, "No rxn");
    AT(rxn->doByteCheck(), "Type verification failed");

    gf_point p = Point2gf_point(rxn->getCentroid(NetworkElement::COORD_SYSTEM_GLOBAL));

    return p;
}

void gf_reaction_setCentroid(gf_reaction* r, gf_point p) {
    Graphfab::Reaction* rxn = (Graphfab::Reaction*) r->r;
    AN(rxn && rxn->doByteCheck(), "Not a reaction");

    rxn->setGlobalCentroid(gf_point2Point(p));
}

size_t gf_reaction_getNumSpec(const gf_reaction* r) {
    Graphfab::Reaction* rxn = (Graphfab::Reaction*) r->r;
    AN(rxn, "No rxn");
    AT(rxn->doByteCheck(), "Type verification failed");
    
    return rxn->numSpecies();
}

int gf_reaction_hasSpec(const gf_reaction* r, const gf_node* n) {
    Graphfab::Reaction* rxn = (Graphfab::Reaction*) r->r;
    AN(rxn, "No rxn");
    AT(rxn->doByteCheck(), "Type verification failed");
    Node* node = CastToNode(n->n);
    AN(node && node->doByteCheck(), "Not a node");

    return rxn->hasSpecies(node);
}

gf_specRole RxnRoleType2gf_specRole(RxnRoleType role) {
    switch(role) {
        case RXN_ROLE_SUBSTRATE: return GF_ROLE_SUBSTRATE;
        case RXN_ROLE_PRODUCT: return GF_ROLE_PRODUCT;
        case RXN_ROLE_SIDESUBSTRATE: return GF_ROLE_SIDESUBSTRATE;
        case RXN_ROLE_SIDEPRODUCT: return GF_ROLE_SIDEPRODUCT;
        case RXN_ROLE_MODIFIER: return GF_ROLE_MODIFIER;
        case RXN_ROLE_ACTIVATOR: return GF_ROLE_ACTIVATOR;
        case RXN_ROLE_INHIBITOR: return GF_ROLE_INHIBITOR;
        default:
            AN(0, "Unknown role type");
            return GF_ROLE_SUBSTRATE;
    }
}

gf_specRole gf_reaction_getSpecRole(const gf_reaction* r, size_t i) {
    Graphfab::Reaction* rxn = (Graphfab::Reaction*) r->r;
    AN(rxn, "No rxn");
    AT(rxn->doByteCheck(), "Type verification failed");
    
    return RxnRoleType2gf_specRole(rxn->getSpeciesRole(i));
}

const char* gf_roleToStr(gf_specRole role) {
    switch(role) {
        case GF_ROLE_SUBSTRATE: return "SUBSTRATE";
        case GF_ROLE_PRODUCT: return "PRODUCT";
        case GF_ROLE_SIDESUBSTRATE: return "SIDESUBSTRATE";
        case GF_ROLE_SIDEPRODUCT: return "SIDEPRODUCT";
        case GF_ROLE_MODIFIER: return "MODIFIER";
        case GF_ROLE_ACTIVATOR: return "ACTIVATOR";
        case GF_ROLE_INHIBITOR: return "INHIBITOR";
        default:
            AN(0, "Unknown role type");
            return "UNKNOWN";
    }
}

gf_specRole gf_strToRole(const char* str) {
  if (!strcmp(str, "SUBSTRATE"))
    return GF_ROLE_SUBSTRATE;
  else if (!strcmp(str, "SIDESUBSTRATE"))
    return GF_ROLE_SIDESUBSTRATE;
  else if (!strcmp(str, "PRODUCT"))
    return GF_ROLE_PRODUCT;
  else if (!strcmp(str, "SIDEPRODUCT"))
    return GF_ROLE_SIDEPRODUCT;
  else if (!strcmp(str, "ACTIVATOR"))
    return GF_ROLE_ACTIVATOR;
  else if (!strcmp(str, "INHIBITOR"))
    return GF_ROLE_INHIBITOR;
  else if (!strcmp(str, "MODIFIER"))
    return GF_ROLE_MODIFIER;
  else {
    fprintf(stderr, "gf_strToRole unknown role type %s", str);
    AN(0, "Unknown role type");
  }
}

size_t gf_reaction_specGeti(const gf_reaction* r, size_t i) {
    Graphfab::Reaction* rxn = (Graphfab::Reaction*) r->r;
    AN(rxn, "No rxn");
    AT(rxn->doByteCheck(), "Type verification failed");
    
    return rxn->getSpecies(i)->get_i();
}

size_t gf_reaction_getNumCurves(const gf_reaction* r) {
    Graphfab::Reaction* rxn = (Graphfab::Reaction*) r->r;
//     std::cerr << "gf_reaction_getNumCurves type verify\n";
    
    AN(rxn, "No rxn");
    AT(rxn->doByteCheck(), "Type verification failed");

//     std::cerr << "gf_reaction_getNumCurves type verified\n";

    return rxn->getNumCurves();
}

gf_curve gf_reaction_getCurve(const gf_reaction* r, size_t i) {
    Graphfab::Reaction* rxn = (Graphfab::Reaction*) r->r;
    AN(rxn, "No rxn");
    AT(rxn->doByteCheck(), "Type verification failed");
    gf_curve c;
    c.c = rxn->getCurve(i);
    
    return c;
}

gf_curve* gf_reaction_getCurvep(const gf_reaction* r, size_t i) {
    gf_curve q = gf_reaction_getCurve(r, i);
    gf_curve* p = (gf_curve*)malloc(sizeof(gf_curve));
    p->c = q.c;
    return p;
}

void gf_reaction_recenter(gf_reaction* r) {
    Graphfab::Reaction* rxn = (Graphfab::Reaction*) r->r;
    AN(rxn, "No rxn");
    AT(rxn->doByteCheck(), "Type verification failed");
    rxn->recenter();
}

void gf_reaction_recalcCurveCPs(gf_reaction* r) {
    Graphfab::Reaction* rxn = (Graphfab::Reaction*) r->r;
    AN(rxn, "No rxn");
    AT(rxn->doByteCheck(), "Type verification failed");
    rxn->recalcCurveCPs();
}

void gf_releaseCurve(const gf_curve* c) {
    RxnBezier* curve = (RxnBezier*)c->c;
    AN(curve, "No curve");
    
    delete curve;
}

gf_curveCP gf_getLocalCurveCPs(const gf_curve* c) {
    RxnBezier* curve = (RxnBezier*)c->c;
    AN(curve, "No curve");
    gf_curveCP cp;
    
    cp.s = Point2gf_point(curve->s);
    cp.e = Point2gf_point(curve->e);
    cp.c1 = Point2gf_point(curve->c1);
    cp.c2 = Point2gf_point(curve->c2);
    
    return cp;
}

gf_curveCP gf_getGlobalCurveCPs(const gf_curve* c) {
    RxnBezier* curve = (RxnBezier*)c->c;
    AN(curve, "No curve");
    gf_curveCP cp;
    
    cp.s = Point2gf_point(curve->getTransformedS());
    cp.e = Point2gf_point(curve->getTransformedE());
    cp.c1 = Point2gf_point(curve->getTransformedC1());
    cp.c2 = Point2gf_point(curve->getTransformedC2());
    
    return cp;
}

gf_specRole gf_curve_getRole(gf_curve* c) {
  RxnBezier* curve = (RxnBezier*)c->c;
  AN(curve, "No curve");

  if (dynamic_cast<SubCurve*>(curve))
    return GF_ROLE_SUBSTRATE;

  if (dynamic_cast<PrdCurve*>(curve))
    return GF_ROLE_PRODUCT;

  if (dynamic_cast<ModCurve*>(curve))
    return GF_ROLE_MODIFIER;

  if (dynamic_cast<ActCurve*>(curve))
    return GF_ROLE_ACTIVATOR;

  if (dynamic_cast<InhCurve*>(curve))
    return GF_ROLE_INHIBITOR;

  // default
  return GF_ROLE_SUBSTRATE;
}

gf_curveCP gf_getCurveCPs(const gf_curve* c) {
    return gf_getGlobalCurveCPs(c);
}

int gf_curve_hasArrowhead(const gf_curve* c) {
  RxnBezier* curve = (RxnBezier*)c->c;
  AN(curve, "No curve");

  return curve->hasArrowhead();
}

int gf_curve_getArrowheadVerts(const gf_curve* c, unsigned int* n, gf_point** v) {
  RxnBezier* curve = (RxnBezier*)c->c;
  AN(curve, "No curve");

  Arrowhead* a = curve->getArrowhead();

//   std::cerr << "  a->getNumVerts() = " << a->getNumVerts() << "\n";
  *n = a->getNumVerts();
//   std::cerr << "  *n = " << *n << "\n";

  *v = (gf_point*)malloc((*n) *sizeof(gf_point));

  for (unsigned int k = 0; k<*n; ++k)
    (*v)[k] = Point2gf_point(a->getTransformedVert(k));

  delete a;
//   std::cerr << "  *n2 = " << *n << "\n";

  return 0;
}

void gf_releaseCompartment(const gf_compartment* c) {
    Graphfab::Compartment* comp = (Graphfab::Compartment*)c->c;
    AN(comp, "No comp");
    
    delete comp;
}

char* gf_compartment_getID(gf_compartment* c) {
    Graphfab::Compartment* comp = (Graphfab::Compartment*)c->c;
    AN(comp, "No comp");
    
    return gf_strclone(comp->getId().c_str());
}

gf_point gf_compartment_getMinCorner(gf_compartment* c) {
    Graphfab::Compartment* comp = (Graphfab::Compartment*)c->c;
    AN(comp, "No comp");
    
    return Point2gf_point(comp->getMin(NetworkElement::COORD_SYSTEM_GLOBAL));
}

void gf_compartment_setMinCorner(gf_compartment* c, gf_point p) {
    Graphfab::Compartment* comp = (Graphfab::Compartment*)c->c;
    AN(comp, "No comp");
    
    comp->setMin(gf_point2Point(p));
}

gf_point gf_compartment_getMaxCorner(gf_compartment* c) {
    Graphfab::Compartment* comp = (Graphfab::Compartment*)c->c;
    AN(comp, "No comp");
    
    return Point2gf_point(comp->getMax(NetworkElement::COORD_SYSTEM_GLOBAL));
}

void gf_compartment_setMaxCorner(gf_compartment* c, gf_point p) {
    Graphfab::Compartment* comp = (Graphfab::Compartment*)c->c;
    AN(comp, "No comp");
    
    comp->setMax(gf_point2Point(p));
}

double gf_compartment_getWidth(gf_compartment* c) {
    Graphfab::Compartment* comp = (Graphfab::Compartment*)c->c;
    AN(comp, "No comp");

    return comp->getGlobalWidth();
}

double gf_compartment_getHeight(gf_compartment* c) {
    Graphfab::Compartment* comp = (Graphfab::Compartment*)c->c;
    AN(comp, "No comp");

    return comp->getGlobalHeight();
}

size_t gf_compartment_getNumElt(gf_compartment* c) {
    Graphfab::Compartment* comp = (Graphfab::Compartment*)c->c;
    AN(comp, "No comp");
    
    return comp->getNElts();
}

void gf_fit_to_window(gf_layoutInfo* l, double left, double top, double right, double bottom) {
    Network* net = (Network*)l->net;
    AN(net, "No network");
    
    Graphfab::Box bbox = net->getBoundingBox();
    
//     std::cerr << "Net bounding box: " << bbox << "\n";
    
    Graphfab::Box window(left, top, right, bottom);
    
//     std::cout << "Window: " << window << "\n";
    
    Graphfab::Affine2d tf = Graphfab::Affine2d::FitToWindow(bbox, 
                                                   window);
    
//     std::cout << "Transform is\n" << tf;
    
    net->setTransform(tf);
    net->setInverseTransform(tf.inv());
    
//     std::cerr << "New bounding box: " << net->getBoundingBox() << "\n";
}

gf_transform* gf_tf_fitToWindow(gf_layoutInfo* l, double left, double top, double right, double bottom) {
    Network* net = (Network*)l->net;
    AN(net, "No network");

    Graphfab::Box bbox = net->getBoundingBox();

//     std::cerr << "Net bounding box: " << bbox << "\n";

    Graphfab::Box window(left, top, right, bottom);

//     std::cout << "Window: " << window << "\n";

    Graphfab::Affine2d* tf = new Graphfab::Affine2d(Graphfab::Affine2d::FitToWindow(bbox,
                                                   window));

//     std::cout << "Transform is\n" << tf;

//     net->setTransform(tf);
//     net->setInverseTransform(tf.inv());

    gf_transform* t = (gf_transform*)malloc(sizeof(gf_transform));
    t->tf = tf;
    return t;
}

CPoint gf_tf_apply_to_point(gf_transform* tf, CPoint p) {
    Graphfab::Affine2d* t = (Graphfab::Affine2d*)tf->tf;
    AN(t, "No transform");
    Graphfab::Point r = Graphfab::xformPoint(CPoint2Point(p), *t);
    return Point2CPoint(r);
}

gf_point gf_tf_getScale(gf_transform* tf) {
  Graphfab::Affine2d* t = (Graphfab::Affine2d*)tf->tf;
  AN(t, "No transform");
  return Point2gf_point(t->getScale());
}

gf_point gf_tf_getDisplacement(gf_transform* tf) {
  Graphfab::Affine2d* t = (Graphfab::Affine2d*)tf->tf;
  AN(t, "No transform");
//   std::cerr << "  gf_tf_getDisplacement: " << t->getDisplacement() << "\n";
  return Point2gf_point(t->getDisplacement());
}

gf_point gf_tf_getPostDisplacement(gf_transform* tf) {
  Graphfab::Affine2d* t = (Graphfab::Affine2d*)tf->tf;
  AN(t, "No transform");
  Graphfab::Point result(t->inv().applyLinearOnly(t->getDisplacement()));
//   std::cerr << "  gf_tf_getPostDisplacement: " << result << "\n";
  return Point2gf_point(result);
}

void gf_dump_transform(gf_transform* tf) {
    Graphfab::Affine2d* t = (Graphfab::Affine2d*)tf->tf;
    AN(t, "No transform");
//     std::cerr << *t;
}

void gf_release_transform(gf_transform* tf) {
    Graphfab::Affine2d* t = (Graphfab::Affine2d*)tf->tf;
    AN(t, "No transform");
    delete t;
}

gf_canvas gf_getCanvas(gf_layoutInfo* l) {
    gf_canvas c = {l->canv};
    return c;
}

gf_canvas* gf_getCanvasp(gf_layoutInfo* l) {
    gf_canvas c = {l->canv};
    gf_canvas* r = (gf_canvas*)malloc(sizeof(gf_canvas));
    r->canv = c.canv;
    return r;
}

void gf_clearCanvas(gf_canvas* c) {
    c->canv = NULL;
}

void gf_releaseCanvas(gf_canvas* c) {
    Canvas *canv = (Canvas*)c->canv;
    AN(canv, "No canvas");
    
    delete canv;
}

unsigned int gf_canvGetWidth(gf_canvas* c) {
    Canvas *canv = (Canvas*)c->canv;
    AN(canv, "No canvas");
    
    return canv->getWidth();
}

void gf_canvSetWidth(gf_canvas* c, unsigned long width) {
    Canvas *canv = (Canvas*)c->canv;
    AN(canv, "No canvas");
    
    canv->setWidth(width);
}

unsigned int gf_canvGetHeight(gf_canvas* c) {
    Canvas *canv = (Canvas*)c->canv;
    AN(canv, "No canvas");
    
    return canv->getHeight();
}

void gf_canvSetHeight(gf_canvas* c, unsigned long height) {
    Canvas *canv = (Canvas*)c->canv;
    AN(canv, "No canvas");
    
    canv->setHeight(height);
}

int gf_writeSBMLwithLayout(const char* filename, gf_SBMLModel* m, gf_layoutInfo* l) {
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     std::cout << "gf_writeSBMLwithLayout started\n" << std::endl;
    #endif
    SBMLDocument* doc = populateSBMLdoc(m,l);
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
//     std::cout << "populateSBMLdoc finished\n" << std::endl;
    #endif
    SBMLWriter writer;
    writer.setProgramName("Graphfab");
    if(writer.writeSBML(doc, filename))
        return 0;
    else
        return -1;
    
    // appropriate?
    //delete lay;
}

int gf_writeSBML(const char* filename, gf_SBMLModel* m) {
    SBMLDocument* doc = populateSBMLdoc(m, NULL);
    SBMLWriter writer;
    writer.setProgramName("Graphfab");
    if(writer.writeSBML(doc, filename))
        return 0;
    else
        return -1;
}

const char* gf_getSBMLwithLayoutStr(gf_SBMLModel* m, gf_layoutInfo* l) {
    SBMLDocument* doc = populateSBMLdoc(m,l);
    SBMLWriter writer;
    writer.setProgramName("Graphfab");
    
    if(l->cont)
        free(l->cont);
    l->cont = writer.writeSBMLToString(doc);
    
    return gf_strclone(l->cont);
}

void gf_randomizeLayout(gf_layoutInfo* m) {
    Network* net = (Network*)m->net;
    AN(net, "No network");
    Canvas* can = (Canvas*)m->canv;
    AN(can, "No canvas");
    
    net->randomizePositions(Graphfab::Box(Graphfab::Point(0.,0.), Graphfab::Point(can->getWidth(), can->getHeight())));
}

void gf_randomizeLayout2(gf_network* n, gf_canvas* c) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");
    Canvas* can = (Canvas*)c->canv;
    AN(can, "No canvas");
    
    net->randomizePositions(Graphfab::Box(Graphfab::Point(0.,0.), Graphfab::Point(can->getWidth(), can->getHeight())));
}

void gf_randomizeLayout_fromExtents(gf_network* n, double left, double top, double right, double bottom) {
    Network* net = CastToNetwork(n->n);
    AN(net, "No network");

    net->randomizePositions(Graphfab::Box(Graphfab::Point(left,top), Graphfab::Point(right, bottom)));
}

//TODO: move to more appropriate place like core/version or something
// and rename to more succinct function - is anyone going to want to get the version
// for anything other than the CURRENT library?
const char* gf_getCurrentLibraryVersion() {
    return GF_EXPAND_AND_QUOTE(SBNW_MAJOR_VER) "." GF_EXPAND_AND_QUOTE(SBNW_MINOR_VER) "." GF_EXPAND_AND_QUOTE(SBNW_PATCHLEVEL);
}

void gf_free(void* x) {
  free(x);
}

gf_point gf_computeCubicBezierPoint(gf_curveCP* c, Graphfab::Real t) {
  CubicBezier2Desc b(gf_point2Point(c->s), gf_point2Point(c->c1), gf_point2Point(c->c2), gf_point2Point(c->e));
  return Point2gf_point(b.p(t));
}

gf_point* gf_computeCubicBezierLineIntersec(gf_curveCP* c, gf_point* line_start, gf_point* line_end) {
  Line2Desc l(gf_point2Point(*line_start), gf_point2Point(*line_end));

  CubicBezier2Desc b(gf_point2Point(c->s), gf_point2Point(c->c1), gf_point2Point(c->c2), gf_point2Point(c->e));

  CubicBezierIntersection r(l, b);

  gf_point* result = (gf_point*)malloc((r.getIntersectionPoints().size()+1)*sizeof(gf_point));

  for (int i = 0; i<r.getIntersectionPoints().size(); ++i) {
    result[i].x = b.p(r.getIntersectionPoints().at(i)).x;
    result[i].y = b.p(r.getIntersectionPoints().at(i)).y;
  }

  result[r.getIntersectionPoints().size()].x = 0;
  result[r.getIntersectionPoints().size()].y = 0;

  return result;
}
int gf_arrowheadStyleGetNumVerts(int style) {
  return Graphfab::ArrowheadStyles::getNumVerts(style);
}

gf_point gf_arrowheadStyleGetVert(int style, int n) {
  return Point2gf_point(Graphfab::ArrowheadStyles::getVert(style,  n));
}

int gf_arrowheadStyleIsFilled(int style) {
  return Graphfab::ArrowheadStyles::isFilled(style);
}

unsigned long gf_arrowheadNumStyles() {
  return Graphfab::ArrowheadStyles::count();
}

void gf_arrowheadSetStyle(gf_specRole role, int style) {
  switch(role) {
    case GF_ROLE_SUBSTRATE:
    case GF_ROLE_SIDESUBSTRATE:
      ArrowheadStyleControl<SubstrateArrowhead>::set(style);
      break;
    case GF_ROLE_PRODUCT:
    case GF_ROLE_SIDEPRODUCT:
      ArrowheadStyleControl<ProductArrowhead>::set(style);
      break;
    case GF_ROLE_MODIFIER:
      ArrowheadStyleControl<ModifierArrowhead>::set(style);
      break;
    case GF_ROLE_ACTIVATOR:
      ArrowheadStyleControl<ActivatorArrowhead>::set(style);
      break;
    case GF_ROLE_INHIBITOR:
      ArrowheadStyleControl<InhibitorArrowhead>::set(style);
      break;
    default:
      fprintf(stderr, "gf_arrowheadSetStyle unknown role type %s\n", gf_roleToStr(role));
      AN(0, "Unknown role type");
  }
}

int gf_arrowheadGetStyle(gf_specRole role) {
  switch(role) {
    case GF_ROLE_SUBSTRATE:
    case GF_ROLE_SIDESUBSTRATE:
      return ArrowheadStyleControl<SubstrateArrowhead>::get();
    case GF_ROLE_PRODUCT:
    case GF_ROLE_SIDEPRODUCT:
      return ArrowheadStyleControl<ProductArrowhead>::get();
    case GF_ROLE_MODIFIER:
      return ArrowheadStyleControl<ModifierArrowhead>::get();
    case GF_ROLE_ACTIVATOR:
      return ArrowheadStyleControl<ActivatorArrowhead>::get();
    case GF_ROLE_INHIBITOR:
      return ArrowheadStyleControl<InhibitorArrowhead>::get();
      break;
    default:
      fprintf(stderr, "gf_arrowheadSetStyle unknown role type %s\n", gf_roleToStr(role));
      AN(0, "Unknown role type");
  }
}
