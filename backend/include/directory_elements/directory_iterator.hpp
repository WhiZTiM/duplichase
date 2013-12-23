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
#ifndef DIRECTORY_ITERATOR_HXX
#define DIRECTORY_ITERATOR_HXX

#include <iterator>
#include <vector>
#include <string>
#include <stack>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "simple_directory_tree.hpp"

namespace DLS
{

    namespace fs = boost::filesystem;
    namespace Tree = SimpleDirectoryTree;



    //! TODO: overload the Constructor and add new members to this class to support
    //! the use of smart Pointers
    class ExclusionHandler
    {
    public:
        //! \a Element signifies an absolute
        //! This class stores a deep copy of \a Element, thus it is safe to delete \a Element
        explicit ExclusionHandler(const Tree::DirectoryElement* Element = nullptr, bool createDeepCopy = true);

        //! Copy Constructor..
        ExclusionHandler(const ExclusionHandler& handler);

        //! Copy Constructor ..not really
        ExclusionHandler(const ExclusionHandler& handler, bool useDeepCopy);

        //! Move Constructor
        ExclusionHandler(ExclusionHandler&& handler);

        //! Copy Assignment Operator
        ExclusionHandler& operator = (const ExclusionHandler& handler);

        //! Move Assignment Operator
        ExclusionHandler& operator = (ExclusionHandler&& handler);

        //! test if a path exists in exclusion
        bool existWithinThis(const std::string& path, bool includeGrandRoot = false) const;

        //! returns true if the last tested path is exact... i.e has no children
        bool exactlyThis() const;

        //! returns whether this object is using a deep Copy of elements
        bool usingDeepCopy() const;

        //! obviously
        ~ExclusionHandler();

    private:
        //! pointer to the current element
        mutable const Tree::DirectoryElement* element;

        //! pointer to the constructed root element
        const Tree::DirectoryElement* root;

        //! whether the last
        bool existingWithinThisLevel;

        //! for destructor
        bool _usingDeepCopy;

        //! just a minor hack
        mutable bool checked;
	//int currentTreeDepth;
    };

    //////////////////////////////////////////////////////
    //////////////////////////////////////////////////////
    //////////////////////////////////////////////////////

    /*! \class RecursiveDirectoryIterator
     * 	This class safely iterates over a given directory specified by a boost::filesystem::path object
     * */

    class RecursiveDirectoryIterator : public std::iterator<std::forward_iterator_tag, RecursiveDirectoryIterator>
    {
    public:
        //! \a StartPath defines where the folder to recursively iterate over
        //! \a handle defines the exclusion handler object
        explicit RecursiveDirectoryIterator(const fs::path& StartPath, const ExclusionHandler& handle = ExclusionHandler());

        //! returns \a true if iteration has ended
        inline bool isEnded () const
        { return iterationEnded; }

        //! returns if there hasn't been any error
        inline bool inError() const
        { return (errorCode != noError); }

        //! returns the current iteration object in form of boost::filesystem::directory_entry
        fs::directory_entry getDirectoryEntry() const;

        //! gets the current boost::filesystem::system::error_code
        boost::system::error_code getErrorCode() const;

        //! pops the iterator from the current directory
        bool pop();

        //! \return \a 1 if the iterator has moved into a folder
        //! \return \a -1 if out of a folder
        //! otherwise returns \a 0
        int levelChanged() const;

        //! setsMaximum number of retries
        void setMaximumRetries(unsigned int retries);

        //! iterator functions
        RecursiveDirectoryIterator& operator ++ ();
        RecursiveDirectoryIterator& operator *  ();
        RecursiveDirectoryIterator  operator ++ (int);		// postfix operator is by VALUE not Reference
    private:

        //! holds the starting path given at the constructor...
        //! This is later reused to determine the last path before an error was reachde
        fs::path startPath;

        //! ExclusionHandle for excluding paths
        ExclusionHandler exclusionHandle;

        //! boost::filesystem::recursive_directory_iterator object
        //fs::recursive_directory_iterator iterator;

        //! end!! Hehehe ->>>> Ouchh my eyes hurt... 1st August, 2013
        //fs::recursive_directory_iterator endIterator;

        //! errorcode ..just to prevent exception
        boost::system::error_code errorCode, noError;

        //! stack used to simulate recursion
        std::stack<fs::directory_iterator> iteratorStack;

        //! end iterator
        const fs::directory_iterator EndIterator;

        //! holds whether the iteration end has been reached
        bool iterationEnded;

        //! should we check for exclusion?
        bool checkForExclusion;

        //! just a hack
        bool _first_call;

        //! to detect exclusion checking
        bool level_crossing;

        //! holds the maximum number of retries to files in a directory before giving up
        unsigned int maximumRetries = 10;

        //! retry keeper
        unsigned int retryCount = 0;

        //! assists the function \fn int levelChanged() const;
        int _level;

        //! just a place holder for previous values
        int _prev_level;

        bool _never_incremented;

        bool _skip_directory_adding = false;

        //! The function that does the actual inreament
        void increment();

        //! advances by one level of recursion
        bool forrardStackIterator();

        //! returns a reference to the next directory_entry
        fs::directory_entry nextDirectoryEnt();

    };

    //! a free function that splits an std::string path on sighting "/", starting Left to Right
    //! (a.k.a generates a directory separated array. as:::implemented below)
    inline std::vector<std::string> decomposePathToVector(const std::string& param)
    {
        std::vector<std::string> rtn;

        //! Trim
        std::string val = boost::trim_copy_if(param, boost::is_any_of("/"));

        //! split the structure
        return boost::split(rtn, val, boost::is_any_of("/"), boost::token_compress_on);
    }

}

#endif
