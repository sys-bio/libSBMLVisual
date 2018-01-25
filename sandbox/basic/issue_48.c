/*== SAGITTARIUS =====================================================================
 * Copyright (c) 2016, Jesse K Medley
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

#include "graphfab/interface/layout.h"
#include "graphfab/layout/fr.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    gf_layoutInfo *layout = gf_layoutInfo_newp (2, 4, 1000, 1000);  // success
    gf_network *network = gf_getNetworkp (layout);  // success
    gf_setDefaultCompartmentId ("compartment");  // returns true

    // Create three nodes
    gf_node *n1 = gf_nw_newNodep (network, "n1", "n1", NULL);
    gf_point p1 = {0.,0.};
    gf_node_setCentroid(n1,p1);
    gf_node *n2 = gf_nw_newNodep (network, "n2", "n2", NULL);
    gf_point p2 = {50.,50.};
    gf_node_setCentroid(n2,p2);
    gf_node *n3 = gf_nw_newNodep (network, "n3", "n3", NULL);
    gf_point p3 = {100.,0.};
    gf_node_setCentroid(n3,p3);

    // make sure functions to get node via index work
    n1 = gf_nw_getUniqueNodep(network, 0);
    n2 = gf_nw_getUniqueNodep(network, 1);
    n3 = gf_nw_getUniqueNodep(network, 2);

    printf("Initial coords:\n  (%.2f,%.2f)-(%.2f,%.2f)-(%.2f,%.2f)\n",
           gf_node_getCentroid(n1).x, gf_node_getCentroid(n1).y,
           gf_node_getCentroid(n2).x, gf_node_getCentroid(n2).y,
           gf_node_getCentroid(n3).x, gf_node_getCentroid(n3).y);

    // Lock the nodes
    gf_node_lock(n1);
    gf_node_lock(n2);
    gf_node_lock(n3);

    // Run layout algorithm
    fr_options opt;
    gf_getLayoutOptDefaults(&opt);
    gf_doLayoutAlgorithm(opt,layout);

    printf("Final coords:\n  (%.2f,%.2f)-(%.2f,%.2f)-(%.2f,%.2f)\n",
           gf_node_getCentroid(n1).x, gf_node_getCentroid(n1).y,
           gf_node_getCentroid(n2).x, gf_node_getCentroid(n2).y,
           gf_node_getCentroid(n3).x, gf_node_getCentroid(n3).y);

    return 0;
}