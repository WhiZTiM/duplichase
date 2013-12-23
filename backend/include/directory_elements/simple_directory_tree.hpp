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
#ifndef SIMPLE_DIRECTORY_TREE_HXX
#define SIMPLE_DIRECTORY_TREE_HXX

#include <string>
#include <vector>

namespace DLS
{

namespace SimpleDirectoryTree
{

    //! An interface that is attached to every DirectoryElement Node
    //! This is to allow the flexibility of adding extra stuff you wish to the element
    class IDirectoryElementProperty;



    class DirectoryElement
    {
    public:


        //! Convenience
        typedef std::vector<DirectoryElement*> Container;
        typedef Container::iterator ContainerIterator;
        typedef Container::const_iterator constContainerIterator;

        //! Constructs An Element with the Name \Name
        DirectoryElement(const std::string& Name, const DirectoryElement* Parent = nullptr);

        /*! Adds a child Element and return a pointer to the child
         * if denyDuplicates is \a true , then duplicate child is avoided
         * if deleteChildDuplicate is \a true , then, it deletes either the new child to be added or the child that was existing before
         * if deleteNewChild is \a true , then the new chlid to be added is deleted
         * NOTE these parameters only work if the preceeding boolean is true!!
         * i.e : you cannot \a deleteChildDuplicate if \a denyDuplicates is \a false
         *  ---> same goes for \a deleteNewChild if \a deleteChildDuplicate is \a false
         * TODO ---> change these 3 default boolean parameters to an \b enum
         * */
        DirectoryElement* addChild(DirectoryElement* child, bool denyDuplicates = true, bool deleteChildDuplicate = true, bool deleteNewChild = true);

        //! With the inclusion or exclusion of the topmost parent... or parent without a parent
        //! Gets the path from the top parent to this...
        std::string getFullPath(bool includeTopMostParent = false) const;

        //! Gets the name of this
        std::string getElementName() const;

        //! gets a const pointer to the property added
        const IDirectoryElementProperty* property() const;

        /*! sets the property
         * \NOTE: If a property is attached, operator delete will be called on the attached property at the destruction of this Node
         * if set to nullptr, you detach the property!!
         */
        void property(const IDirectoryElementProperty* Property);

        //! Checks whether this Node is a directory, assuming this->getFullPath() is a valid path
        bool isDirectory () const;

        //! Checks whether this Node is a file, assuming this->getFullPath() is a valid path
        bool isFile () const;

        //! Returns \a true if this node has children
        inline bool hasChildren() const
        { return !children.empty(); }

        //! Returns the parent of this node as a const pointer
        inline const DirectoryElement* getParent() const
        { return parent; }

        //! Returns a const pointer to the found element if found... else, \a nullptr
        static const DirectoryElement* find(const DirectoryElement* element, const Container& elements);

        //! Returns a const pointer to the found element if found... else, \a nullptr
        static const DirectoryElement* find_by_name(const std::string& name, const Container& elements);

        //! Returns a const pointer to the found element if found... else, \a nullptr
        static const DirectoryElement* find_by_full_path(const std::string& fullPath, const Container& elements);

        //! returns a deep copy of the elements, the optional parent parameter can be used to parent the deep copy to a Parent pointer
        //! This function returns a Copy of source if parent is nullptr; else it deep copies source, adds it to Parent and returns Parent
        static DirectoryElement* deepCopy(const DirectoryElement* source, DirectoryElement* Parent = nullptr);

        //! returns a deep copy of the elements, the optional parent parameter can be used to parent the deep copy to a Parent pointer
        //! This function always returns a copy of \a source without the parent!
        static DirectoryElement* createDeepCopy(const DirectoryElement* source, const DirectoryElement* Parent = nullptr);

        //! Returns a const pointer to merged Elements.
        //! Uses a simple and efficient tree merging algorithm
        //! The \a interWeave parameter is dangerous
        //! \NOTE: CURRENTLY NEEDS WORK
        static void merge(DirectoryElement* const toBeMerged, DirectoryElement* const reciever, bool denyDuplicates = true, bool interWeave = false) = delete;

        //! returns the children of this node as a vector of const pointers to DirectoryElement
        std::vector<DirectoryElement*> getChildren() const;

        //! Deletes all children recursively then itself
        ~DirectoryElement();

    private:
        //! A pointer to the parent directory
        const DirectoryElement* parent;

        //! A vector of child nodes
        std::vector<DirectoryElement*> children;

        //! The name of the element
        std::string elementName;

        //! For any extra dummy element you want to add
        const IDirectoryElementProperty* baseProperty = nullptr;
    };



    ///////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////



    class DirectoryElementHelper
    {
    public:
        //! If destroyWithMe is false, the DirectoryElement is completely deleted at destruction
        explicit DirectoryElementHelper(bool DestroyWithMe = true);

        //! If destroyWithMe is false, the DirectoryElement is completely deleted at destruction
        DirectoryElementHelper(DirectoryElement* element, bool createDeepCopy = true, bool DestroyWithMe = false);

        //! Adds path in a neat and efficient manner
        const DirectoryElement* addPath(const std::string& Path);

        //! returns the relusting directory structure
        DirectoryElement* getDirectoryElement() const;

        //! Destructor
        ~DirectoryElementHelper();

    private:
        //! The root element whose name is an empty string
        DirectoryElement* rootElement;

        //! destruction flag
        bool destroy;
    };

    ///////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////



    class IDirectoryElementProperty
    {
    public:
        IDirectoryElementProperty(){}
        virtual const IDirectoryElementProperty* Clone(const DirectoryElement*) const = 0;
        virtual ~IDirectoryElementProperty(){}
    };



/* <<UNCOMMENT IF you want to recursively print nodes for testing...
//! < Print Function for a directoryElement
template<typename Stream, typename Delimeter>
inline void writeToStream(const DirectoryElement* element, Stream& stream, Delimeter& delimeter, int c = 0)
{
    if(element->hasChildren())
    {
	std::vector<DirectoryElement*> vecElem = element->getChildren();

	for(auto elem : vecElem)
	{
	    writeToStream(elem, stream, delimeter, c++);
	}
    }

    stream << "(" << c << ")" << element->getFullPath() << delimeter << std::endl;
}

*/

}
}
#endif //SIMPLE_DIRECTORY_TREE_H
