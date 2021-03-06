/*******************************************************************************************
**  (C) Copyright September 2013 - November 2013 by
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
#include "backend/include/recommendation/path_recommender.hpp"
#include <boost/algorithm/string.hpp>

using namespace DLS;
using namespace Tree;

PathRecommender::PathRecommender()
{
    //ctor
}

PathRecommender::PathRecommender(const std::vector<std::string>& vecString)
{
    for(auto& iter : vecString)
        deAgent.addPath(iter);
}


bool PathRecommender::accentuate(const std::string& giver, const std::string& reciever)
{
    return ( deAgent.raiseIncoming(giver) && deAgent.raiseOutgoing(reciever) );
}

bool PathRecommender::setWeights(const std::string& ToBeDeleted, const std::string& ToBeKept)
{
    return ( accentuate(ToBeDeleted, ToBeKept) );
}

bool PathRecommender::unsetWeights(const std::string& ToBeDeleted, const std::string& ToBeKept)
{
    return ( deAgent.lowerIncoming(ToBeDeleted) && deAgent.lowerOutgoing(ToBeKept) );
}

void PathRecommender::addPath(const std::string& path)
{
    deAgent.addPath(path);
}

long long PathRecommender::getIncomingIndex(const std::string& path) const
{
    return deAgent.getIncoming(path);
}

long long PathRecommender::getOutgoingIndex(const std::string& path) const
{
    return deAgent.getOutgoing(path);
}

void PathRecommender::resetAllToZero()
{
    deAgent.resetToZero();
}

void PathRecommender::clear()
{
    deAgent.clear();
}

PathRecommender::~PathRecommender()
{
    //dtor
}



///////////////////////////////////////////////////
/////////////////////////////////////////////////////
//////////////////////////////////////////////

SimpleDEEdgeAgent::SimpleDEEdgeAgent()
{
    rootElement = new DirectoryElement("__DLS<<PARENT>>DLS__");
    rootElement->property(new DEEdgeProperty());
}

SimpleDEEdgeAgent::~SimpleDEEdgeAgent()
{
    delete rootElement;
}

void SimpleDEEdgeAgent::clear()
{
    mStringElem.clear();
    delete rootElement;
    rootElement = new DirectoryElement("__DLS<<PARENT>>DLS__");
    rootElement->property(new DEEdgeProperty());
}

void SimpleDEEdgeAgent::resetToZero()
{
    for(auto const& paths : mStringElem)
    {
        const DirectoryElement* element = paths.second;
        if(!element)
            continue;
        while(element->getParent())
        {
            const DEEdgeProperty* ptr = cast( element->property() );
            if(!ptr)
                break;
            if(ptr->outgoing() == 0 && ptr->incoming() == 0)
                break;
            ptr->setIncoming(0);
            ptr->setOutgoing(0);
            element = element->getParent();
        }
    }
}

void SimpleDEEdgeAgent::addPath(const std::string& path)
{
    std::vector<std::string> pathStructure;
    std::string PPath = boost::trim_copy_if(path, boost::is_any_of("/"));
    boost::split(pathStructure, PPath, boost::is_any_of("/"), boost::token_compress_on);

    DirectoryElement* element = rootElement;
    DirectoryElement* elem = nullptr;
    for(unsigned i=0; i < pathStructure.size(); i++)
    {
        elem = new DirectoryElement( pathStructure[i], element );
        if(!elem->property())
            elem->property(new DEEdgeProperty());
        element = element->addChild( elem );
    }

    mStringElem.insert( std::make_pair(path, elem) );
}


bool SimpleDEEdgeAgent::lastgetSuccessful() const
{
    return _success;
}


long long SimpleDEEdgeAgent::getOutgoing(const std::string& path) const
{
    long long rtn = 0;
    const DirectoryElement* element = find(path);
    if(!element)
    {
        _success = false;
        return rtn;
    }
    while(element->getParent())
    {
        const DEEdgeProperty* ptr = cast( element->property() );
        if(!ptr)
        {
            _success = false;
            return rtn;
        }
        rtn += ptr->outgoing();
        element = element->getParent();
    }
    _success = true;
    return rtn;
}


long long SimpleDEEdgeAgent::getIncoming(const std::string& path) const
{
    long long rtn = 0;
    const DirectoryElement* element = find(path);
    if(!element)
    {
        _success = false;
        return rtn;
    }
    while(element->getParent())
    {
        const DEEdgeProperty* ptr = cast( element->property() );
        if(!ptr)
        {
            _success = false;
            return rtn;
        }
        rtn += ptr->incoming();
        element = element->getParent();
    }
    _success = true;
    return rtn;
}


const DirectoryElement* SimpleDEEdgeAgent::find(const std::string& path) const
{
    auto iter = mStringElem.find(path);
    if(iter == mStringElem.end())
        return nullptr;

    return iter->second;
}


bool SimpleDEEdgeAgent::raiseOutgoing(const std::string& path)
{
    const DirectoryElement* element = find(path);
    while(element && element->getParent())
    {
        const DEEdgeProperty* ptr = cast( element->property() );
        if(!ptr)
            return false;
        ptr->raiseOutgoing();
        element = element->getParent();
    }
    return true;
}



bool SimpleDEEdgeAgent::raiseIncoming(const std::string& path)
{
    const DirectoryElement* element = find(path);
    while(element && element->getParent())
    {
        const DEEdgeProperty* ptr = cast( element->property() );
        if(!ptr)
            return false;
        ptr->raiseIncoming();
        element = element->getParent();
    }
    return true;
}


bool SimpleDEEdgeAgent::lowerOutgoing(const std::string& path)
{
    const DirectoryElement* element = find(path);
    while(element && element->getParent())
    {
        const DEEdgeProperty* ptr = cast( element->property() );
        if(!ptr)
            return false;
        ptr->lowerOutgoing();
        element = element->getParent();
    }
    return true;
}


bool SimpleDEEdgeAgent::lowerIncoming(const std::string& path)
{
    const DirectoryElement* element = find(path);
    while(element && element->getParent())
    {
        const DEEdgeProperty* ptr = cast( element->property() );
        if(!ptr)
            return false;
        ptr->lowerIncoming();
        element = element->getParent();
    }
    return true;
}

///////////////////////////////////////////////////
/////////////////////////////////////////////////////
//////////////////////////////////////////////

const DEEdgeProperty* DEEdgeProperty::Clone(const DirectoryElement* parent) const
{
    (void)parent;
    return new DEEdgeProperty();
}

void DEEdgeProperty::incoming(long long value)
{
    _incoming = value;
}

void DEEdgeProperty::outgoing(long long value)
{
    _outgoing = value;
}

long long DEEdgeProperty::incoming() const
{
    return _incoming;
}

long long DEEdgeProperty::outgoing() const
{
    return _outgoing;
}

void DEEdgeProperty::raiseIncoming() const
{
    ++_incoming;
}

void DEEdgeProperty::raiseOutgoing() const
{
    ++_outgoing;
}

void DEEdgeProperty::lowerIncoming() const
{
    if(_incoming > 0)
        --_incoming;
}

void DEEdgeProperty::lowerOutgoing() const
{
    if(_outgoing > 0)
        --_outgoing;
}

void DEEdgeProperty::setOutgoing(long long value) const
{
    _outgoing = value < 0 ? 0 : value;
}

void DEEdgeProperty::setIncoming(long long value) const
{
    _incoming = value < 0 ? 0 : value;
}

void DEEdgeProperty::used(bool value) const
{
    _used = value;
}

bool DEEdgeProperty::used() const
{
    return _used;
}
