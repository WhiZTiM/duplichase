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
#include "backend/include/directory_elements/directory_iterator.hpp"
#include <iostream>

using namespace DLS;


ExclusionHandler::ExclusionHandler(const Tree::DirectoryElement* Element, bool createDeepCopy)
    :	existingWithinThisLevel(false)
{
    _usingDeepCopy = createDeepCopy;
    if(_usingDeepCopy)
        root = Tree::DirectoryElement::deepCopy(Element);
    else
        root = Element;

    if(root && !root->hasChildren()) //most likely empty
        root = nullptr;
    element = root;
    checked = false;
}

ExclusionHandler::ExclusionHandler(const ExclusionHandler& handler)
{
    //! Faith is the conviction of things not known, the evidence of things unseen
    _usingDeepCopy = handler._usingDeepCopy;
    if(_usingDeepCopy)
        root = Tree::DirectoryElement::deepCopy(handler.root);
    else
        root = handler.root;
    if(root && !root->hasChildren()) //most likely empty
        root = nullptr;
    element = root;
}

ExclusionHandler::ExclusionHandler(const ExclusionHandler& handler, bool useDeepCopy)
{
    //! Faith is the conviction of things not known, the evidence of things unseen
    _usingDeepCopy = useDeepCopy;
    if(_usingDeepCopy)
        root = Tree::DirectoryElement::deepCopy(handler.root);
    else
        root = handler.root;
    if(root && !root->hasChildren()) //most likely empty
        root = nullptr;
    element = root;
}


ExclusionHandler::ExclusionHandler(ExclusionHandler&& handler)
{
    //! Faith is the conviction of things not known, the evidence of things unseen
    _usingDeepCopy = handler._usingDeepCopy;
    if(_usingDeepCopy)
    {
        root = Tree::DirectoryElement::deepCopy(handler.root);
        delete handler.root;
    }
    else
        root = handler.root;
    if(root && !root->hasChildren()) //most likely empty
        root = nullptr;
    element = root;
}


ExclusionHandler& ExclusionHandler::operator= (const ExclusionHandler& handler)
{
    if(this == &handler)
        return *this;
    //! Faith is the conviction of things not known, the evidence of things unseen
    _usingDeepCopy = handler._usingDeepCopy;
    if(_usingDeepCopy)
        root = Tree::DirectoryElement::deepCopy(handler.root);

    else
        root = handler.root;
    if(root && !root->hasChildren()) //most likely empty
        root = nullptr;
    element = root;
    return *this;
}


ExclusionHandler& ExclusionHandler::operator= (ExclusionHandler&& handler)
{
    if(this == &handler)
        return *this;
    //! Faith is the conviction of things not known, the evidence of things unseen
    _usingDeepCopy = handler._usingDeepCopy;
    if(_usingDeepCopy)
    {
        root = Tree::DirectoryElement::deepCopy(handler.root);
        delete handler.root;
    }
    else
        root = handler.root;
    if(!root->hasChildren()) //most likely empty
        root = nullptr;
    element = root;
    return *this;
}


//! by using another level of indirection
bool ExclusionHandler::existWithinThis(const std::string& path, bool includeGrandRoot) const
{
    if(!root)
        return false;

    checked = false;	//we need this so that we do not use previous values
    element = root;	//assign --->useless comment hehehe

    // decomposepath
    std::vector<std::string> vecName = decomposePathToVector(path);
    unsigned int iter = 0;	// start at 0

    if(vecName.empty())				// empty or false path
        return false;

    if(includeGrandRoot && element->hasChildren())
        element = element->getChildren().back();


    //self explanatory
    while(iter < vecName.size() && element->hasChildren())
    {
        //for(auto& it : element->getChildren())
        //    std::cout << "\nElements::: " << it->getFullPath() << std::endl;

        //std::cout << "Element Before: " << element->getFullPath(true) << "\nName" << element->getElementName() << "\n";
        //std::cout << "Searching for \"" << vecName[iter] << "\" in \"" << element->getElementName() << "\"\n";
        element = Tree::DirectoryElement::find_by_name( vecName[iter], element->getChildren() );

        if(!element)
            return false;
        //std::cout << "Element After: " << element->getFullPath(true) << "\nName" << element->getElementName() << "\n";

        ++iter;
    }

    checked = true;	//so we escaped that onslaught

    if(element)
        return true;
    return false;
}


bool ExclusionHandler::exactlyThis() const
{
    if(!element)
        return false;
    if(!checked)
        return false;
    return !element->hasChildren();
}


bool ExclusionHandler::usingDeepCopy() const
{
    return _usingDeepCopy;
}


