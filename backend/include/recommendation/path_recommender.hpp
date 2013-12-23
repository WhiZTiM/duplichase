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
#ifndef PATH_RECOMMENDER_HXX
#define PATH_RECOMMENDER_HXX
#include "backend/include/directory_elements/simple_directory_tree.hpp"
#include <unordered_map>
#include <functional>

namespace DLS
{
    namespace Tree = SimpleDirectoryTree;


    class DEEdgeProperty : public Tree::IDirectoryElementProperty
    {
        public:
            const DEEdgeProperty* Clone(const Tree::DirectoryElement* Parent) const;
            long long outgoing() const;
            void outgoing(long long);

            long long incoming() const;
            void incoming(long long);

            void raiseOutgoing() const;
            void raiseIncoming() const;

            void lowerOutgoing() const;
            void lowerIncoming() const;

            void setOutgoing(long long value) const;
            void setIncoming(long long value) const;

            void used(bool) const;
            bool used() const;
        private:
            mutable long long _outgoing = 0;     // Abuse of the keyword mutable! ...yikes!
            mutable long long _incoming = 0;     // Sometimes, you gotta write codes politically
            mutable bool _used = false;
    };


    class SimpleDEEdgeAgent
    {
        public:
            SimpleDEEdgeAgent();
            ~SimpleDEEdgeAgent();
            void addPath(const std::string&);
            bool raiseOutgoing(const std::string&);
            bool raiseIncoming(const std::string&);
            bool lowerIncoming(const std::string&);
            bool lowerOutgoing(const std::string&);
            long long getIncoming(const std::string&) const;
            long long getOutgoing(const std::string&) const;
            bool lastgetSuccessful() const;

            void resetToZero();
            void clear();
        private:
            std::unordered_map<std::string, Tree::DirectoryElement*> mStringElem;
            Tree::DirectoryElement* rootElement = nullptr;
            mutable bool _success = false;

            inline const DEEdgeProperty* cast(const Tree::IDirectoryElementProperty* property) const
            { return static_cast<const DEEdgeProperty*>(property); }

            const Tree::DirectoryElement* find(const std::string& path) const;
    };


    class PathRecommender
    {
        public:
            PathRecommender();
            PathRecommender(const std::vector<std::string>& vecString);
            void addPath(const std::string& path);

            //! \a giver is delete. \a reciever is keep
            bool accentuate(const std::string& giver, const std::string& reciever);
            bool setWeights(const std::string& ToBeDeleted, const std::string& ToBeKept);
            bool unsetWeights(const std::string& ToBeDeleted, const std::string& ToBeKept);
            long long getIncomingIndex(const std::string& path) const;
            long long getOutgoingIndex(const std::string& path) const;
            long long getKeepingWeight(const std::string& path) const { return getOutgoingIndex(path); }
            long long getDeletionWeight(const std::string& path)const { return getIncomingIndex(path); }

            void resetAllToZero();
            void clear();

            virtual ~PathRecommender();
        protected:
        private:
            SimpleDEEdgeAgent deAgent;
    };
}

#endif // PATH_RECOMMENDER_HXX
