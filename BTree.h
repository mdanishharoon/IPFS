#pragma once

#include <iostream>
#include "bigInt.h"
#include <queue>
using namespace std;

struct filePathKey {
    string path;
    bint key;
};

class BTreeNode {
public:
    filePathKey* keys;
    int degree;
    BTreeNode** children;
    int numKeys;
    bool isLeaf;

    BTreeNode(int _degree, bool _isLeaf);
    void traverse();

    int findKey(filePathKey key);
    void insertNonFull(filePathKey key);
    void splitChild(int i, BTreeNode* child);
    void deletion(filePathKey key);
    void removeFromLeaf(int index);
    void removeFromNonLeaf(int index);
    filePathKey getPredecessor(int index);
    filePathKey getSuccessor(int index);
    void fill(int index);
    void borrowFromPrev(int index);
    void borrowFromNext(int index);
    void merge(int index);
    bool searchKey(filePathKey key);
    friend class BTree;
};

class BTree {
public:
    BTreeNode* root;
    int degree;
    
    BTree() {
        degree = 3;
    }

    BTree(int _degree) {
        root = nullptr;
        degree = _degree;
    }

    void traverse() {
        if (root != nullptr)
            root->traverse();
    }

    void insertion(filePathKey key);
    void deletion(filePathKey key);
    bool searchKey(filePathKey key);
    void printBTree();
};

// B tree node
BTreeNode::BTreeNode(int degree, bool isLeaf) {
    this->degree = degree;
    this->isLeaf = isLeaf;

    keys = new filePathKey[2 * degree - 1];
    children = new BTreeNode * [2 * degree];

    numKeys = 0;
}

// Find the key
int BTreeNode::findKey(filePathKey toFind) {
    int index = 0;
    while (index < numKeys && keys[index].key < toFind.key)
        ++index;
    return index;
}

// Deletion operation
void BTreeNode::deletion(filePathKey key) {
    int index = findKey(key);

    if (index < numKeys && keys[index].key == key.key) {
        if (isLeaf)
            removeFromLeaf(index);
        else
            removeFromNonLeaf(index);
    }
    else {
        if (isLeaf) {
            cout << "The key " << key.key << " does not exist in the tree\n";
            return;
        }

        bool flag = ((index == numKeys) ? true : false);

        if (children[index]->numKeys < degree)
            fill(index);

        if (flag && index > numKeys)
            children[index - 1]->deletion(key);
        else
            children[index]->deletion(key);
    }
    return;
}

// Remove from the leaf
void BTreeNode::removeFromLeaf(int index) {
    for (int i = index + 1; i < numKeys; ++i)
        keys[i - 1] = keys[i];

    numKeys--;

    return;
}

// Delete from non-leaf node
void BTreeNode::removeFromNonLeaf(int index) {
    filePathKey key = keys[index];

    if (children[index]->numKeys >= degree) {
        filePathKey pred = getPredecessor(index);
        keys[index] = pred;
        children[index]->deletion(pred);
    }

    else if (children[index + 1]->numKeys >= degree) {
        filePathKey succ = getSuccessor(index);
        keys[index] = succ;
        children[index + 1]->deletion(succ);
    }

    else {
        merge(index);
        children[index]->deletion(key);
    }
    return;
}

filePathKey BTreeNode::getPredecessor(int index) {
    BTreeNode* current = children[index];
    while (!current->isLeaf)
        current = current->children[current->numKeys];

    return current->keys[current->numKeys - 1];
}

filePathKey BTreeNode::getSuccessor(int index) {
    BTreeNode* current = children[index + 1];
    while (!current->isLeaf)
        current = current->children[0];

    return current->keys[0];
}

void BTreeNode::fill(int index) {
    if (index != 0 && children[index - 1]->numKeys >= degree)
        borrowFromPrev(index);

    else if (index != numKeys && children[index + 1]->numKeys >= degree)
        borrowFromNext(index);

    else {
        if (index != numKeys)
            merge(index);
        else
            merge(index - 1);
    }

    return;
}

// Borrow from previous
void BTreeNode::borrowFromPrev(int index) {
    BTreeNode* child = children[index];
    BTreeNode* sibling = children[index - 1];

    for (int i = child->numKeys - 1; i >= 0; --i)
        child->keys[i + 1] = child->keys[i];

    if (!child->isLeaf) {
        for (int i = child->numKeys; i >= 0; --i)
            child->children[i + 1] = child->children[i];
    }

    child->keys[0] = keys[index - 1];

    if (!child->isLeaf)

        child->children[0] = sibling->children[sibling->numKeys];

    keys[index - 1] = sibling->keys[sibling->numKeys - 1];

    child->numKeys += 1;
    sibling->numKeys -= 1;

    return;
}

// Borrow from the next
void BTreeNode::borrowFromNext(int index) {
    BTreeNode* child = children[index];
    BTreeNode* sibling = children[index + 1];

    child->keys[(child->numKeys)] = keys[index];

    if (!(child->isLeaf))
        child->children[(child->numKeys) + 1] = sibling->children[0];

    keys[index] = sibling->keys[0];

    for (int i = 1; i < sibling->numKeys; ++i)
        sibling->keys[i - 1] = sibling->keys[i];

    if (!sibling->isLeaf) {
        for (int i = 1; i <= sibling->numKeys; ++i)
            sibling->children[i - 1] = sibling->children[i];
    }

    child->numKeys += 1;
    sibling->numKeys -= 1;

    return;
}

