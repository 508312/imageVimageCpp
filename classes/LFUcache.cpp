#include "LFUcache.h"
#include <iostream>

LFUcache::LFUcache(int mCapacity)
{
    this->mCapacity = 0;
    mMaxCapacity = mCapacity;
}

LFUcache::~LFUcache()
{
    node* p_cur = mStartNode;
    node* p_next = nullptr;

    while (p_cur != nullptr) {
        p_next = p_cur->right_node;
        delete p_cur;
        p_cur = p_next;
    }
}

void LFUcache::removeNode(node* p_node) {
    //std::cout << "b4 remove " << std::endl;
    //debugPrintInfo();
    node* p_old_right = p_node->right_node;
    node* p_old_left = p_node->left_node;
    if (p_old_left != nullptr) {
        p_old_left->right_node = p_old_right;
    } else {
        mStartNode = p_old_right;
    }
    if (p_old_right != nullptr) {
        p_old_right->left_node = p_old_left;
    } else {
        mBackNode = p_old_left;
    }
    //std::cout << "after remove " << std::endl;
    //debugPrintInfo();
}

void LFUcache::insertStart(node* p_node) {
    p_node->right_node = mStartNode;
    p_node->left_node = nullptr;
    if (mStartNode != nullptr) {
        mStartNode->left_node = p_node;
    }
    mStartNode = p_node;
}

void LFUcache::insertBack(node* p_node) {
    p_node->left_node = mBackNode;
    p_node->right_node = nullptr;
    if (mBackNode != nullptr) {
        mBackNode->right_node = p_node;
    }
    mBackNode = p_node;
}

void LFUcache::removeBack() {
    node* old_back = mBackNode;
    mBackNode = mBackNode->left_node;
    if (mBackNode != nullptr)
        mBackNode->right_node = nullptr;
    delete old_back;
}

int LFUcache::put(int ind) {
    //std::cout << "coming in mCapacity " << mCapacity << std::endl;
    int return_value = -1;
    node* new_node;
    if (mapping[ind] == 0) {
        new_node = new node;
        new_node->val = ind;
        new_node->left_node = nullptr;
        new_node->right_node = nullptr;
        mapping[ind] = new_node;
        mCapacity += 1;

        //std::cout << "not found new ptr" << mapping[ind] << std::endl;
    } else {
        //std::cout << "found ptr " << mapping[ind] << std::endl;
        new_node = mapping[ind];
        removeNode(new_node);
    }

    if (mCapacity == 1) {
        //std::cout << "mCapacity is 1 " << new_node << " " << mBackNode << " " << mStartNode << std::endl;
        mBackNode = new_node;
        //std::cout << "assigning back " << std::endl;
        mStartNode = new_node;
        //std::cout << "returning " << std::endl;
        return -1;
    }

    if (mCapacity > mMaxCapacity) {
        if (mBackNode != nullptr)
            return_value = mBackNode->val;

        mapping[return_value] = 0;
        removeBack();
        mCapacity -= 1;
    }

    //std::cout << "insert start" << std::endl;
    insertStart(new_node);
    //std::cout << "insert end" << std::endl;
    return return_value;
}

void LFUcache::debugPrintInfo() {
    std::cout << "start " << mStartNode << " end " << mBackNode << std::endl;
    std::cout << "list itself: ";

    node* p_cur = mStartNode;
    node* p_next = nullptr;

    while (p_cur != nullptr) {
        p_next = p_cur->right_node;
        std::cout << p_cur << " ";
        p_cur = p_next;
    }
    std::cout << std::endl;
}
