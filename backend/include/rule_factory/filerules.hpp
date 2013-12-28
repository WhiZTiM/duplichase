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
#ifndef FILERULES_H
#define FILERULES_H

//! \WARNING: all classes here and their member functions are guaranteed NOT re-entrant and NOT thread-safe!

#include "backend/include/rule_facade/rule_facade.hpp"
#include "backend/include/fileproperty.hpp"
#include "backend/include/filereader.hpp"
#include "backend/include/hashers/md5_hash.hpp"
#include "backend/include/rule_factory/extension_aggregator.hpp"
#include "backend/include/path.hpp"
#include <cstring>
#include <unordered_set>

namespace DLS
{
    const unsigned long MAX_MEMORY_BUFFER_LIMIT = 67108863;
    std::string readerHexDigest(FileReader& reader, float percentage, FileReader::OPT::Position Options);

    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    //////
    //////      This rule takes care of file sizes
    //////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    //! inclusion rule based on FileSize
    class FR_by_size : public DLS::rule_facade<Path, FileProperty>
    {
    public:
        //! \a min -> minimum filesize in bytes, \a max -> maximum file size in bytes
        //! \a option see \enum enum class rule_option
        FR_by_size(ulong min, ulong max, rule_option option = rule_option::Accept)
            : _min(min), _max(max), ruleOpt(option), _err(false)
        {
            std::cout << "FR_by_size ctor: _min: " << _min << "  _max: " << _max << std::endl;
        }

        //! we checkout the file's size
        bool operator () (const Path& p) override
        {
            FileReader reader(p.toString());

            if(!reader)
            {
                _err = true;
                _error = "Failed to get the file size of: \"" + reader.getFilePath() + "\"";
                return false;
            }

            ulong sz = reader.size();

            if(reader.fail())
            {
                _err = true;
                _error = "Fatal Error while trying to get the size of of \"" + reader.getFilePath() + "\" --Failed to open stream";
                return false;
            }

            property.setFilePath(p.toString());
            property.setSize(sz);

            return ( (sz >= _min) && (sz <= _max) );
        }

        //! approval option
        rule_option getOption() const override
        { return ruleOpt; }

        //! best way to duplicate this class
        FR_by_size* Clone() const override
        { return new FR_by_size(*this); }

        //! returns error
        bool inError() const override
        { return _err; }

        //! string representation of error
        std::string getError() const override
        { return _error; }

        //! silly stuff
        std::string className() const override
        { return "FR_by_size()"; }

        //! we ruturn our property as verdict
        FileProperty getVerdict() const override
        { return property; }

        //! we only modify size as verdict
        bool getVerdict(FileProperty& fprop) const override
        {
            fprop.setSize(property.getSize());
            return true;
        }

    private:
        FileProperty property;
        std::string _error;
        ulong _min;
        ulong _max;
        rule_option ruleOpt;
        bool _err;
    };



    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    //////
    //////      This rule takes care of file hashes by the first x percent
    //////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    //! this is an inclusion rule hashes the first portion of the file
    class FR_by_hashA : public DLS::rule_facade<Path, FileProperty>
    {
        public:
        FR_by_hashA(float percentage, rule_option option = rule_option::Accept)
            : _err(false), ruleOpt(option)
        {
            if(percentage > 100.00)
                percent = 100.0;
            percent = percentage;
        }

        bool operator () (const Path& path) override
        {
            property.setFilePath(path.toString());
            FileReader reader(path.toString());

            if(!reader)
            {
                _err = true;
                _error = "Failed to get hashA of \"" + reader.getFilePath() + "\"";
                return false;
            }

            //! read percentage bytes
            //Hash::MD5 md( reader.getStringByPercentage(percent, FileReader::OPT::Start) );
            //hashA = md.hexdigest();
            hashA = readerHexDigest(reader, percent, FileReader::OPT::Start);
            property.setHashA(hashA);

            if(reader.fail())
            {
                _err = true;
                _error = "Fatal Error while trying to get hashA of \"" + reader.getFilePath() + "\" --Failed to open stream";
                return false;
            }

            //std::cout << "\nHASH: " << hashA;

            if(hashA.empty())
                return false;
            return true;
        }

