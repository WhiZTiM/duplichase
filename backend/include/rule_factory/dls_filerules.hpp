#ifndef DLS_FILERULES_H
#define DLS_FILERULES_H
#include "backend/include/rule_factory/filerules.hpp"
#include "backend/include/rule_factory/rule_bag.hpp"

namespace DLS
{

    class dls_filerules
    {
        public:
            dls_filerules();
        protected:
        private:
    };


    //! Inline funtion that generates the rules in a static manner
    //! \NOTE FRANKLY THIS IS A VERY STUPID IMPLEMENTATION:
    //! The next maintainer should please overhaul this crap
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

    /*
    inline void makeBagA(RuleBag<Path, FileProperty>& rulebg)
    {
        RuleSequence<Path, FileProperty> rules;

        FR_by_size *rs1 = new FR_by_size(0, 1000, false);
        FR_by_hashA *rh1 = new FR_by_hashA(100);
        rules.AddRule(rs1);
        rules.AddRule(rh1);
        rulebg.AddRule(rules);
        rules.Clear();


        FR_by_size* rs2 = new FR_by_size(1000, 10000, true);
        FR_by_hashA* rh2 = new FR_by_hashA(100);
        rules.AddRule(rs2);
        rules.AddRule(rh2);
        rulebg.AddRule(rules);
        rules.Clear();

        FR_by_size* rs3 = new FR_by_size(10000, 100000, true);
        FR_by_hashA* rh3 = new FR_by_hashA(100);
        rules.AddRule(rs3);
        rules.AddRule(rh3);
        rulebg.AddRule(rules);
        rules.Clear();

        FR_by_size* rs4 = new FR_by_size(100000, 1000000, true);
        FR_by_hashA* rh4 = new FR_by_hashA(30);
        rules.AddRule(rs4);
        rules.AddRule(rh4);
        rulebg.AddRule(rules);
        rules.Clear();

        FR_by_size* rs5 = new FR_by_size(1000000, 30000000, true);
        FR_by_hashA* rh5 = new FR_by_hashA(15);
        rules.AddRule(rs5);
        rules.AddRule(rh5);
        rulebg.AddRule(rules);
        rules.Clear();

        FR_by_size* rs6 = new FR_by_size(30000000, 100000000, true);
        FR_by_hashA* rh6 = new FR_by_hashA(13);
        rules.AddRule(rs6);
        rules.AddRule(rh6);
        rulebg.AddRule(rules);
        rules.Clear();

        FR_by_size* rs7 = new FR_by_size(100000000, 500000000, true);
        FR_by_hashA* rh7 = new FR_by_hashA(9);
        rules.AddRule(rs7);
        rules.AddRule(rh7);
        rulebg.AddRule(rules);
        rules.Clear();

    } */

}
#endif // DLS_FILERULES_H
