/*******************************************************************************************
**  (C) Copyright August 2013 - September 2013 by
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
#ifndef RULEBAG_H
#define RULEBAG_H

#include "backend/include/rule_facade/rule_facade.hpp"
#include <vector>
#include <utility>
#include <iostream>
namespace DLS
{
    template<typename T, typename Result = T>
    class RuleSequence
    {
    public:

        //! an alias for convenience
        typedef std::vector<rule_facade<T, Result>* > Container;
        typedef typename Container::iterator ContainerIterator;

        //! Constructor: Setting \a Destroy to true makes this class to delete the Added objects at destruction of this class
        RuleSequence();

        //RuleSequence(const RuleSequence& seq) = delete;

        //! Adds an object inherited from rule_facade by pointer
        void AddRule(rule_facade<T, Result>* ptrRule);

        //! returns the next descending rule...
        inline rule_facade<T, Result>* NextRule();

        //! returns a pointer to the current dereferenced rule_facade<iterator>
        inline rule_facade<T, Result>* ThisRule() const;

        //! returns if you can ask for the next sequence...
        bool NextIsAvailable() const;

        //! if the internall vector is empty
        bool Empty() const;

        //! Clears all added rule
        void Clear(bool deleteRules = false);

        //! just changes the internal iterator to the begin() of the storage container
        void ResetIterator();

        //! gets the pointers
        inline Container getContainerElements() const
        { return rules; }

        //RuleSequence<T, Result>& operator = (const RuleSequence<T, Result>& rhs);

        ~RuleSequence();

        /*! \brief makes deep copy of a rule_facade item be calling on Clone()
        *
        */
        static RuleSequence<T, Result> deepCopy(const RuleSequence<T, Result>& source);

    private:
        //! private vector that holds the rules
        Container rules;

        //! iterator to the _rules that can be moved in a const object
        ContainerIterator rulesIterator;

        //! Developer choice:: should this distruct the rules at exit?
        unsigned stepper;
    };



