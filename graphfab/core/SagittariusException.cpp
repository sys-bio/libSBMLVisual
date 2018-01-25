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

//- GENERAL -//
#include <sstream>

//- SPECIFIC -//
#include "graphfab/core/SagittariusException.hpp"

namespace Graphfab
{
    //== CLASS METHODS ===============================================================
    
    //## CLASS Exception #############################################################
    Exception::Exception( const int type, const String& desc, const String& origin, const char* name, const char* file, const long line)
            : m_type( type ), m_desc( desc ), m_origin( origin ), m_name(name), m_file( file ), m_line( line ) {}
        //note: logging the exception (like Ogre) is probably a bad idea (assuming the log function
        //itself can throw exceptions).  We try to log the exception, then the log function throws
        //another exception.  We try to log that exception, and the cycle repeats ad infinitum.
        //We could declare the logging function with const throw, preventing it from throwing exceptions, but
        //then we must come up with another error handling mechanism.  All in all, I don't think
        //it's worth it.
    
    void Exception::operator = ( const Exception &rval )
    {
        m_type   = rval.m_type;
        m_desc   = rval.m_desc;
        m_origin = rval.m_origin;
        m_name   = rval.m_name;
        m_file   = rval.m_file;
        m_line   = rval.m_line;
    }
    
    String Exception::getReport() const
    {
        String report;
        std::stringstream s;
        s << m_line;
        String linestr;
        s >> linestr;
        
        report  = "EXCEPTION: ";
        report += m_name;
        report += " in ";
        report += m_origin;
        report += " (file: ";
        report += m_file;
        report += ", line: ";
        report += linestr;
        report += "):";
        report += "    ";
        report += m_desc;
        report += "\n";
        return report;
    }
    
    int Exception::getType() const throw()
    {
        return m_type;
    }
} //namespace Sagittarius
