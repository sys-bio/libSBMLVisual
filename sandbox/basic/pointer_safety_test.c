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

typedef struct {
  int x;
} A;
typedef struct {
  char dummy;
} Awrap;
typedef Awrap* Aref;

typedef struct {
  int y;
} B;
typedef struct {
  char dummy;
} Bwrap;
typedef Bwrap* Bref;

int Afunc(Aref a) {
  if (a)
    return ((A*)a)->x;
  else
    return 0;
}

int Bfunc(Bref b) {
  if (b)
    return ((B*)b)->y;
  else
    return 0;
}

int main(int argc, char* argv[]) {
    A a;
    B b;

    Aref ap = (Aref)&a;
    Bref bp = (Bref)&b;

    Afunc(ap);
    Bfunc(bp);

//     Afunc(bp); // Warning
//     Bfunc(ap); // Warning

    return 0;
}