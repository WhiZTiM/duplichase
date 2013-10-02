
#ifndef FP_HOLDERS_HXX
#define FP_HOLDERS_HXX

#include <string>
#include <deque>
#include <memory>
#include <functional>
#include <unordered_set>
#include "backend/include/fileproperty.hpp"
#include "backend/include/exception.hpp"

namespace DLS
{

    //! convenience function
    typedef std::deque<FileProperty> VEC_FileProperty;
    typedef std::shared_ptr<VEC_FileProperty> ptrVEC_FileProperty;

    /*! hash funtor for found files. . .
     */
     class FH_VEC_FileProperty
     {
        public:
        std::size_t operator () (const ptrVEC_FileProperty& val) const
        {
            if(val->empty())     //! THIS IS WRONG TO INSERT AN EMPTY ARRAY || \WARTNING: Fire an error
                //return 0x001;       //! const value for empty set
                throw DLS::Exception("VERY STUPID PROGRAMMING ERROR: An attempt was made to insert an vector");

            return std::hash<std::string>()(val->back().getHashB());
        }
        std::size_t operator () (const VEC_FileProperty& val) const
        {
            if(val.empty())     //! THIS IS WRONG TO INSERT AN EMPTY ARRAY || \WARTNING: Fire an error
                //return 0x001;       //! const value for empty set
                throw DLS::Exception("VERY STUPID PROGRAMMING ERROR: An attempt was made to insert an vector");

            return std::hash<std::string>()(val.back().getHashB());
        }
     };

     class FE_VEC_FileProperty
     {
        public:
        size_t operator () (const ptrVEC_FileProperty& lhs, const ptrVEC_FileProperty& rhs) const
        {
            if(lhs->empty() || rhs->empty())
                throw DLS::Exception("Illegal attempt to campare one or two empty vectors");

            return ( (lhs->back().getSize() == rhs->back().getSize())   &&
                     (lhs->back().getHashA() == rhs->back().getHashA()) &&
                     (lhs->back().getHashB() == rhs->back().getHashB())
                   );
        }

        size_t operator () (const VEC_FileProperty& lhs, const VEC_FileProperty& rhs) const
        {
            if(lhs.empty() || rhs.empty())
                throw DLS::Exception("Illegal attempt to campare one or two empty vectors");
            return ( (lhs.back().getSize() == rhs.back().getSize())       &&
                     (lhs.back().getHashA() == rhs.back().getHashA())   &&
                     (lhs.back().getHashB() == rhs.back().getHashB())
                   );
        }
     };

     typedef std::unordered_set<ptrVEC_FileProperty, FH_VEC_FileProperty, FE_VEC_FileProperty> DuplicatesContainer;

}

#endif
