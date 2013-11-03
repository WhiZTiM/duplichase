#include <boost/algorithm/string.hpp>
#include "useful_functions.hpp"

//using namespace DLS;
using namespace boost;

    bool DLS::isTrackableTo(const std::string& child, const std::string& parent)
    {
        std::vector<std::string> vParent, vChild;
        vParent = algorithm::split(vParent, parent, algorithm::is_any_of("/"), algorithm::token_compress_on);
        vChild = algorithm::split(vChild, child, algorithm::is_any_of("/"), algorithm::token_compress_on);

        if(vParent.size() >= vChild.size())
            return false;

        std::size_t k(0);
        while(k < vParent.size())
        {
            if(vParent[k] != vChild[k])
                return false;
            ++k;
        }

        return (k == vParent.size());
    }


    /*/! A blawdy O(n^2) algorithm
    std::vector<std::string>
    overlappingPathVector(const std::vector<std::list>& newList, const std::vector<std::string>& oldList)
    {
        std::vector<std::string> rtn;

    }
    */