// Merge
void BTreeNode::merge(int index) {
    BTreeNode* child = children[index];
    BTreeNode* sibling = children[index + 1];

    child->keys[degree - 1] = keys[index];

    for (int i = 0; i < sibling->numKeys; ++i)
        child->keys[i + degree] = sibling->keys[i];

    if (!child->isLeaf) {
        for (int i = 0; i <= sibling->numKeys; ++i)
            child->children[i + degree] = sibling->children[i];
    }

    for (int i = index + 1; i < numKeys; ++i)
        keys[i - 1] = keys[i];

    for (int i = index + 2; i <= numKeys; ++i)
        children[i - 1] = children[i];

    child->numKeys += sibling->numKeys + 1;
    numKeys--;

    delete (sibling);
    return;
}

void BTree::insertion(filePathKey key) {
    if (root == nullptr) {
        root = new BTreeNode(degree, true);
        root->keys[0] = key;
        root->numKeys = 1;
    }
    else {
        if (root->numKeys == 2 * degree - 1) {
            BTreeNode* newRoot = new BTreeNode(degree, false);

            newRoot->children[0] = root;

            newRoot->splitChild(0, root);

            int i = 0;
            if (newRoot->keys[0].key < key.key)
                i++;
            newRoot->children[i]->insertNonFull(key);

            root = newRoot;
        }
        else
            root->insertNonFull(key);
    }
}

void BTreeNode::insertNonFull(filePathKey key) {
    int i = numKeys - 1;

    if (isLeaf) {
        while (i >= 0 && keys[i].key > key.key) {
            keys[i + 1] = keys[i];
            i--;
        }

        keys[i + 1] = key;
        numKeys = numKeys + 1;
    }
    else {
        while (i >= 0 && keys[i].key > key.key)
            i--;

        if (children[i + 1]->numKeys == 2 * degree - 1) {
            splitChild(i + 1, children[i + 1]);

            if (keys[i + 1].key < key.key)
                i++;
        }
        children[i + 1]->insertNonFull(key);
    }
}

// Split child
void BTreeNode::splitChild(int i, BTreeNode* y) {
    BTreeNode* z = new BTreeNode(y->degree, y->isLeaf);
    z->numKeys = degree - 1;

    for (int j = 0; j < degree - 1; j++)
        z->keys[j] = y->keys[j + degree];

    if (y->isLeaf == false) {
        for (int j = 0; j < degree; j++)
            z->children[j] = y->children[j + degree];
    }

    y->numKeys = degree - 1;

    for (int j = numKeys; j >= i + 1; j--)
        children[j + 1] = children[j];

    children[i + 1] = z;

    for (int j = numKeys - 1; j >= i; j--)
        keys[j + 1] = keys[j];

    keys[i] = y->keys[degree - 1];

    numKeys = numKeys + 1;
}

// Traverse
void BTreeNode::traverse() {
    int i;
    cout << endl;
    for (i = 0; i < numKeys; i++) {
        if (!isLeaf)
            children[i]->traverse();
        cout << " " << keys[i].key;
    }

    if (!isLeaf)
        children[i]->traverse();
}

// Delete Operation
void BTree::deletion(filePathKey key) {
    if (!root) {
        cout << "The tree is empty\n";
        return;
    }

    root->deletion(key);

    if (root->numKeys == 0) {
        BTreeNode* tmp = root;
        if (root->isLeaf)
            root = nullptr;
        else
            root = root->children[0];

        delete tmp;
    }
    return;
}

bool BTree::searchKey(filePathKey key) {
    if (!root) {
        return false;  // The tree is empty, key cannot be present.
    }

    // Call the searchKey function starting from the root.
    return root->searchKey(key);
}

bool BTreeNode::searchKey(filePathKey key) {
    int index = 0;
    while (index < numKeys && keys[index].key < key.key)
        ++index;

    // If the key is found in the current node, return true.
    if (index < numKeys && keys[index].key == key.key && keys[index].path == key.path)
        return true;

    // If the node is a leaf, and the key is not found, return false.
    if (isLeaf)
        return false;

    // Recursively search in the appropriate child node.
    return children[index]->searchKey(key);
}

void BTree::printBTree() {
    cout << endl;
    if (!root) {
        cout << "The tree is empty\n";
        return;
    }

    queue<BTreeNode*> q;
    q.push(root);

    // Level-order traversal
    while (!q.empty()) {
        int nodesAtCurrentLevel = q.size();

        // Process each node at the current level
        for (int i = 0; i < nodesAtCurrentLevel; ++i) {
            // Dequeue the front node
            BTreeNode* current = q.front();
            q.pop();

            // Print the keys of the current node

            cout << "(";
            for (int j = 0; j < current->numKeys - 1; ++j)
                cout << current->keys[j].key << ",";
            cout << current->keys[current->numKeys - 1].key;
            cout << ")";

            // Enqueue children if the current node is not a leaf
            if (!current->isLeaf) {
                for (int j = 0; j <= current->numKeys; ++j) {
                    if (current->children[j])
                        q.push(current->children[j]);
                }
            }

            // Separate nodes at the same level
            cout << "    ";
        }
        cout << endl;  // Move to the next level
    }
}
