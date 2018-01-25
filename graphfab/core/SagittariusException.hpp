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

#ifndef _SAGITTARIUS_EXCEPTION_H_
#define _SAGITTARIUS_EXCEPTION_H_

//== INCLUDES ========================================================================

#include "graphfab/core/SagittariusCommon.h"
#include "graphfab/core/SagittariusException.hpp"
#include "graphfab/util/string.h"
#include <exception>

namespace Graphfab
{
    
    //== CLASSES =====================================================================
    
    //########################################################
    //CLASS Exception
    //  Used in place of std::exception within Sagittarius
    //  Provides more detailed exception information
    //  (such as file and line number)
	//  Inspired by Ogre exceptions
    //########################################################
    class _GraphfabExport Exception : public std::exception
    {
        public:
            enum ExceptionTypeSet
            {
                EXC_UNKNOWN,
                EXC_FREAD_FAIL,
                EXC_FWRITE_FAIL,
                EXC_ACCESS_VIOLATION,
                EXC_SANITY_CHECK,
                EXC_REDUNDANCY_CHECK_FAILURE,
                EXC_IMPOSSIBLE_CMD,
                EXC_INVALID_PARAMETER,
                EXC_INTERNAL_CHECK_FAILURE
            };
            
            Exception( const int type, const String& desc, const String& origin, const char* name, const char* file, const long line);
            
            ~Exception() throw() {}
            
            void operator = ( const Exception &rval );
            
            virtual String getReport() const;
            
            //Can't have a const int return type or warnings about ignored qualifiers. Stupid compiler.
            //Idea: maybe it really SHOULD be const int. Think about it.
            virtual int getType() const throw();
            
            virtual const String& getDescription() const { return m_desc; }
            
            virtual const String& getOrigin() const { return m_origin; }
            
            virtual const String& getFile() const { return m_file; }
            
            virtual long getLine() const { return m_line; }
            
            //Override std::exception::what
            const char* what() const throw() {
              // leak
              return gf_strclone(getReport().c_str());
            }
            
        protected:
            int m_type;
            String m_desc;
            String m_origin;
            String m_name;
            String m_file;
            long m_line;
            
    };
    
    template <int t>
    struct ExceptionType
    {
        enum { type = t };
    };
    
    //Exception classes
    
    /**
        This exception should not be thrown in good practice
        Instead, a more descriptive variant should be used
        If one does not exist, it should be created
    */
    class _GraphfabExport UnknownException : public Exception
    {
        public:
            UnknownException( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_UNKNOWN, desc, origin, "UnknownException", file, line) {}
    };
    
    /**
        Thrown whenever a file read operation fails
    */
    class FileReadFailureException : public Exception
    {
        public:
            FileReadFailureException( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_FREAD_FAIL, desc, origin, "FileReadFailureException", file, line) {}
    };
    
    /**
        Thrown whenever when a file write operation fails
    */
    class _GraphfabExport FileWriteFailureException : public Exception
    {
        public:
            FileWriteFailureException( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_FWRITE_FAIL, desc, origin, "FileWriteFailureException", file, line) {}
    };
    
    /**
        Thrown whenever the application attempts to access data that does not exist
        (at lease, not at the expected location)
    */
    class _GraphfabExport AccessViolationException : public Exception
    {
        public:
            AccessViolationException( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_ACCESS_VIOLATION, desc, origin, "AccessViolationException", file, line) {}
    };
    
    /**
        Thrown whenever data or directives do not make logical sense
    */
    class _GraphfabExport SanityCheckException : public Exception
    {
        public:
            SanityCheckException( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_SANITY_CHECK, desc, origin, "SanityCheckException", file, line) {}
    };
    
    /**
        Redundancy checks can take many forms, but a failure of one always
        means the same thing - there is inconsistent data present.
        These redundancy checks are used throughout Sagittarius/Akra to
        provide forewarning against an impending cascade of failures.
    */
    class _GraphfabExport RedundancyCheckFailureException : public Exception
    {
        public:
            RedundancyCheckFailureException( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_REDUNDANCY_CHECK_FAILURE, desc, origin, "RedundancyCheckFailureException", file, line) {}
    };
    
