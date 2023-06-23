#include "LFUcache.h"
#include <iostream>

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
    //std::cout << "b4 remove " << std::endl;
    //debug_print_info();
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
    //std::cout << "after remove " << std::endl;
    //debug_print_info();
}

void LFUcache::insert_start(node* p_node) {
    p_node->right_node = p_start_node;
    p_node->left_node = nullptr;
    if (p_start_node != nullptr) {
        p_start_node->left_node = p_node;
    }
    p_start_node = p_node;
}

void LFUcache::insert_back(node* p_node) {
    p_node->left_node = p_back_node;
    p_node->right_node = nullptr;
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
    //std::cout << "coming in capacity " << capacity << std::endl;
    int return_value = -1;
    node* new_node;
    if (mapping[ind] == 0) {
        new_node = new node;
        new_node->val = ind;
        new_node->left_node = nullptr;
        new_node->right_node = nullptr;
        mapping[ind] = new_node;
        capacity += 1;

        //std::cout << "not found new ptr" << mapping[ind] << std::endl;
    } else {
        //std::cout << "found ptr " << mapping[ind] << std::endl;
        new_node = mapping[ind];
        remove_node(new_node);
    }

    if (capacity == 1) {
        //std::cout << "capacity is 1 " << new_node << " " << p_back_node << " " << p_start_node << std::endl;
        p_back_node = new_node;
        //std::cout << "assigning back " << std::endl;
        p_start_node = new_node;
        //std::cout << "returning " << std::endl;
        return -1;
    }

    if (capacity > max_capacity) {
        if (p_back_node != nullptr)
            return_value = p_back_node->val;

        mapping[return_value] = 0;
        remove_back();
        capacity -= 1;
    }

    //std::cout << "insert start" << std::endl;
    insert_start(new_node);
    //std::cout << "insert end" << std::endl;

    return return_value;
}

void LFUcache::debug_print_info() {
    std::cout << "start " << p_start_node << " end " << p_back_node << std::endl;
    std::cout << "list itself: ";

    node* p_cur = p_start_node;
    node* p_next = nullptr;

    while (p_cur != nullptr) {
        p_next = p_cur->right_node;
        std::cout << p_cur << " ";
        p_cur = p_next;
    }
    std::cout << std::endl;
}
