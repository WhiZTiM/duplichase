#include "backend/include/duplicatefinder.hpp"
#include <atomic>
#include <future>
#include <thread>
#include <algorithm>
#include <chrono>

using namespace DLS;

DuplicateFinder::DuplicateFinder(const DLS::DuplicateFinderConstruct& dfConstruct)
    : filesCounted(false), dfAggregate(true, true), dfCon(dfConstruct), exclusionHandle(dfCon.exclusionHandle, false),
    _newPath(true), _finallyEnded(false)
{
    //static_assert(!dfConstruct._usingDeepCopy, "You must have deep Copy enabled")
    //ctor
    count_future = std::async(std::launch::async, &DuplicateFinder::getTotalFilesToBeTransversed, this);
}


bool DuplicateFinder::getTotalFilesToBeTransversed()
{
    //! copy start iterator
    //DLS::RecursiveDirectoryIterator iter(Iterator);

    //const DLS::ExclusionHandler exclusionHandle(dfCon.exclusionHandle, false);
    unsigned long yield_counter = 0;
    for(auto& fpath : dfCon.folderPaths)
    {
        DLS::RecursiveDirectoryIterator iter(boost::filesystem::path(fpath), exclusionHandle);

        while(!iter.isEnded() && !_endCounting)
        {
            //! yield after 50 runs
            if(++yield_counter > 50)
            {
                //std::this_thread::yield();
                //std::this_thread::sleep_for(std::chrono::milliseconds(10));
                yield_counter = 0;
            }

            if(iter.inError())
            {
                //filesCounted.store(true);
                //return false;
                continue;
            }

            if(boost::filesystem::is_regular_file(iter.getDirectoryEntry().path()))
            {
                ++countedFiles;
            }

            ++iter;
        }
    }
    filesCounted.store(true);
    return true;
}


bool DuplicateFinder::forceCounting()
{
    countingSuccessful = count_future.get();
    return countingSuccessful;
}


FileProperty DuplicateFinder::nextFile()
{
    _finallyEnded = forradPaths();
    return dfAggregate.getFileProperty();
}


bool DuplicateFinder::forradPaths()
{
    if(folderPathsIter < dfCon.folderPaths.size())
    {
        if(_newPath)
            ptrDirectoryIterator = new RecursiveDirectoryIterator(dfCon.folderPaths[folderPathsIter], exclusionHandle);
        _newPath = false;

        if(ptrDirectoryIterator->isEnded())
        {
            ++folderPathsIter;
            _newPath = true;
            delete ptrDirectoryIterator;
            ptrDirectoryIterator = nullptr;
            if(forradPaths())
                return true;
        }

        while(!boost::filesystem::is_regular_file(ptrDirectoryIterator->getDirectoryEntry().path()) && !(ptrDirectoryIterator->isEnded()))
        {
            ++(*ptrDirectoryIterator);
        }

        ruledepth = dfAggregate.addPath(ptrDirectoryIterator->getDirectoryEntry().path().generic_string());
        ++currentCount;

        if(!ptrDirectoryIterator->isEnded())
            ++(*ptrDirectoryIterator);

        return false;
    }

    return true;
}


BasicTaggedFileProperty DuplicateFinder::getBasicTaggedFileProperty() const
{
    return dfAggregate.getLastestTag();
}


DuplicatesContainer DuplicateFinder::getDuplicatesContainer() const
{
    return dfAggregate.getDuplicatesContainer();
}


std::string DuplicateFinder::getCurrentError() const
{
    return dfAggregate.lastInputError();
}

void DuplicateFinder::clearDuplicatesContainer()
{
    dfAggregate.clearDuplicatesContainer();
}

