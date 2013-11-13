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
            unsigned long outgoing() const;
            void outgoing(unsigned);

            unsigned long incoming() const;
            void incoming(unsigned);

            void raiseOutgoing() const;
            void raiseIncoming() const;

            void lowerOutgoing() const;
            void lowerIncoming() const;

            void used(bool) const;
            bool used() const;
        private:
            mutable unsigned long _outgoing = 0;     // Abuse of the keyword mutable! ...yikes!
            mutable unsigned long _incoming = 0;     // Sometimes, you gotta write codes politically
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
            unsigned long getIncoming(const std::string&) const;
            unsigned long getOutgoing(const std::string&) const;
            bool lastgetSuccessful() const;
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
            unsigned long getIncomingIndex(const std::string& path) const;
            unsigned long getOutgoingIndex(const std::string& path) const;
            unsigned long getKeepingWeight(const std::string& path) const { return getOutgoingIndex(path); }
            unsigned long getDeletionWeight(const std::string& path)const { return getIncomingIndex(path); }

            virtual ~PathRecommender();
        protected:
        private:
            SimpleDEEdgeAgent deAgent;
    };
}

#endif // PATH_RECOMMENDER_HXX
