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
#include "backend/include/directory_elements/simple_directory_tree.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>

using namespace DLS::SimpleDirectoryTree;

/*
void DirectoryTree::addPath(const std::string path)
{
    std::vector<std::string> pathStructure;

    //! split the structure
    boost::split(path, boost::is_any_of("/"), pathStructure, boost::token_compress_on);

    for(auto& element: children)
    {
	if(element->getElementName() == )
    }
}
*/

/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
struct DirectoryElementCompare
{
    explicit DirectoryElementCompare(const DirectoryElement* element)
    { path = element->getElementName(); }

    explicit DirectoryElementCompare(const std::string& name)
    { path = name; }

    bool operator () (const DirectoryElement* rhs)
    { return path == rhs->getElementName(); }

    std::string path;
};

struct DirectoryElementFullPathCompare
{
    explicit DirectoryElementFullPathCompare(const DirectoryElement* element)
    { path = element->getFullPath(); }

    explicit DirectoryElementFullPathCompare(const std::string& fullPath)
    { path = fullPath; }

    bool operator () (const DirectoryElement* rhs)
    { return path == rhs->getFullPath(); }

    std::string path;
};




/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


DirectoryElement::DirectoryElement(const std::string& Name, const DirectoryElement* Parent)
    :	parent(Parent)
{
    elementName = Name;
}


//////////////////////////////////////////////////
///		 Search functions		///
/////////////////////////////////////////////////
const DirectoryElement* DirectoryElement::find(const DirectoryElement* element, const Container& elements)
{
    if(!element)
        return nullptr;

    const constContainerIterator iter = std::find_if(elements.begin(), elements.end(), DirectoryElementFullPathCompare(element));
    return (iter == elements.end()) ? nullptr : *iter;
}


const DirectoryElement* DirectoryElement::find_by_full_path(const std::string& fullPath, const Container& elements)
{
    const constContainerIterator iter = std::find_if(elements.begin(), elements.end(), DirectoryElementFullPathCompare(fullPath));
    return (iter == elements.end()) ? nullptr : *iter;
}


const DirectoryElement* DirectoryElement::find_by_name(const std::string& name, const Container& elements)
{
    const constContainerIterator iter = std::find_if(elements.begin(), elements.end(), DirectoryElementCompare(name));
    return (iter == elements.end()) ? nullptr : *iter;
}


DirectoryElement* DirectoryElement::deepCopy(const DirectoryElement* source, DirectoryElement* Parent)
{
    if(!Parent)
        return DirectoryElement::createDeepCopy(source, Parent);

    Parent->addChild( createDeepCopy(source, Parent) );
    return Parent;
}


DirectoryElement* DirectoryElement::createDeepCopy(const DirectoryElement* source, const DirectoryElement* Parent)
{
    if(!source)
        return nullptr;
    DirectoryElement* rtn = new DirectoryElement(source->getElementName(), Parent );
    // add a parent
    if(Parent)
        if(Parent->property())
            rtn->property( source->property()->Clone(Parent) );

    std::vector<DirectoryElement*> Children = source->getChildren();

    for(auto& child : Children)
    {
        rtn->addChild( createDeepCopy(child, rtn) );
    }

    return rtn;
}

/*
void DirectoryElement::merge(DirectoryElement* const toBeMerged, DirectoryElement* const reciever, bool denyDuplicates, bool interWeave)
{
    toBeMerged->addChild( reciever, denyDuplicates, false, false );

    if(!interWeave)
    {
        reciever->parent = toBeMerged;

    }

    auto& t_children = toBeMerged->getChildren();
    auto& r_children = reciever->getChildren();

    for(auto& iter : t_children)
    {
        DirectoryElement* e = DirectoryElement::find_by_name(iter->getElementName(), r_children);
        if(e)
            DirectoryElement::merge(iter, static_cast<DirectoryElement*>(e), denyDuplicates);
    }

}
*/


DirectoryElement::~DirectoryElement()
{
    for(auto& child : children)
        delete child;

    if(baseProperty)
        delete baseProperty;
}


