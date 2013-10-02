#ifndef DUPLICATEFINDER_HXX
#define DUPLICATEFINDER_HXX

#include <atomic>
#include <future>
#include <string>
#include <vector>
#include "exception.hpp"
#include "rule_factory/dls_filerules.hpp"
#include "directory_elements/directory_iterator.hpp"
#include "container_helpers/fp_holders.hpp"
#include "container_helpers/fp_unordered_set.hpp"


namespace DLS
{

    struct BasicTaggedFileProperty
    {
        DLS::FileProperty   property;
        bool                isDuplicate;
        int                 id;
    };


    class DuplicateFinderAggregator
    {
    public:
        DuplicateFinderAggregator(bool deleteDupConAllocations = true, bool deleteAllocatedRules = true);

        //! adds a new file path and does respective processing
        int addPath(const std::string& newPath);

        void clearDuplicatesContainer();

        //! gets all the duplicate files
        //! \a deepCopy determines whether to make deep copy of all
        DuplicatesContainer getDuplicatesContainer() const;

        //! gets the last property insert
        BasicTaggedFileProperty getLastestTag() const;

        //! a cheaper function
        FileProperty getFileProperty() const;

        //! returns a pointer to the rules used
        std::vector< DLS::rule_facade<DLS::Path, DLS::FileProperty>* > getRules() const;

        //! returns a string representation of the last input's error
        std::string lastInputError() const;

        //! returns true if the last input was a duplicate
        bool lastInputWasDuplicate() const
        { return _duplicateFound; }


        ~DuplicateFinderAggregator();

    private:

        bool deleteVects;
        bool deleteRules;

        typedef DLS::rule_facade<DLS::Path, DLS::FileProperty>* VEC_RF;
        std::vector<VEC_RF> V_RF;

        FPSet_size  fpSet_size;
        FPSet_hashA fpSet_hashA;
        FPSet_hashB fpSet_hashB;

        FileProperty lastestFP;
        mutable BasicTaggedFileProperty bFPtag;
        std::vector<std::pair<const DLS::FileProperty, const int>> fptag_id_map;
        std::string lastError;
        bool _duplicateFound;
        int increment_variable;

        DuplicatesContainer dupContainer;
        DLS::RuleBag<DLS::Path, DLS::FileProperty> rulebg;

        void makeRuleBag();
        void insertAppropriately(const DLS::FileProperty& newFileProperty, const DLS::FileProperty& oldFileProperty);

        inline void tag_it_nonDuplicate(const DLS::FileProperty property)
        {
            bFPtag.property = property;
            bFPtag.isDuplicate = false;
            bFPtag.id = 0;
        }

    };




    //////////////////////////////////////////////////
    /////////////////////////////////////////////////////
    ////////////////////////////////////////////////

    class DuplicateFinder;


    class DuplicateFinderConstruct
    {
    public:
        DuplicateFinderConstruct(const ExclusionHandler& handle);
        DuplicateFinderConstruct(const DuplicateFinderConstruct& construcT);

        DuplicateFinderConstruct(DuplicateFinderConstruct&& construcT) = default;
        DuplicateFinderConstruct& operator = (DuplicateFinderConstruct&&) = default;
        void addPath(const std::string& FolderPath);

    private:
        friend DuplicateFinder;

        std::vector<std::string> folderPaths;
        DLS::ExclusionHandler exclusionHandle;
    };



    //////////////////////////////////////////////////
    /////////////////////////////////////////////////////
    ////////////////////////////////////////////////




    class DuplicateFinder
    {
    public:
        //! Constructorrr
        DuplicateFinder(const DuplicateFinderConstruct& dfConstruct);

        //! Return The whole container
        DuplicatesContainer getDuplicatesContainer() const;

        //! deletes all elements in the continer... saves a few MB of RAM
        void clearDuplicatesContainer();

        //! The probability of Duplicate; -->lies between 0 and 4
        inline int ruleDepth() const
        { return ruledepth; }

        //! gets a \var BasicTaggedFileProperty
        BasicTaggedFileProperty getBasicTaggedFileProperty() const;

        //! checks whether the system has finished transversing through its entire construct
        inline bool processFinished() const
        { return _finallyEnded; }

        inline bool endend() const
        { return _finallyEnded; }

        //! advances to the next file it sees along its way
        FileProperty nextFile();

        //! gets the current error
        std::string getCurrentError() const;

        //! Not really important.. scraped out this
        std::shared_future<unsigned long> totalFiles() const;

        //! returns true if file counting has been done
        inline bool CountsPrepared() const
        { return filesCounted.load(); }

        //! returns the current files transversed so far
        inline unsigned long getCount() const
        { return currentCount; }

        //! return total count
        unsigned long totalCount() const
        { return countedFiles; }

        inline void endCounting(bool value)
        { _endCounting = value; }

        //! Blocks until file counting is done
        bool forceCounting();

        ~DuplicateFinder();

    private:

        //! just for keeps
        bool countingSuccessful = false;

        //! shared Future to hold the file transversing;
        std::shared_future<bool> count_future;

        //! all the files counted
        unsigned long countedFiles = 0;

        //! the current number of files processed
        mutable unsigned long currentCount = 0;

        //! used by this object to determine if the file counting is finished
        std::atomic<bool> filesCounted;

        //! The Class where every Duplicate is resolved
        DLS::DuplicateFinderAggregator dfAggregate;

        //! what the scanner is gonna scan
        DLS::DuplicateFinderConstruct dfCon;

        //! const reusable exclusionHandle
        const DLS::ExclusionHandler exclusionHandle;

        //! The function that transverses through these
        bool getTotalFilesToBeTransversed();

        //! flattens all inputs and gradually iterates over them
        bool forradPaths();

        int ruledepth = 0;

        size_t folderPathsIter = 0;

        DLS::RecursiveDirectoryIterator* ptrDirectoryIterator = nullptr;

        bool _newPath;
        bool _finallyEnded;
        volatile bool _endCounting = false;

    };


    //////////////////////////////////////////////////
    /////////////////////////////////////////////////////
    ////////////////////////////////////////////////

}
#endif // DUPLICATEFINDER_HXX