/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////


    template<typename T, typename Result>
    RuleSequence<T, Result>::RuleSequence()
    {
        //std::cout << "Rule Sequence constructed!" << std::endl;
        //_iter = _rules.begin();
        stepper = 0;
    }


    template<typename T, typename Result>
    void RuleSequence<T, Result>::AddRule(rule_facade<T, Result>* ptrRule)
    {
        if(ptrRule != nullptr)
        rules.push_back(ptrRule);
        //rulesIterator = rules.begin();  //currently not in use
        stepper = 0;
    }


    template<typename T, typename Result>
    rule_facade<T, Result>* RuleSequence<T, Result>::NextRule()
    {
        if(!NextIsAvailable()) return nullptr;
            return rules[stepper++];
    }


    template<typename T, typename Result>
    rule_facade<T, Result>* RuleSequence<T, Result>::ThisRule() const
    {
        //return *_iter;
        return rules[ (stepper > 0 ? (stepper - 1) : stepper)];
    }


    template<typename T, typename Result>
    bool RuleSequence<T, Result>::NextIsAvailable() const
    {
        //return _iter != _rules.end();
        return  (unsigned((stepper + 1)) <= rules.size() );
    }


    template<typename T, typename Result>
    bool RuleSequence<T, Result>::Empty() const
    {
        return rules.empty();
    }


    template<typename T, typename Result>
    void RuleSequence<T, Result>::Clear(bool deleteRules)
    {
        if(deleteRules)
            for(auto& it : rules)
                delete it;

        rules.clear();
    }


    template<typename T, typename Result>
    void RuleSequence<T, Result>::ResetIterator()
    {
        //_iter = _rules.begin();
        stepper = 0;
    }


    template<typename T, typename Result>
    RuleSequence<T, Result>::~RuleSequence()
    {
        //
    }

    template<typename T, typename Result>
    RuleSequence<T, Result> RuleSequence<T, Result>::deepCopy(const RuleSequence<T, Result>& source)
    {
        RuleSequence<T, Result> rtn;
        Container ct = source.getContainerElements();
        for(auto& it : ct)
            rtn.AddRule(it.Clone());
        return rtn;
    }



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


    template<typename T, typename Result = T, typename Option = rule_option>
    class RuleBag
    {
    public:

        //! An alias for convenience sake
        typedef std::vector<RuleSequence<T, Result> > Container;
        typedef typename Container::iterator ContainerIterator;
        typedef typename Container::const_iterator ConstContainerIterator;

        //! deletes all element pointers at the destruction
        RuleBag(bool deleteAll = false) { _deleteAtDestruction = deleteAll; }
        //! Object type constructor
        RuleBag(const T& object);
        //! Move constructor for object
        RuleBag(T&& object);

        //! Copy constructor, has more adverse effect than good. use the static function deepCopy
        RuleBag(const RuleBag<T>&) = delete;

        //! Move constructor
        RuleBag(const RuleBag<T>&&);

        //! Adds a const RuleSequence by reference
        void AddRule(const RuleSequence<T, Result>& rule);

        //! sets the object to be ruled against
        void set(const T& object);

        //! sets Deletion option and returns what will be
        bool deleteAtDestruction(bool);

        //! clears all current rules
        void clearRules();

        //! resets the iterator used to iterate the rules internally to start from beginning
        void resetIterator();

        //! if you arent satisfied with the current rule, go down again into the current RuleSequence and return true if matched
        bool nextRule();

        //! same as above but this directly modifies the \a result and returns \var true if modified
        bool nextRule(Result& result);

        //! automatically finds a matching rule and returns true if found
        bool findMatchingRule() = delete;

        //! returns true if no matching rule was found at all... i.e if internal iterator is == end iterator
        inline bool failedAll() const;

        //! returns the option specified for that kind of file
        Option getOption() const;

        //! returns a const rule_facade<> to the current rule
        const rule_facade<T, Result>* getThisRule() const;

        //! Returns \var Result verdict for current rule
        Result getVerdict() const;

        //! Modifies the argument \a result according to current rule facade object
        bool getVerdict(Result& result) const;

        bool getIndependentVerdict(const T& object, Result& resulT) const;

        //! gets string representation of error;
        std::string getError() const;

        //! returns if the vfile is accepted
        bool Accepted() const;

        //! returns if the current rule has error
        bool hasError() const;

        //! returns a Container copy of the RuleSequence
        inline Container getContainerElements() const
        { return rules; }

        //! same effect as set(const T& object)
        RuleBag& operator = (const T&);

        //! same affect as above but with an std::move
        RuleBag& operator = (const T&&);

        //! same effect as the copy constructor
        RuleBag& operator = (const RuleBag<T>& ) = delete;

        //! same effect as the move constructor
        RuleBag& operator = (RuleBag<T>&& );

        //! destructor
        ~RuleBag();

        //! creates a deep copy using RuleSequence::deepCopy() on each item
        static RuleBag<T, Result, Option> deepCopy(const RuleBag<T, Result, Option>& source);


    protected:

        //! runs through the rule sequence and returns a pointer to the matching rule
        rule_facade<T, Result>* runRules();


    private:

        //! the internal container used to store RuleSequence
        Container rules;

        //! The iterator used to transverse rule sequence
        ContainerIterator rulesIterator;

        //! A pointer to rule facade object
        rule_facade<T, Result>* result = nullptr;

        //! a copy of type
        T type;

        //! option holder
        Option _option;

        //! whether all rules did not yield true for T
        bool failed = false;

        //! holds the state of calling \var bool rule_facade<T, Result>::operator()
        bool accepted = false;

        //! if the rule has some error
        bool in_error = false;

        //! how many RuleSequence hit so far
        int rulesCounter;

        bool _deleteAtDestruction = false;
    };



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

    /*
    template<typename T, typename Result>
    RuleBag<T, Result>::RuleBag(const T& object)
    {
        type = object;
        resetIterator();
    }
    */

    template<typename T, typename Result, typename Option>
    RuleBag<T, Result, Option>::RuleBag(T&& object)
    {
        type = std::move(object);
        rules = std::move(object.rules);
        resetIterator();
    }

    template<typename T, typename Result, typename Option>
    void RuleBag<T, Result, Option>::AddRule(const RuleSequence<T, Result>& rule)
    {
        rules.push_back(rule);
        resetIterator();
        //ruleIterator = _rules.begin();
    }


    template<typename T, typename Result, typename Option>
    bool RuleBag<T, Result, Option>::hasError() const
    {
        return in_error;
    }


    template<typename T, typename Result, typename Option>
    rule_facade<T, Result>* RuleBag<T, Result, Option>::runRules()
    {
        using namespace std;
        //! This iterates through the list calling the operator ()..
        while(rulesIterator != rules.end())
        {
            //cout << "rule count: " << _rules.size() << endl;
            auto& ruleSeq = *rulesIterator;
            if(ruleSeq.NextIsAvailable())
            {
                rule_facade<T, Result>* rule = ruleSeq.NextRule();

                if( (*rule)(type) )
                {
                    _option = (*rule).getOption();
                    in_error = (*rule).inError();

                    accepted = true;
                    return rule;
                }
                ++rulesCounter;
            }
            ++rulesIterator; //https://class.coursera.org/neuralnets-2012-001/class/index for more information.
        }

        failed = true;

        if(rules.begin() == rules.end())
            return nullptr;
        else
            --rulesIterator;

        return (*rulesIterator).ThisRule();
    }


    template<typename T, typename Result, typename Option>
    void RuleBag<T, Result, Option>::set(const T& object)
    {
        type = object;
        resetIterator();
    }


    template<typename T, typename Result, typename Option>
    bool RuleBag<T, Result, Option>::failedAll() const
    {
        return failed;
    }


    template<typename T, typename Result, typename Option>
    void RuleBag<T, Result, Option>::clearRules()
    {
        rules.clear();
        rulesIterator = rules.begin();
    }


    template<typename T, typename Result, typename Option>
    void RuleBag<T, Result, Option>::resetIterator()
    {
        rulesIterator = rules.begin();

        for(auto& it : rules)
            it.ResetIterator();

        rulesCounter = 0;
        accepted = false;
        failed = false;
    }


    template<typename T, typename Result, typename Option>
    bool RuleBag<T, Result, Option>::nextRule()
    {
        result = runRules();

        if(failed)
            return false;

        return true;
    }


    template<typename T, typename Result, typename Option>
    bool RuleBag<T, Result, Option>::nextRule(Result& result)
    {
        result = runRules();

        if(failed)
            return false;

        return getVerdict(result);
    }


    template<typename T, typename Result, typename Option>
    RuleBag<T, Result, Option>& RuleBag<T, Result, Option>::operator = (const T& object)
    {
        type = object;
        resetIterator();
        return *this;
    }

    template<typename T, typename Result, typename Option>
    bool RuleBag<T, Result, Option>::getIndependentVerdict(const T& object, Result& resulT) const
    {
        rule_facade<T, Result, Option>* independent_Rules = result->Clone();
        independent_Rules->operator()(object);
        bool rtn = independent_Rules->getVerdict(resulT);
        delete independent_Rules;
        return rtn;
    }

    template<typename T, typename Result, typename Option>
    bool RuleBag<T, Result, Option>::getVerdict(Result& resulT) const
    {
        return result->getVerdict(resulT);
    }


    template<typename T, typename Result, typename Option>
    Result RuleBag<T, Result, Option>::getVerdict() const
    {
        return result->getVerdict();
    }


    template<typename T, typename Result, typename Option>
    bool RuleBag<T, Result, Option>::Accepted() const
    {
        return accepted;
    }


    template<typename T, typename Result, typename Option>
    RuleBag<T, Result, Option>& RuleBag<T, Result, Option>::operator = (const T&& object)
    {
        type = std::move(object);
        resetIterator();
        return *this;
    }


    template<typename T, typename Result, typename Option>
    Option RuleBag<T, Result, Option>::getOption() const
    {
        return _option;
    }


    template<typename T, typename Result, typename Option>
    std::string RuleBag<T, Result, Option>::getError() const
    {
        ConstContainerIterator it = rulesIterator;
        if(it == rules.end())
            return std::string("Foolish programming error");
        return (*it).ThisRule()->getError();
    }

    /* FOR SAFETY PURPOSES, WE SHOULDN"T USE THIS *
    template<typename T, typename Result>
    RuleBag<T, Result>& RuleBag<T, Result>::operator = (const RuleBag<T>& object)
    {
        type = object.type;
        rules = object.rules;
        resetIterator();
        return *this;
    }
    */


    template<typename T, typename Result, typename Option>
    RuleBag<T, Result, Option>& RuleBag<T, Result, Option>::operator = (RuleBag<T>&& object)
    {
        type = std::move(object.type);
        rules = std::move(object.rules);
        resetIterator();
        return *this;
    }


    template<typename T, typename Result, typename Option>
    bool RuleBag<T, Result, Option>::deleteAtDestruction(bool value)
    {
        deleteAtDestruction = value;
        return _deleteAtDestruction;
    }


    template<typename T, typename Result, typename Option>
    RuleBag<T, Result, Option>::~RuleBag()
    {
        if(_deleteAtDestruction)
        {
            for(auto& rC : rules)
            {
                auto myRules = rC.getContainerElements();
                for(auto& it : myRules)
                {
                    delete it;
                }
            }
        }
    }

    template<typename T, typename Result, typename Option>
    RuleBag<T, Result, Option> RuleBag<T, Result, Option>::deepCopy(const RuleBag<T, Result, Option>& source)
    {
        RuleBag<T, Result> rtn;

        Container ct = source.getRules();
        for(auto& it : ct)
            rtn.AddRule( RuleSequence<T, Result>::deepCopy(it) );

        return rtn;
    }



}   //end namespace

#endif // RULEBAG_H
