/** *******************************************************
    Copyright 2009 Kyle Medley
      All Rights Reserved
    
THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********************************************************/

//== BEGINNING OF CODE ===============================================================

#ifndef __SAGITTARIUS_DEFAULT_BOOL_H_
#define __SAGITTARIUS_DEFAULT_BOOL_H_

//== INCLUDES ========================================================================

//==DEFINES/TYPES===================================//

namespace Sagittarius
{
    
    class DefaultFalseBool
    {
        private:
            bool b;
        public:
            DefaultFalseBool() : b(false) {}
            DefaultFalseBool(const DefaultFalseBool& other) : b(other.b) {}
            bool& get() { return b; }
            const bool& get() const { return b; }
    };
    
    class DefaultTrueBool
    {
        private:
            bool b;
        public:
            DefaultTrueBool() : b(true) {}
            DefaultTrueBool(const DefaultTrueBool& other) : b(other.b) {}
            bool& get() { return b; }
            const bool& get() const { return b; }
    };
    
}

#endif
