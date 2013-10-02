#ifndef EXTENSION_AGGREGATOR_H
#define EXTENSION_AGGREGATOR_H

#include <string>
#include <vector>
#include <unordered_set>

namespace DLS
{

    //! Creates a list of extensions for the FR_by_extension class
    class ConditionedList
    {
    public:

        typedef std::vector<std::string>        vectorContainer;
        typedef std::unordered_set<std::string> unordered_setContainer;

        enum class Option { Inclusion, Exclusion };

        //! Adds a list
        ConditionedList(size_t threshold = 32);

        //! Extension Add
        void add(const std::string& extension, Option opt = Option::Inclusion);

        /* !!TODO
        //! merges the contents of the List
        void add(const ConditionedList& conditionedList);
        */

        //! sets whether everyother condition should result \a value in Option
        bool setEveryOther(Option opt, bool value = true);

        //! returns whether anything outside added items is accepted in the inclusion or Exclusion list
        bool getEveryOther(Option opt);

        //! returns true if excluded --> performance critical
        bool excludes(const std::string& extension) const;

        //! includes    ---> performace critical
        bool includes(const std::string& extension) const;

        //! inclusive extension count
        inline size_t getInclusionCount() const
        { return i_count; }

        //! exclusive extension count
        inline size_t getExclusionCount() const
        { return e_count; }

    private:

        //! first resort
        //! i_vCon -> inclusion vector container
        //! e_vCon -> exclusion vector container
        vectorContainer i_vCon, e_vCon;

        //! second resort Container
        //! i_uCon -> inclusion unordered_set Container
        //! e_uCon -> exclusion unordered_set Container
        unordered_setContainer i_uCon, e_uCon;

        //! just a counter and a threshold value
        size_t i_count = 0;
        size_t e_count = 0;
        size_t verge;

        //! is it low?
        bool i_low = true;
        bool e_low = true;

        //! others
        bool i_others = false;
        bool e_others = false;

        //! is it transfered
        bool i_metamorphosed = false;
        bool e_metamorphosed = false;
    };



    class Extension_Aggregator
    {
        public:
            Extension_Aggregator();
            ~Extension_Aggregator();
        protected:
        private:
    };

}

#endif // EXTENSION_AGGREGATOR_H