    /**
        Thrown whenever the application is asked to perform something impossible
    */
    class _GraphfabExport ImpossibleCommandException : public Exception
    {
        public:
            ImpossibleCommandException( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_IMPOSSIBLE_CMD, desc, origin, "ImpossibleCommandException", file, line) {}
    };
    
    /**
        Thrown whenever an invalid parameter is passed to a function
    */
    class _GraphfabExport InvalidParameterException : public Exception
    {
        public:
            InvalidParameterException( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_INVALID_PARAMETER, desc, origin, "InvalidParameterException", file, line) {}
    };
    
    /**
        An internal check is a check performed by various objects and functions to insure
        that the data they use is reliable.  A failure probably means that the data was either
        corrupted or never initialized in the first place
    */
    class _GraphfabExport InternalCheckFailureException : public Exception
    {
        public:
            InternalCheckFailureException( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_INTERNAL_CHECK_FAILURE, desc, origin, "InternalCheckFailureException", file, line) {}
    };
    
    /**
        Thrown whenever a general runtime error occurs
    */
    class RuntimeException : public Exception
    {
        public:
            RuntimeException( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_SANITY_CHECK, desc, origin, "RuntimeException", file, line) {}
    };
    
    class MissingDataException : public Exception
    {
        public:
            MissingDataException( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_SANITY_CHECK, desc, origin, "MissingDataException", file, line) {}
    };
    
    class UnexpectedDataException : public Exception
    {
        public:
            UnexpectedDataException( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_SANITY_CHECK, desc, origin, "UnexpectedDataException", file, line) {}
    };
    
    class BoundsCheckFailure : public Exception
    {
        public:
            BoundsCheckFailure( const String& desc, const String& origin, const char* file, const long line )
                : Exception( Exception::EXC_SANITY_CHECK, desc, origin, "BoundsCheckFailure", file, line) {}
    };
    
    //########################################################
    //CLASS ExceptionFactory
    //  Generates specific exceptions (i.e. derived classes of 
    //  Exception) based on the type argument
    //  One of the few cases in which the factory can return 
    //  by value
    //########################################################
    class ExceptionFactory
    {
        public:
            static UnknownException create( ExceptionType< Exception::EXC_UNKNOWN >,
                const String& desc, const String& origin,
                const char* file, const long line) {
                    return UnknownException(desc, origin, file, line);
                }
            static FileReadFailureException create( ExceptionType< Exception::EXC_FREAD_FAIL >,
                const String& desc, const String& origin,
                const char* file, const long line) {
                    return FileReadFailureException(desc, origin, file, line);
                }
            static FileWriteFailureException create( ExceptionType< Exception::EXC_FWRITE_FAIL >,
                const String& desc, const String& origin,
                const char* file, const long line) {
                    return FileWriteFailureException(desc, origin, file, line);
                }
            static AccessViolationException create( ExceptionType< Exception::EXC_ACCESS_VIOLATION >,
                const String& desc, const String& origin,
                const char* file, const long line) {
                    return AccessViolationException(desc, origin, file, line);
                }
            static SanityCheckException create( ExceptionType< Exception::EXC_SANITY_CHECK >,
                const String& desc, const String& origin,
                const char* file, const long line) {
                    return SanityCheckException(desc, origin, file, line);
                }
            static RedundancyCheckFailureException create( ExceptionType< Exception::EXC_REDUNDANCY_CHECK_FAILURE >,
                const String& desc, const String& origin,
                const char* file, const long line) {
                    return RedundancyCheckFailureException(desc, origin, file, line);
                }
            static ImpossibleCommandException create( ExceptionType< Exception::EXC_IMPOSSIBLE_CMD >,
                const String& desc, const String& origin,
                const char* file, const long line) {
                    return ImpossibleCommandException(desc, origin, file, line);
                }
            static InvalidParameterException create( ExceptionType< Exception::EXC_INVALID_PARAMETER >,
                const String& desc, const String& origin,
                const char* file, const long line) {
                    return InvalidParameterException(desc, origin, file, line);
                }
            static InternalCheckFailureException create( ExceptionType< Exception::EXC_INTERNAL_CHECK_FAILURE >,
                const String& desc, const String& origin,
                const char* file, const long line) {
                    return InternalCheckFailureException(desc, origin, file, line);
                }
    };
    
#define SBNW_THROW( type, desc, origin ) throw type( desc, origin, __FILE__, __LINE__ );
} //namespace Sagittarius

#endif
