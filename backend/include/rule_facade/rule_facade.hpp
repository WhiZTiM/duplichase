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
#ifndef RULE_FACADE_H
#define RULE_FACADE_H
#include <string>
#include <algorithm>
#include <list>
#include <vector>
#include <iostream>
#include <type_traits>

    /*! \file rule_facade.h
    *  \author WhiZTiM, 2013
    *  \brief This contains the class where all rules inherit from... its just an interface class
    *
    *  \b Motivation
    *  Certain prioritized Rules are needed to make verdicts on objects...
    *  This, in such a manner, that if, the object doesnt fit into one rule, we try the next, next... until it does
    *  If a matching rule is found, we check, if the object satisfies that rule, else, we descend the ladder
    *  To make rules generic in a system is a bit difficult or too localized
    *  This \class DLS::rule_facade attempts to alleviate this by it being a pre-defined contract to a set of rules which is to be used by a rule
    */

    /*! \namespace DLS
    *  \brief scoping....
    */
namespace DLS
{

    enum class rule_option
    {
        Accept = 0x01,
        Reject = 0x02,
        Dismiss= 0x04
    };


    //! Global operator for bitmasking
    inline rule_option operator | (rule_option a, rule_option b)
    {
        typedef std::underlying_type<rule_option>::type tp;
        return static_cast<rule_option>( static_cast<tp>(a) | static_cast<tp>(b) );
    }


    /*! \class rule_facade<T, Result>
    *  \brief An interface class that is used to create rules and process rules
    *
    */
    template<typename T, typename Result = T, typename RuleOption = rule_option>
    class rule_facade
    {
    public:
        enum class Option { AND=0x01, OR=0x02, XOR=0x04, NOT=0x08 };
        rule_facade()
        {
            //
        }

        /*! \fn virtual bool operator () (const T& ) = 0;
         *  \brief the operator called by a rule management interface, in this case, \class DLS::RuleBag<T, Result>
         *  \b Details \par
         *  This operator MUST be implemented by the contracting rule class. It takes an argument of type \a T and
         *  runs the predefined set of rule on the object
         *  it returns \var true if the object fits into this rule....
         *  \note PRECONDITION: \a T must be valid and instantiated
         *  \n POSTCONDITION: the objects conformance is stored
         */
        virtual bool operator () (const T& ) = 0;


        /*! \fn virtual bool operator () () = 0;
         *  \brief checks if the previously checked object mstches the defined rule
         *  \note \b NOT TO BE CONFUSED WITH the argumented version
         *  \par \b Details
         *  This
         *
         */
        virtual RuleOption getOption () const = 0;

        //! something very stupid
        virtual std::string className() const = 0;

        //! Derieved classes should implement co-variant return types if supported by compiler
        //! else return a pointer to rule_facade
        virtual rule_facade<T, Result>* Clone() const = 0;

        //! should return an object of Result as verdict for the rule
        virtual Result getVerdict() const = 0;

        //! should be used to modify the \param Result object and return \a true if modified
        virtual bool getVerdict(Result&) const = 0;

        //! should return true if there was an error in parsing the rule
        virtual bool inError() const = 0;

        //! should be used to return a string representation of the error
        virtual std::string getError() const = 0;

        //! derived classes gets deleted first
        virtual ~rule_facade() {}

    protected:

    };

}
#endif // RULE_FACADE_H