DuplicateFinder::~DuplicateFinder()
{
    if(ptrDirectoryIterator)
        delete ptrDirectoryIterator;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



//! Will be auto generated with its siblings;;;:: -->WAS too lazy to write more codes
DuplicateFinderConstruct::DuplicateFinderConstruct(const ExclusionHandler& handle)
    : exclusionHandle(handle, true)
{
    //deeeep
}


DuplicateFinderConstruct::DuplicateFinderConstruct(const DuplicateFinderConstruct& construcT)
    : folderPaths(construcT.folderPaths), exclusionHandle(construcT.exclusionHandle, true)
{
    //deeeep
}


void DuplicateFinderConstruct::addPath(const std::string& FolderPath)
{
    folderPaths.push_back(FolderPath);
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


DuplicateFinderAggregator::DuplicateFinderAggregator(bool deleteDupConAllocations, bool deleteRuleAllocations)
    : deleteVects(deleteDupConAllocations), deleteRules(deleteRuleAllocations),
     _duplicateFound(false), increment_variable(0)
{
    //ctor;
    makeRuleBag();
}

DuplicateFinderAggregator::~DuplicateFinderAggregator()
{
    if(deleteRules)
    {
        for(auto& it : V_RF)
            delete it;
    }
}


void DuplicateFinderAggregator::clearDuplicatesContainer()
{
    dupContainer.clear();
}


int DuplicateFinderAggregator::addPath(const std::string& newPath)
{
    //gymnastics
    //! fp01 represents the current (newPath)'s FileProperty
    DLS::FileProperty fp01, fp02;
    _duplicateFound = false;

    rulebg.set(DLS::Path(newPath));

    if(rulebg.nextRule())
    {
        fp01 = rulebg.getVerdict();
        if(rulebg.getOption() == DLS::rule_option::Accept)
        {

            //! 1st CASE:: SIZE
            if(!existIn(fp01, fpSet_size, fp02))
            {
                insertInto(fp01, fpSet_size);
                tag_it_nonDuplicate(fp01);
                lastError = rulebg.getError();
                return 1;
            }

            //! 2nd Case :::  HASH_A
            if(rulebg.nextRule())
            {
                rulebg.getVerdict(fp01);
                rulebg.getIndependentVerdict(fp02.getFilePath(), fp02);

                if(rulebg.getOption() == DLS::rule_option::Accept)
                {
                    if(!existIn(fp02, fpSet_hashA))
                    {
                        insertInto(fp02, fpSet_hashA);
                    }

                    if(!existIn(fp01, fpSet_hashA, fp02))
                    {
                        insertInto(fp01, fpSet_hashA);

                        tag_it_nonDuplicate(fp01);
                        lastError = rulebg.getError();
                        return 2;
                    }
                }

            }

            //! 3rd Case :: HashB
            if(rulebg.nextRule())
            {
                rulebg.getVerdict(fp01);
                rulebg.getIndependentVerdict(fp02.getFilePath(), fp02);

                if(rulebg.getOption() == DLS::rule_option::Accept)
                {
                    if(!existIn(fp02, fpSet_hashB))
                    {
                        insertInto(fp02, fpSet_hashB);
                    }

                    if(!existIn(fp01, fpSet_hashB, fp02))
                    {
                        insertInto(fp01, fpSet_hashB);

                        tag_it_nonDuplicate(fp01);
                        lastError = rulebg.getError();
                        return 3;
                    }

                }

            }

            //! At this point we have the assurance that fp01 and fp02 are duplicates
            _duplicateFound = true;
            insertAppropriately(fp01, fp02);
            lastError = rulebg.getError();
            return 4;
        }
    }

    //! Well we weren't able to add the string! //Sorry, no rule Available!
    lastError = rulebg.getError();
    return false;

}


void DuplicateFinderAggregator::insertAppropriately(const DLS::FileProperty& newFileProperty, const DLS::FileProperty& oldFileProperty)
{
    ptrVEC_FileProperty vproperty(new VEC_FileProperty({oldFileProperty}));
    //vproperty.push_back(oldFileProperty);

    bFPtag.property = newFileProperty;
    bFPtag.isDuplicate = true;

    auto dupIterator = dupContainer.find(vproperty);
    if(dupIterator == dupContainer.end())       //! if NOT found
    {
        ++increment_variable;
        bFPtag.id = increment_variable;
        fptag_id_map.push_back( std::make_pair(newFileProperty, increment_variable) );

        vproperty->push_back(newFileProperty);
        dupContainer.insert(vproperty);         //! insert them!!
        return;
    }

    //! else we foooooound it!!!! :-)
    (*dupIterator)->push_back(newFileProperty);
    lastestFP = newFileProperty;

}


DuplicatesContainer DuplicateFinderAggregator::getDuplicatesContainer() const
{
    return dupContainer;
}


BasicTaggedFileProperty DuplicateFinderAggregator::getLastestTag() const
{
    auto k_pos =
        std::find_if(fptag_id_map.begin(), fptag_id_map.end(),
                        [&](const std::pair<const DLS::FileProperty, const int>& k) -> bool
                            {
                                return DLS::FE_FP_hashB()( (*this).lastestFP, k.first);
                            }
                    );
    //if(k_pos != fptag_id_map.end())
    bFPtag.id = k_pos - fptag_id_map.begin();

    return bFPtag;
}


std::vector< DLS::rule_facade<DLS::Path, DLS::FileProperty>* > DuplicateFinderAggregator::getRules() const
{
    return V_RF;
}


FileProperty DuplicateFinderAggregator::getFileProperty() const
{
    return bFPtag.property;
}


std::string DuplicateFinderAggregator::lastInputError() const
{
    return lastError;
}


void DuplicateFinderAggregator::makeRuleBag()
{
        RuleSequence<Path, FileProperty> rules;

        //! 0 bytes to 1KB
        V_RF.push_back( new FR_by_size(0, 2047, rule_option::Dismiss) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(100) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashB(100) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 1KB to 50KB
        V_RF.push_back( new FR_by_size(2048, 51199UL) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(100) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashB(100) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 50Kb to 250KB
        V_RF.push_back( new FR_by_size(51200UL, 262143UL) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(100) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashB(100) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 250KB to 1MB
        V_RF.push_back( new FR_by_size(262144UL, 1048575UL) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(30) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashB(19) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 1MB to 8MB
        V_RF.push_back( new FR_by_size(1048576UL, 8388607UL) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(15) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashB(13) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 8MB to 24MB
        V_RF.push_back( new FR_by_size(8388608UL, 25165823UL) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(13) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashB(19) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 24MB to 64MB
        V_RF.push_back( new FR_by_size(25165824UL, 67108863UL) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(10) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashB(17) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 64MB to 256MB
        V_RF.push_back( new FR_by_size(67108864UL, 268435455UL) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(9) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashB(12) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 256MB to 1GB
        V_RF.push_back( new FR_by_size(67108864UL, 1073741823UL) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(7) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashB(10) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();


        //! 1GB to 4GB
        V_RF.push_back( new FR_by_size(1073741824UL, 4294967295UL) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashA(5) );
        rules.AddRule(V_RF.back());

        V_RF.push_back( new FR_by_hashB(6) );
        rules.AddRule(V_RF.back());

        rulebg.AddRule(rules);
        rules.Clear();
}

