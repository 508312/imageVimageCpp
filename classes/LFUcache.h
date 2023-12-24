#ifndef LFUCACHE_H
#define LFUCACHE_H
#include <unordered_map>

struct node {
    int val;
    node* left_node;
    node* right_node;
};

/** LFU cache without the use of counter. Made for use for working with indexes of images.
    Uses doubly linked list implementation, where hottest nodes are at the start and the
    coldest ones are at the back.
**/
class LFUcache
{
    public:
        LFUcache(int mCapacity);
        virtual ~LFUcache();

        /** puts the selected index in the LFU cache, returns removed item or -1 if nothing has been removed. **/
        int put(int ind);

    protected:

    private:
        /** moves the selected node to the top(right) of the linked list. **/
        void insertStart(node* p_node);
        /** move the selected node to the bot(left) of the linked lost. **/
        void insertBack(node* p_node);
        /** removes first node. **/
        void removeBack();
        /** removes node from arbitrary position. Doesn't delete it **/
        void removeNode(node* p_node);

        /** first node of the linked list. **/
        node* mStartNode = nullptr;
        /** last node of the linked list. **/
        node* mBackNode = nullptr;

        /** current mCapacity of this lfu cache. **/
        int mCapacity;
        /** max mCapacity of this lfu cache. **/
        int mMaxCapacity;

        /** Index to node mapping. **/
        std::unordered_map<int, node*> mapping;

        /** prints useful information. **/
        void debugPrintInfo();
};

#endif // LFUCACHE_H