        rule_option getOption() const override
        { return ruleOpt; }

        FR_by_hashA* Clone() const override
        { return new FR_by_hashA(*this); }

        bool inError() const override
        { return _err; }

        //! very silly! ... arrgh
        std::string className() const override
        { return "FR_by_hashA()"; }

        std::string getError() const override
        { return _error; }

        FileProperty getVerdict() const override
        { return property; }

        //! modify the property and return true
        bool getVerdict(FileProperty& fprop) const override
        {
            fprop.setHashA(property.getHashA());
            return true;
        }

    private:
        FileProperty property;
        std::string _error;
        std::string hashA;
        float percent;
        bool _err;
        rule_option ruleOpt;
    };




    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    //////
    //////      This rule hashes the last x percent of the file
    //////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    //! Another inclusive rule based on hashing the last bytes of the file
    class FR_by_hashB : public DLS::rule_facade<Path, FileProperty>
    {
        public:
        FR_by_hashB(float percentage, rule_option option = rule_option::Accept) : ruleOpt(option)
        {
            if(percentage > 100.00)
                percent = 100.0;
            percent = percentage;
        }

        bool operator () (const Path& path) override
        {
            property.setFilePath(path.toString());
            FileReader reader(path.toString());

            if(!reader)
            {
                _err = true;
                _error = "Failed to get hashB of \"" + reader.getFilePath() + "\"";
                return false;
            }

            //Hash::MD5 md( reader.getStringByPercentage(percent, FileReader::OPT::End) );
            //hashB = md.hexdigest();
            hashB = readerHexDigest(reader, percent, FileReader::OPT::End);
            property.setHashB(hashB);

            if(reader.fail())
            {
                _err = true;
                _error = "Fatal Error while trying to get hashB of \"" + reader.getFilePath() + "\" --Failed to open stream";
                return false;
            }

            if(hashB.empty())
                return false;
            return true;
        }

        rule_option getOption() const override
        { return ruleOpt; }

        bool inError() const override
        { return _err; }

        FR_by_hashB* Clone() const override
        { return new FR_by_hashB(*this); }

        std::string className() const override
        { return "FR_by_hashB()"; }

        std::string getError() const override
        { return _error; }

        FileProperty getVerdict() const override
        { return property; }

        bool getVerdict(FileProperty& fprop) const override
        { fprop.setHashB(property.getHashB()); return true; }

        private:
        FileProperty property;
        rule_option ruleOpt;
        std::string _error;
        std::string hashB;
        float percent;
        bool _err;
    };




    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    //////
    //////      This rule hashes certain percentage of the file
    //////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    //! Another inclusion rule based on the middle Hash ..i.e hashes a portion from 50% of the file
    class FR_by_hashC : public DLS::rule_facade<Path, FileProperty>
    {
        public:
        FR_by_hashC(float percentPosition, float percentage, rule_option option = rule_option::Accept) : ruleOpt(option)
        {
            if(percentage > 100.00)
                percentage = 100.0;
            percent = percentage;
            if(percentPosition > 100)
                percentPosition = 100;
            position = percentPosition;
        }

        bool operator () (const Path& path) override
        {
            property.setFilePath(path.toString());
            FileReader reader(path.toString());

            if(!reader)
            {
                _err = true;
                _error = "Failed to get hashC of \"" + reader.getFilePath() + "\" --Failed to open stream";
                return false;
            }

            ulong bytesPosition = reader.getBytesByPercentage(position);
            reader.setCurrentBytePosition(bytesPosition);

            //Hash::MD5 md( reader.getStringByPercentage(percent, FileReader::OPT::LastUsed) );
            //hashC = md.hexdigest();
            hashC = readerHexDigest(reader, percent, FileReader::OPT::LastUsed);
            property.setHashC(hashC);

            if(reader.fail())
            {
                _err = true;
                _error = "Fatal Error while trying to get hashC of \"" + reader.getFilePath() + "\"";
                return false;
            }

            if(hashC.empty())
                return false;
            return true;
        }

        rule_option getOption() const override
        { return ruleOpt; }