DirectoryElement* DirectoryElement::addChild(DirectoryElement* child, bool denyDuplicates, bool deleteChildDuplicate, bool deleteNewChild)
{
    if(denyDuplicates)		//! if we do not want duplicates, then check if that element exists as a child before
    {
        std::vector<DirectoryElement*>::iterator iter;

        //! TODO : change this to \DirectoryElementCompare and test performance results with \DirectoryElementFullPathCompare
        iter = std::find_if(children.begin(), children.end(), DirectoryElementFullPathCompare(child) );

        if(iter == children.end())	//! Then its not found
        {
            children.push_back(child);
            return child;		//! return the inserted child
        }

        else				//! pointer safety measures
        {
            if(deleteChildDuplicate)
            {
            if(deleteNewChild)
            {
                delete child;	//! delete the \a child parameter
                return *iter;	//! return the found child
            }
            else
            {
		    //magic here
                delete (*iter);		//! delete previous child
                children.erase(iter);	//! remove from container
                children.push_back(child); //! push in new child
            }
            }
        }

        return child;			//! return the DirectoryElement that was there before
    }

    //! obviously at this point, we do not want to delete anything, we just want to add
    children.push_back(child);
    return child;
}


std::string DirectoryElement::getFullPath(bool includeTopMostParent) const
{
    std::string rtn = elementName;	//! return value

    const DirectoryElement* Parent = parent;	//! get the parent of this node

    if(Parent)				//! if parent exists
    {
        rtn = Parent->getFullPath( includeTopMostParent ) + "/" + rtn;
    }

    else				//! else we are at topmost parent
        if( !includeTopMostParent )	//! if we shouldn't include topmost parent
            return std::string();	//! then return an empty string instead

    return rtn;
}


std::string DirectoryElement::getElementName() const
{
    return elementName;
}


bool DirectoryElement::isDirectory() const
{
    return boost::filesystem::is_directory( getFullPath() );
}


bool DirectoryElement::isFile() const
{
    return boost::filesystem::is_regular_file( getFullPath() );
}


std::vector<DirectoryElement*> DirectoryElement::getChildren() const
{
    return children;
}

const IDirectoryElementProperty* DirectoryElement::property() const
{
    return baseProperty;
}


void DirectoryElement::property(const IDirectoryElementProperty* Property)
{
    baseProperty = Property;
}

/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


DirectoryElementHelper::DirectoryElementHelper(bool destryWithMe)
{
    destroy = destryWithMe;
    rootElement = new DirectoryElement("__DLS<<PARENT>>DLS__");
    //std::cout << "Helper root at " << rootElement << std::endl;
}


DirectoryElementHelper::DirectoryElementHelper(DirectoryElement* element, bool createDeepCopy, bool destryWithMe)
{
    destroy = destryWithMe;
    if(createDeepCopy)
        rootElement = DirectoryElement::deepCopy(element, new DirectoryElement("__DLS<<PARENT>>DLS__"));
    else
        rootElement = element;
    //std::cout << "Helper root at " << rootElement << std::endl;
}



const DirectoryElement* DirectoryElementHelper::addPath(const std::string& Path)
{
    std::vector<std::string> pathStructure;

    //! Trim
    std::string PPath = boost::trim_copy_if(Path, boost::is_any_of("/"));

    //! split the structure
    boost::split(pathStructure, PPath, boost::is_any_of("/"), boost::token_compress_on);

    //! Temporary holder
    DirectoryElement* element = rootElement;

    //! Create the tree;
    DirectoryElement* elem = nullptr;
    for(unsigned i=0; i < pathStructure.size(); i++)
    {
        elem = new DirectoryElement( pathStructure[i], element );
        element = element->addChild( elem );
        //std::cout << "Added : " << pathStructure[i] << std::endl;
    }

    return elem;
}


DirectoryElement* DirectoryElementHelper::getDirectoryElement() const
{
    return rootElement;
}


DirectoryElementHelper::~DirectoryElementHelper()
{
    if(destroy)
        delete rootElement;
}
