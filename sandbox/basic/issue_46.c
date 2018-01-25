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

#include "graphfab/core/SagittariusCore.h"

#include <stdio.h>
#include <string.h>

#include "graphfab/sbml/autolayoutSBML.h"
#include "graphfab/interface/layout.h"
#include "graphfab/layout/fr.h"

#include <stdlib.h>

int main(int argc, char* argv[]) {
    gf_SBMLModel *model = gf_SBMLModel_newp();  // is this call required?
    gf_layoutInfo *layout = gf_layoutInfo_newp (2, 4, 1000, 1000);  // success
    gf_network *network = gf_getNetworkp (layout);  // success
    gf_setDefaultCompartmentId ("compartment");  // returns true
    // Create a master (unique) node
    gf_node *node = gf_nw_newNodep (network, "Node0", "myNode0", NULL); // success

    // next create an alias associated with the node we just created
    gf_node *mynode = gf_nw_getUniqueNodep (network, 0); // success, 0 is the node index
    gf_node *myalias = gf_nw_newAliasNodep (network, node); // ACCESS VIOLATION HERE
    
    return 0;
}