#include "LFUcache.h"

LFUcache::LFUcache(int capacity)
{
    this->capacity = 0;
    max_capacity = capacity;
}

LFUcache::~LFUcache()
{
    node* p_cur = p_start_node;
    node* p_next = nullptr;

    while (p_cur != nullptr) {
        p_next = p_cur->right_node;
        delete p_cur;
        p_cur = p_next;
    }
}

void LFUcache::remove_node(node* p_node) {
    node* p_old_right = p_node->right_node;
    node* p_old_left = p_node->left_node;
    if (p_old_left != nullptr) {
        p_old_left->right_node = p_old_right;
    } else {
        p_start_node = p_old_right;
    }
    if (p_old_right != nullptr) {
        p_old_right->left_node = p_old_left;
    } else {
        p_back_node = p_old_left;
    }
}

void LFUcache::insert_start(node* p_node) {
    p_node->right_node = p_start_node;
    if (p_start_node != nullptr) {
        p_start_node->left_node = p_node;
    }
    p_start_node = p_node;
}

void LFUcache::insert_back(node* p_node) {
    p_node->left_node = p_back_node;
    if (p_back_node != nullptr) {
        p_back_node->right_node = p_node;
    }
    p_back_node = p_node;
}

void LFUcache::remove_back() {
    node* old_back = p_back_node;
    p_back_node = p_back_node->left_node;
    if (p_back_node != nullptr)
        p_back_node->right_node = nullptr;
    delete old_back;
}

int LFUcache::put(int ind) {
    int return_value = -1;
    node* new_node;
    if (mapping[ind] == 0) {
        new_node = new node;
        new_node->val = ind;
        new_node->left_node = nullptr;
        new_node->right_node = nullptr;
        mapping[ind] = new_node;
        capacity += 1;
    } else {
        new_node = mapping[ind];
        remove_node(new_node);
    }

    if (capacity == 1) {
        p_back_node = new_node;
        p_start_node = new_node;
        return -1;
    }

    if (capacity > max_capacity) {
        return_value = p_back_node->val;
        mapping[return_value] = 0;
        remove_back();
        capacity -= 1;
    }
    insert_start(new_node);

    return return_value;
}
