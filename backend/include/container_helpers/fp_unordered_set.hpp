#ifndef FP_UNORDERED_SET_HXX
#define FP_UNORDERED_SET_HXX

#include <string>
#include <unordered_set>
#include "backend/include/rule_factory/rule_bag.hpp"
#include "backend/include/rule_factory/dls_filerules.hpp"
#include "backend/include/fileproperty.hpp"
#include <functional>

namespace DLS
{

    class F_FP_size;
    class FE_FP_size;

    class F_FP_hashA;
    class FE_FP_hashA;

    class F_FP_hashB;
    class FE_FP_hashB;

    class F_FP_hashC;
    class FE_FP_hashC;

    class F_FP_extension;
    class FE_FP_extension;

    ///////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////

    typedef std::unordered_set<FileProperty, F_FP_size, FE_FP_size>             FPSet_size;
    typedef std::unordered_set<FileProperty, F_FP_hashA, FE_FP_hashA>           FPSet_hashA;
    typedef std::unordered_set<FileProperty, F_FP_hashB, FE_FP_hashB>           FPSet_hashB;
    typedef std::unordered_set<FileProperty, F_FP_hashC, FE_FP_hashC>           FPSet_hashC;
    typedef std::unordered_set<FileProperty, F_FP_extension, FE_FP_extension>   FPSet_extension;

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    template<typename FPSet>
    inline bool insertInto(const FileProperty& fp, FPSet& container)
    {
        if(container.find(fp) == container.end())
        {
            container.insert(fp);
            return true;
        }

        return false;
    }


    template<typename FPSet>
    inline bool existIn(const FileProperty& input, const FPSet& container)
    {
        if(container.find(input) == container.end())
            return false;
        return true;
    }


    template<typename FPSet>
    inline bool existIn(const FileProperty& input, const FPSet& container, FileProperty& output)
    {
        auto iter = container.find(input);

        if(iter == container.end())
            return false;

        output = *iter;
        return true;
    }

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////


    class F_FP_size
    {
    public:
        size_t operator () (const FileProperty& fp) const
        { return std::hash<unsigned long>()(fp.getSize()); }
    };

    class FE_FP_size
    {
    public:
        bool operator () (const FileProperty& fp1, const FileProperty& fp2) const
        { return fp1.getSize() == fp2.getSize(); }
    };


    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    class F_FP_hashA
    {
    public:
        size_t operator () (const FileProperty& fp) const
        { return std::hash<std::string>()(fp.getHashA()); }
    };

    class FE_FP_hashA
    {
    public:
        bool operator () (const FileProperty& fp1, const FileProperty& fp2) const
        { return fp1.getHashA() == fp2.getHashA(); }
    };


    //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    class F_FP_hashB
    {
    public:
        size_t operator () (const FileProperty& fp) const
        { return std::hash<std::string>()(fp.getHashB()); }
    };

    class FE_FP_hashB
    {
    public:
        bool operator () (const FileProperty& fp1, const FileProperty& fp2) const
        { return fp1.getHashB() == fp2.getHashB(); }
    };


    //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    class F_FP_hashC
    {
    public:
        size_t operator () (const FileProperty& fp) const
        { return std::hash<std::string>()(fp.getHashC()); }
    };

    class FE_FP_hashC
    {
    public:
        bool operator () (const FileProperty& fp1, const FileProperty& fp2) const
        { return fp1.getHashC() == fp2.getHashC(); }
    };


    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    class F_FP_extension
    {
    public:
        size_t operator () (const FileProperty& fp) const
        { return std::hash<std::string>()(fp.getFileExtension()); }
    };

    class FE_FP_extension
    {
    public:
        bool operator () (const FileProperty& fp1, const FileProperty& fp2) const
        { return fp1.getFileExtension() == fp2.getFileExtension(); }
    };
}


#endif
