#include "rule_factory/rule_bag.h"
using namespace DLS;
/*
template<typename T, typename Result>
RuleSequence<T, Result>::RuleSequence(bool Destroy)
{
    _destruct = Destroy;
    _iter = _rules.begin();
}


template<typename T, typename Result>
void RuleSequence<T, Result>::AddRule(const rule_facade<T, Result>* ptrRule)
{
    if(ptrRule != nullptr)
    _rules.push_back(ptrRule);
}


template<typename T, typename Result>
const rule_facade<T, Result>* RuleSequence<T, Result>::NextRule() const
{
    if(_iter == _rules.end()) return nullptr;
    return _iter++;
}


template<typename T, typename Result>
const rule_facade<T, Result>* RuleSequence<T, Result>::ThisRule() const
{
    return *_iter;
}


template<typename T, typename Result>
bool RuleSequence<T, Result>::NextIsAvailable() const
{
    return _iter != _rules.end();
}

template<typename T, typename Result>
bool RuleSequence<T, Result>::Empty() const
{
    return _rules.empty();
}

template<typename T, typename Result>
void RuleSequence<T, Result>::Clear()
{
    _rules.clear();
}

template<typename T, typename Result>
void RuleSequence<T, Result>::ResetIterator() const
{
    _iter = _rules.begin();
}

template<typename T, typename Result>
RuleSequence<T, Result>::~RuleSequence()
{
    if(_destruct)
    for(auto& it = _rules.begin() : _rules.end())
    {
        if(*it != nullptr)
        delete *it;
    }
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////




template<typename T, typename Result>
RuleBag<T, Result>::RuleBag(const T& object)
{
    _type = object;
    resetIterator();
}

template<typename T, typename Result>
RuleBag<T, Result>::RuleBag(T&& object)
{
    _type = std::move(object);
    _rules = std::move(object._rules);
    resetIterator();
}

template<typename T, typename Result>
void RuleBag<T, Result>::AddRule(const RuleSequence<T, Result>& rule)
{
    _rules.push_back(rule);
}

template<typename T, typename Result>
const rule_facade<T, Result>* RuleBag<T, Result>::runRules()
{
    //! This iterates through the list calling the operator ()..
    while(_rule_iterator != _rules.end())
    {
        auto& ruleSeq = *_rule_iterator;
        while(ruleSeq.NextIsAvailable())
        {
            auto& rule = *ruleSeq.NextRule();
            if( rule(_type) )
            {
                if( rule() )
                    return &rule;
            }
            ++_count;
        }
        ++_rule_iterator;
    }
    _failed = true;
    return nullptr;
}

template<typename T, typename Result>
bool RuleBag<T, Result>::failedAll() const
{
    return _failed;
}

template<typename T, typename Result>
void RuleBag<T, Result>::clearRules()
{
    _rules.clear();
    _rule_iterator = _rules.begin();
}

template<typename T, typename Result>
void RuleBag<T, Result>::resetIterator()
{
    _r_it _rule_iterator = _rules.begin();
    for(auto& it = _rules.begin() : _rules.end()) (*it).ResetIterator();
    _count = 0;
}

template<typename T, typename Result>
bool RuleBag<T, Result>::nextRule()
{
    if(_failed)
        return false;
    _result = runRules();
    return true;
}

template<typename T, typename Result>
bool RuleBag<T, Result>::nextRule(Result& result)
{
    if(_failed)
        return false;
    _result = runRules();
    return getVerdict(result);
}

template<typename T, typename Result>
RuleBag<T, Result>& RuleBag<T, Result>::operator = (const T& object)
{
    _type = object;
    resetIterator();
    return *this;
}

template<typename T, typename Result>
bool RuleBag<T, Result>::getVerdict(Result& result) const
{
    return (*_result).ThisRule()->getVerdict(result);
}

template<typename T, typename Result>
Result RuleBag<T, Result>::getVerdict() const
{
    return (*_result).ThisRule()->getVerdict();
}

template<typename T, typename Result>
RuleBag<T, Result>& RuleBag<T, Result>::operator = (const T&& object)
{
    _type = std::move(object);
    resetIterator();
    return *this;
}

template<typename T, typename Result>
RuleBag<T, Result>& RuleBag<T, Result>::operator = (const RuleBag<T>& object)
{
    _type = object._type;
    _rules = object._rules;
    resetIterator();
    return *this;
}

template<typename T, typename Result>
RuleBag<T, Result>& RuleBag<T, Result>::operator = (RuleBag<T>&& object)
{
    _type = std::move(object._type);
    _rules = std::move(object._rules);
    resetIterator();
    return *this;
}

template<typename T, typename Result>
RuleBag<T, Result>::~RuleBag()
{
    //dtor
}

*/
