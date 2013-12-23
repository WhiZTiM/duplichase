/*******************************************************************************************
**  (C) Copyright September 2013 by
**  @author: Ibrahim Timothy Onogu {WhiZTiM}
**  @email: <ionogu@acm.org>
**
**	Provided this copyright notice appears on all derived works;
**  Use, modification and distribution are subject to the Boost Software License,
**  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
**  http://www.boost.org/LICENSE_1_0.txt).
**
**  Project DupLichaSe...2013
**  See http://github.com/WhiZTiM/duplichase for most recent version including documentation.
********************************************************************************************/
#ifndef DLS_FILERULES_H
#define DLS_FILERULES_H
#include "backend/include/rule_factory/filerules.hpp"
#include "backend/include/rule_factory/rule_bag.hpp"

namespace DLS
{
    //!Supposedly Supposed an abstraction for dynamic rules and and functions regarding it
    class dls_filerules
    {
        public:
            dls_filerules();
        protected:
        private:
    };


    //! Inline funtion that generates the rules in a static manner
    //! \NOTE FRANKLY THIS IS A VERY STUPID IMPLEMENTATION:
    //! The next maintainer should please overhaul this crap ...that is, if He understands it
    inline std::vector< rule_facade<Path, FileProperty>* >
            makeBag01(RuleBag<Path, FileProperty>& rulebg)
    {
        RuleSequence<Path, FileProperty> rules;
        typedef rule_facade<Path, FileProperty>* VEC_RF;
        std::vector<VEC_RF> V_RF;

        //! 0 bytes to 1KB
        V_RF.push_back( new FR_by_size(0, 1023, rule_option::Dismiss) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(100) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 1KB to 50KB
        V_RF.push_back( new FR_by_size(1024, 51199) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(100) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 50Kb to 250KB
        V_RF.push_back( new FR_by_size(51200, 262143) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(100) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 250KB to 1MB
        V_RF.push_back( new FR_by_size(262144, 1048575) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(30) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 1MB to 8MB
        V_RF.push_back( new FR_by_size(1048576, 8388607) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(15) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 8MB to 24MB
        V_RF.push_back( new FR_by_size(8388608, 25165823) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(13) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 24MB to 64MB
        V_RF.push_back( new FR_by_size(25165824, 67108863) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(10) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 64MB to 256MB
        V_RF.push_back( new FR_by_size(67108864, 268435455) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(9) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 256MB to 1GB
        V_RF.push_back( new FR_by_size(67108864, 1073741823) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(7) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 1GB to 4GB
        V_RF.push_back( new FR_by_size(1073741824, 4294967295) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(5) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();

        return V_RF;
    }
}
#endif // DLS_FILERULES_H
