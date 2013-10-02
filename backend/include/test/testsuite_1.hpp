#ifndef TESTSUITE_1_H
#define TESTSUITE_1_H
#include <boost/filesystem.hpp>
#include <iterator>
#include "include/fileproperty.hpp"
#include "include/rule_factory/filerules.hpp"
#include "include/rule_factory/rule_bag.hpp"
#include "include/rule_factory/dls_filerules.hpp"
#include "rule_factory/dls_filerules.hpp"
#include <unordered_set>
#include <functional>

namespace Test
{

    using namespace boost::filesystem;
    using namespace std;
    using namespace DLS;

    class testSuite_1
    {
        public:
            testSuite_1() {}
            ~testSuite_1() {}
        protected:
        private:
    };



bool byteBybyte(const DLS::FileProperty& fp1, const DLS::FileProperty& fp2)
{
    DLS::FileReader r1(fp1.getFilePath());
    DLS::FileReader r2(fp2.getFilePath());

    if(r1.size() != r2.size())
        return true;

    const unsigned long bytes = 8192;

    while(!r1.eof())
    {
        if(r1.getStringByBytes(bytes) != r2.getStringByBytes(bytes))
            return true;
    }

    return false;
}


class FUNC_FilePropertySize
{
    public:
    std::size_t operator () (const DLS::FileProperty& prop) const
    {
        return std::hash<unsigned long>()(prop.getSize());
    }
};

class FUNC_FilePropertySizeEquality
{
    public:
    bool operator () (const DLS::FileProperty& pr1, const DLS::FileProperty& pr2) const
    {
        return (pr1.getSize() == pr2.getSize());
    }
};


class FUNC_FilePropertyHashA
{
    public:
    std::size_t operator () (const DLS::FileProperty& prop) const
    {
        return std::hash<std::string>()(prop.getHashA());
    }
};

class FUNC_FilePropertyHashAEquality
{
    public:
    bool operator () (const DLS::FileProperty& pr1, const DLS::FileProperty& pr2) const
    {
        return (pr1.getHashA() == pr2.getHashA());
    }
};


void throwDuplicate(const DLS::FileProperty& fp1, const DLS::FileProperty& fp2)
{
    cout << "DUPLICATE FOUND!" << endl;
    cout << fp1.getFilePath() << endl;
    cout << "SIZE: " << fp1.getSize() << "    Hash: " << fp1.getHashA() << endl;

    cout << "\nis same with" << endl;
    cout << fp2.getFilePath() << endl;
    cout << "SIZE: " << fp2.getSize() << "    Hash: " << fp2.getHashA() << endl << endl;
}



class printHexInserter : public iterator<output_iterator_tag, directory_entry>
{
    int count = 0;
    int duplicates = 0;
    typedef std::unordered_set<DLS::FileProperty, FUNC_FilePropertySize, FUNC_FilePropertySizeEquality> USet_FPS;
    typedef std::unordered_set<DLS::FileProperty, FUNC_FilePropertyHashA, FUNC_FilePropertyHashAEquality> USet_FPH;
    USet_FPS values;
    USet_FPH valuesByHashA;
    DLS::RuleBag<DLS::Path, DLS::FileProperty> rulebag;

    public:
    printHexInserter() { makeBag01(rulebag); }
    printHexInserter& operator = (const directory_entry& ent )
    {
        //cout << ent.path().string() << endl;
        DLS::Path pt( ent.path().string() );
        FileProperty fp;
        rulebag.set(pt);

        if(rulebag.nextRule())
        {
            fp = rulebag.getVerdict();

            if(rulebag.Accepted())
            {
                auto fp_it = values.find(fp);

                if(fp_it == values.end())       //Does not exist
                    values.insert(fp);
                else                            //exists
                {
                    if(rulebag.nextRule())
                    {
                        auto fp2 = *fp_it;      //dereference already existing FileProperty
                        rulebag.getVerdict(fp); //Pass it for verdict
                        rulebag.getVerdict(fp2);//Pass it for 2nd order verdict

                        //std::cout << "\nFP1:" << fp.getFilePath() << "\nHashA: " << fp.getHashA() << std::endl;
                        //std::cout << "\nFP2:" << fp2.getFilePath() << "\nHashA: " << fp2.getHashA() << std::endl;

                        valuesByHashA.insert(fp2);
                        auto fp2_it = valuesByHashA.find(fp);

                        if(fp2_it == valuesByHashA.end())
                            valuesByHashA.insert(fp);
                        else
                        {
                            if(!byteBybyte(*fp2_it, fp))
                            {
                                ++duplicates;
                                throwDuplicate(*fp2_it, fp);
                            }

                            return *this;
                        }

                    }
                }
            }
        }
        //cout << pt.toString() << endl;
        return *this;
    }
    printHexInserter& operator ++ ()    { count++; return *this; }
    printHexInserter& operator *  ()    { return *this; }
    printHexInserter& operator ++ (int) { return *this; }
    ~printHexInserter()
    {
        cout << "Duplicates found: " << duplicates << endl;
    }

    int getCount() const { return count; };

};


bool directory_entry_is_regular_file(const directory_entry& dir_ent)
{
    return is_regular_file(dir_ent.path());
}


int RecursiveTest()
{
    //path pt("/home/whiztim/Documents/developer/C++/");
    path pt("/home/whiztim/Documents/developer/C++/LibreCAD/librecad/");
    //path pt("/home/whiztim/Documents/developer/C++/Codeblocks/DuplichaseTest");

    printHexInserter in = copy_if(recursive_directory_iterator(pt), recursive_directory_iterator(),
            printHexInserter(), directory_entry_is_regular_file);
    return in.getCount();
}


void test1()
{
    int count = RecursiveTest();
    cout << "Found " << count << " files" << endl;
    cout << "MAX: " << ((size_t) - 1) << endl;
}

}   //end namespace

#endif // TESTSUITE_1_H
