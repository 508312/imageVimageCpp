#ifndef LFUCACHE_H
#define LFUCACHE_H
#include <unordered_map>

struct node {
    int val;
    node* left_node;
    node* right_node;
};

/* LFU cache without the use of counter. Made for use for working with indexes of images.
    Uses doubly linked list implementation, where freshest nodes are at the start and the
    stalest nodes are at the back*/
class LFUcache
{
    public:
        LFUcache(int capacity);
        virtual ~LFUcache();

        // puts the selected index in the LFU cache, returns removed item or -1 if nothing has been removed.
        int put(int ind);

    protected:

    private:
        // moves the selected node to the top(right) of the linked list
        void insert_start(node* p_node);
        // move the selected node to the bot(left) of the linked lost
        void insert_back(node* p_node);
        // removes first node
        void remove_back();
        // removes node from arbitrary position
        void remove_node(node* p_node);

        // first node of the linked list
        node* p_start_node = nullptr;
        // last node of the linked list
        node* p_back_node = nullptr;

        // current capacity of this lfu cache
        int capacity;
        // max capacity of this lfu cache
        int max_capacity;

        // Index to node mapping
        std::unordered_map<int, node*> mapping;

        //prints useful information
        void debug_print_info();
};

#endif // LFUCACHE_H
