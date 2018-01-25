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
#include "graphfab/sbml/autolayoutSBML.h"
#include "graphfab/diag/error.h"

#include "sbml/SBMLTypes.h"
#include <sstream>

void gf_freeSBMLModel(gf_SBMLModel* lo) {
    if(!lo)
        AN(0, "Not a valid layout pointer"); //null
    SBMLDocument* doc = (SBMLDocument*)lo->pdoc;
    delete doc;
    free(lo);
}

extern "C" gf_SBMLModel* gf_loadSBMLbuf(const char* buf) {
    gf_SBMLModel* r=(gf_SBMLModel*)malloc(sizeof(gf_SBMLModel));
    SBMLReader reader;
    SBMLDocument* doc = reader.readSBMLFromString(buf);
    
    AN(doc, "Failed to parse SBML"); //not libSBML's documented way of failing, but just in case...
    
    if(doc->getNumErrors()) {
        #if SAGITTARIUS_DEBUG_LEVEL >= 2
        fprintf(stderr, "Failed to parse SBML\n");
        for(unsigned int i=0; i<doc->getNumErrors(); ++i) {
            std::cerr << "Error " << i << ": " <<doc->getError(i)->getMessage() << "\n";
        }
        std::stringstream ss;
        ss << "Failed to parse SBML\n";
        for(unsigned int i=0; i<doc->getNumErrors(); ++i) {
            ss << "Error " << i << ": " <<doc->getError(i)->getMessage() << "\n";
        }
        gf_setError(ss.str().c_str());
        #endif
        // if all are warnings, continue - else abort
        for(unsigned int i=0; i<doc->getNumErrors(); ++i) {
          if (!doc->getError(i)->isWarning())
            return NULL;
        }
    }
    
    r->pdoc = doc;
    return r;
}

extern "C" gf_SBMLModel* gf_loadSBMLfile(const char* path) {
  char* buf;
  size_t size=0;
  FILE* file=NULL;
  size_t bytes_read;

  try {

    file = fopen(path, "rb");
    if(!file)
      SBNW_THROW(Graphfab::InternalCheckFailureException, "Failed to open file", "gf_loadSBMLfile");

    //get t3h s!z3
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    #if SAGITTARIUS_DEBUG_LEVEL >= 2
  //     fprintf(stderr, "File size is %lu\n", size);
    #endif
    assert(size > 0);

    //allocated buffer
    if (sizeof(char) != 1)
      SBNW_THROW(Graphfab::InternalCheckFailureException, "char must be one byte wide", "gf_loadSBMLfile");
    buf=(char*)malloc(size+1); //one extra byte for null char
    if (!buf)
      SBNW_THROW(Graphfab::InternalCheckFailureException, "Failed to allocate buffer", "gf_loadSBMLfile");
    //read the whole file at once
    bytes_read = fread(buf, 1, size, file);
    if (bytes_read != size)
      SBNW_THROW(Graphfab::InternalCheckFailureException, "Failed to read whole file (wrong size specified?)", "gf_loadSBMLfile");
    //trip EOF indicator
    fgetc(file);
    if (!feof(file))
      SBNW_THROW(Graphfab::InternalCheckFailureException, "EOF Expected", "gf_loadSBMLfile");
    buf[size] = '\0'; //terminating null char

    /*close*/
    fclose(file);

    gf_SBMLModel* mod = gf_loadSBMLbuf(buf);

    free(buf);

    return mod;

  } catch (const Graphfab::Exception& e) {
    gf_setError( e.getReport().c_str() );
    return NULL;
  }
}