        bool inError() const override
        { return _err; }

        FR_by_hashC* Clone() const override
        { return new FR_by_hashC(*this); }

        std::string className() const override
        { return "FR_by_hashC()"; }

        std::string getError() const override
        { return _error; }

        FileProperty getVerdict() const override
        { return property; }

        bool getVerdict(FileProperty& fprop) const override
        { fprop.setHashC(property.getHashC()); return true; }

        private:
        FileProperty property;
        rule_option ruleOpt;
        std::string _error;
        std::string hashC;
        float position;
        float percent;
        bool _err;
    };


    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    //
    //
    //      This section contains a lovely way to select extensions
    //
    //
    //
    //
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////



    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    //! this is a bit of a special rule class, it is both an inclusion and exclution rule
    //! Constructor is supplied with a list of extensions to include or exclude
    class FR_by_extension : public DLS::rule_facade<Path,  FileProperty>
    {
        public:
        //! noExtension determines wgether files wthout extentioons sgould be valid
        FR_by_extension(const ConditionedList& extensions, bool AcceptNoExtension, bool Strict, rule_option option = rule_option::Accept)
            : extensionHandle(extensions), noExtension(AcceptNoExtension), ruleOpt(option), strict(Strict)
        {
            //
        }

        bool operator () (const Path& path) override
        {
            property.setFilePath( path.toString() );
            fs::path pt = path.fsPath();

            if(!pt.has_extension())
            {

                bool acceptance = noExtension ? true : extensionHandle.getEveryOther(ConditionedList::Option::Inclusion); /* &&
                                 !extensionHandle.getEveryOther(ConditionedList::Option::Exclusion); */
                return acceptance;
            }

            std::string ext = path.fsPath().extension().string();
            return (strict ? (extensionHandle.includes(ext) && !extensionHandle.excludes(ext)) : extensionHandle.includes(ext) );

        }

        rule_option getOption() const override
        { return ruleOpt; }

        bool inError() const override
        { return _err; }

        FR_by_extension* Clone() const override
        { return new FR_by_extension(*this); }

        std::string className() const override
        { return "FR_by_extension()"; }

        std::string getError() const override
        { return _error; }

        FileProperty getVerdict() const override
        { return property; }

        bool getVerdict(FileProperty& fprop) const override
        { (void)fprop; return true; }

        private:
        ConditionedList extensionHandle;
        FileProperty property;
        std::string _error;
        bool noExtension;
        rule_option ruleOpt;
        bool strict;
        bool _err;

    };



    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////



    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    //
    //
    //      This rule adaptor takes input of two FileProperty and yields
    //      another FileProperty
    //
    //
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////
    ///
    ///

    inline std::string readerHexDigest(FileReader& reader, float percentage, FileReader::OPT::Position Options)
    {
        std::string rtn;
        //! read percentage bytes
        if(reader.getBytesByPercentage(percentage) > MAX_MEMORY_BUFFER_LIMIT)
        {
            const unsigned long sz = reader.getBytesByPercentage(percentage);
            unsigned long chunk = MAX_MEMORY_BUFFER_LIMIT;
            unsigned long chunkReadSoFar = 0;
            Hash::MD5 md;
            bool firstRead = true;
            while(chunk > 0)
            {
                std::string chunkData;
                if(firstRead)
                {
                    chunkData = std::move(reader.getStringByBytes(chunk, Options));
                    firstRead = false;
                }
                else
                    chunkData = std::move(reader.getStringByBytes(chunk, FileReader::OPT::LastUsed));
                md.update(chunkData.c_str(), chunkData.length());
                if(chunkData.empty())
                    return std::string();

                chunkReadSoFar += chunk;
                if(sz - chunkReadSoFar < MAX_MEMORY_BUFFER_LIMIT)
                    chunk = sz - MAX_MEMORY_BUFFER_LIMIT;
            }
            md.finalize();
            rtn = md.hexdigest();
        }
        else
        {
            Hash::MD5 md( reader.getStringByPercentage(percentage, FileReader::OPT::Start) );
            rtn = md.hexdigest();
        }

        return rtn;
    }

}
#endif // FILERULES_H
