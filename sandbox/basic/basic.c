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

#include "graphfab/core/SagittariusCore.h"

#include <stdio.h>
#include <string.h>

#include "graphfab/sbml/gf_autolayoutSBML.h"
#include "graphfab/sbml/gf_layout.h"
#include "graphfab/layout/gf_fr.h"

#if SBNW_USE_MAGICK
#include "graphfab/draw/gf_magick.h"
#endif

#include <stdlib.h>

///CompareArg
//Compares the two strings
//Will fail if lengths do not match
int CompareArg(const char* arg, const char* cmp)
{
    while(*arg == *cmp && *arg != '\0' && *cmp != '\0')
    {
        ++arg;
        ++cmp;
    }
    if(*arg == *cmp)
        return 1;
    else
        return 0;
}
///CompareArg_Short
//Compares two strings until null char is found
//Terminates at null char, returning 1 if strings match
//up to first occurrence of null char
int CompareArg_Short(const char* arg, const char* cmp)
{
    while(1) {
        if(*arg == '\0' || *cmp == '\0')
            return 1;
        if(*arg != *cmp)
            break;
        ++arg;
        ++cmp;
    }
    return 0;
}

int IsUIntegerString(const char* arg)
{
    //ASCII:
    //0x30 = '0'
    //0x39 = '9'
    while(0x30 <= *arg && *arg <= 0x39) {
        ++arg;
    }
    if(*arg == '\0')
        return 1;
    else
        return 0;
}

unsigned int String2UInt(const char* arg)
{
    unsigned int result=0;
    //ASCII:
    //0x30 = '0'
    //0x39 = '9'
    while(0x30 <= *arg && *arg <= 0x39)
    {
        result *= 10;
        result += (unsigned int)(*arg-0x30);
        ++arg;
    }
    assert(*arg == '\0');
    return result;
}

//int becuase getc returns int
int buf_pushc(char** buf, size_t* n, size_t* size, int c) {
    char *tmpbuf;
    if(!(*buf)) {
        *size = 256;
        *buf = malloc(*size);
        *n = 1;
        (*buf)[0] = c;
    } else {
        if(*n == *size) {
            tmpbuf = malloc(*size << 2);
            memcpy(tmpbuf, *buf, *size);
            *size <<= 2;
            free(*buf);
            *buf = tmpbuf;
        }
        (*buf)[(*n)++] = c;
    }
    return c;
}

int main(int argc, char* argv[]) {
    int i;
    char* buf;
    size_t bytes_read;
    int chr;
    const char* defaultout = "/tmp/graphfab_out.xml";
    const char* outfile = defaultout;
    
    size_t size=0, n;
    
    FILE* file=NULL;

	fprintf(stderr, "Graphfab version: %s\n", gf_getCurrentLibraryVersion());
    
    for(i=1; i<argc; ++i)
    {
        //output
        if(CompareArg_Short(argv[i], "-o"))
        {
            if(++i >= argc) {
                fprintf(stderr, "Expected file name after \"-o\"");
                return -1;
            }
            outfile = argv[i];
        } else {
            //not recognized
            continue;
        }
    }
    
    /*open*/
    if(argc >= 2) {
        fprintf(stderr, "Specified file is %s\n", argv[argc-1]);
        file = fopen(argv[argc-1], "rb");
        if(!file)
        {
			fprintf(stderr, "Failed to open file\n"); // Assert elided on release build, which we have to use thanks to libsbml
            assert(0 && "Failed to open file");
            return -1;
        }
        //get t3h s!z3
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        rewind(file);
        fprintf(stderr, "File size is %lu\n", size);
        assert(size > 0);
        
        //allocated buffer
        fprintf(stderr, "Allocate buf\n");
        assert(sizeof(char) == 1 && "char must be one byte wide");
        buf=(char*)malloc(size+1); //one extra byte for null char
        assert(buf && "Failed to allocate buffer");
        fprintf(stderr, "Allocated buf\n");
        
        //read the whole file at once
        bytes_read = fread(buf, 1, size, file);
        assert(bytes_read == size && "Failed to read whole file (wrong size specified?)");
        //trip EOF indicator
        fgetc(file);
        assert(feof(file) && "EOF Expected");
        buf[size] = '\0'; //terminating null char
        
        /*close*/
        fclose(file);
    } else {
        fprintf(stderr, "Using standard input\n");
        buf = NULL;
        while(1) {
            chr = getc(stdin);
            if(chr == EOF)
                break;
            buf_pushc(&buf, &n, &size, chr);
        }
        buf_pushc(&buf, &n, &size, '\0');
    }
    
    {
        //type to store layout info
        gf_layoutInfo* l;
        
        //load the model
        fprintf(stderr, "Load model\n");
        gf_SBMLModel* mod = gf_loadSBMLbuf(buf);
		fprintf(stderr, "Loaded model\n");

		{
			// test last error
			fprintf(stderr, "Last error: %s\n", gf_getLastError());
		}

        fr_options opt;
        
        //read layout info from the model
        fprintf(stderr, "Process layout\n");
        l = gf_processLayout(mod);
        srand(10000);
//         srand((unsigned)time(NULL));
        //randomize node positions
        gf_randomizeLayout(l);
        #if SBNW_USE_MAGICK
          gf_MagickRenderToFile(l, "/tmp/tmpfs/random.png", NULL);
        #endif

        {
          // test obj model
          gf_network* nw = gf_getNetworkp(l);

          // may leak
          printf("Network ID: %s\n", gf_nw_getID(nw));
          printf("Network # nodes: %zu\n", gf_nw_getNumNodes(nw));

          gf_node* n = gf_nw_getNodep(nw, 0);
          printf("Node ID: %s\n", gf_node_getID(n));

          gf_free(n);
          gf_free(nw);
        }
        
        //do layout algo
        gf_getLayoutOptDefaults(&opt);
        fprintf(stderr, "Run layout\n");
        gf_doLayoutAlgorithm(opt, l);
        #if SBNW_USE_MAGICK
            gf_MagickRenderToFile(l, "/tmp/tmpfs/plot.png", NULL);
        #endif
        //save layout information to new SBML file
        gf_writeSBMLwithLayout(outfile, mod, l);
        //run destructors on the model
        gf_freeSBMLModel(mod);
        //run destructors on the layout
        gf_freeLayoutInfoHierarch(l);
    }
    
    free(buf);
    
    printf("Done\n");
    
    return 0;
}