ExclusionHandler::~ExclusionHandler()
{
    if(_usingDeepCopy)
        delete root;
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

RecursiveDirectoryIterator::RecursiveDirectoryIterator(const fs::path& StartPath, const ExclusionHandler& handle)
    : startPath(StartPath), exclusionHandle(handle), errorCode(boost::system::error_code()), iterationEnded(false)
{
    //startPath = StartPath;

    //exclusionHandle.setStartPath(const std::string& path);
    _level = 0;
    _prev_level = 0;
    _first_call = true;
    checkForExclusion = true;

    increment();
}


void RecursiveDirectoryIterator::setMaximumRetries(unsigned int retries)
{
    maximumRetries = retries;
}


fs::directory_entry RecursiveDirectoryIterator::getDirectoryEntry() const
{

    if(!iterationEnded)
    {
        return *iteratorStack.top();
    }

    //! else we return a new and empty directory_entry
    return fs::directory_entry();
}


boost::system::error_code RecursiveDirectoryIterator::getErrorCode() const
{
    return errorCode;
}


//! This is where everything is
void RecursiveDirectoryIterator::increment()
{
    //! check if iteration has ended
    //if(iterationEnded)
    //{
    //	return;
    //}

    //! gymnastics here... we check if there is no error found
    if(errorCode == noError)
    {
        retryCount = 0;
        fs::directory_entry entry = nextDirectoryEnt();

        if(iterationEnded)
            return;

        if(checkForExclusion)
        {
            std::string str = entry.path().generic_string();

            if(exclusionHandle.existWithinThis(entry.path().generic_string()) )
            {
                if(exclusionHandle.exactlyThis())
                {
                    iteratorStack.pop();
                    if(iteratorStack.empty())
                    {
                        iterationEnded = true;
                        return;
                    }

                    _skip_directory_adding = true;
                    forrardStackIterator();
                    /*
                    iteratorStack.top().increment(errorCode);

                    if(iteratorStack.size() == 1 and iteratorStack.top() == EndIterator)
                        iterationEnded = true;
                        */
                }
            }
            else
            {
                checkForExclusion = false;
                _prev_level = _level;
            }

        }

        else
        {
            //! stack poped
            if(_level <= _prev_level)
            {
                checkForExclusion = true;
            }
        }
        //if(!iterationEnded)

    }


    //! there is error, lets skip the file and try another
    ////! TODO: Implement a form of minimum retries
    //! \DONE		<< Friday, 2nd August, 2013 -->by WhiZTiM
    else
    {
        nextDirectoryEnt();		// see if we can try the next file
        //! if retryCount is maximum, pop out of the current directory and continue with parent directory
        if(++retryCount >= maximumRetries)
            iteratorStack.pop();
    }

}


int RecursiveDirectoryIterator::levelChanged() const
{
    //int newLevel = iterator.level();
    //int rtn = (_level == newLevel) ? 0 : ( (_level > newLevel) ? 1 : (-1) );
    //_level = newLevel;
    //return rtn;
    return 0;
}


fs::directory_entry RecursiveDirectoryIterator::nextDirectoryEnt()
{
    if(_first_call)
    {
        if(!fs::is_directory(fs::path(startPath)))
        {
            iterationEnded = true;
            _first_call = false;
            return fs::directory_entry();
        }
        iteratorStack.push(fs::directory_iterator(startPath));
        _first_call = false;
        return *iteratorStack.top();
    }
    if(forrardStackIterator() && !iteratorStack.empty())
    {
        return *iteratorStack.top();
    }

    iterationEnded = true;
    return fs::directory_entry();

}


bool RecursiveDirectoryIterator::forrardStackIterator()
{
    if(iteratorStack.empty())
        return false;

    fs::directory_iterator& top = iteratorStack.top();


    if(top == EndIterator)
    {
        iteratorStack.pop();
        if(iteratorStack.empty())
            return false;
        return pop();
    }

    const fs::path path = top->path();
    if(!_skip_directory_adding and is_directory( path , errorCode) and !is_symlink( path, errorCode))
    {
        boost::system::error_code ec;
        fs::directory_iterator it(path, ec);
        if(!ec and it != EndIterator)
        {
            iteratorStack.push(it);
            ++_level;
            return true;
        }
    }
    _skip_directory_adding = false;

    //! unwind the stack if end reached
    return pop();
}

bool RecursiveDirectoryIterator::pop()
{
    bool rtn = false;
    //! stack unwinding
    while(!iteratorStack.empty())
    {
        if(iteratorStack.top() == EndIterator)
        {
            rtn = true;
            iteratorStack.pop();
            --_level;
        }
        else
        {
            iteratorStack.top().increment(errorCode);

            if(errorCode)
            {
                std::cout << "BS_ Error" << errorCode.message() << std::endl;
            }

            if(iteratorStack.top() != EndIterator)
            {
                //std::cout << (*iteratorStack.top()).path().generic_string() << std::endl;
                return true;
            }
        }
    }

    return rtn;
}

//! Pre-increament operator
RecursiveDirectoryIterator& RecursiveDirectoryIterator::operator ++ ()
{
    increment();
    return *this;
}


//! post increament operator by value
RecursiveDirectoryIterator RecursiveDirectoryIterator::operator ++ (int m)
{
    RecursiveDirectoryIterator rtn(*this);

    increment();

    return rtn;
}

RecursiveDirectoryIterator& RecursiveDirectoryIterator::operator * ()
{
    //Nothing ---> Have fun! Did you know that Jesus Loves You (^_^)
    return *this;
}
