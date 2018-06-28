#include <BufferManager/BufferManager.h>
#include <IndexManager/BPlusTree.h>
#include <algorithm>
#include <memory>

namespace IM {

namespace BPlusTree {

Node::Node(int fanout) : parent(NullPtr), numKeys(0), isLeaf(false) {
    keys = new Key[fanout - 1];
    children = new Ptr[fanout];
    std::fill_n(children, fanout, NullPtr);
}

Node::~Node() {
    delete[] keys;
    delete[] children;
}

bool Node::isRoot() { return parent == NullPtr; }

void Node::readFromBlock(BM::PtrBlock blk,
                         const std::pair<ValueType, size_t> &info) {
    Ptr ptr;
    Value value;
    uint32_t isLeafCnt;

    blk->resetPos(0);
    blk->read(reinterpret_cast<char *>(&numKeys), sizeof(uint32_t));
    blk->read(reinterpret_cast<char *>(&isLeafCnt), sizeof(uint32_t));
    blk->read(reinterpret_cast<char *>(&parent), sizeof(Ptr));
    isLeaf = (isLeafCnt == 1);
    for (int i = 0; i <= numKeys; i++) {
        blk->read(reinterpret_cast<char *>(&ptr), sizeof(Ptr));
        children[i] = ptr;
    }
    for (int i = 0; i < numKeys; i++) {
        value.type = info.first;
        value.charCnt = info.second;
        blk->read(value.val(), value.size());
        keys[i] = value;
    }
}

void Node::writeToBlock(BM::BlockID id) {
    uint32_t _offset = 0;
    auto write = [&](const char *src, size_t size) {
        BM::writeBlock(id, src, _offset, size);
        _offset += size;
    };
    uint32_t isLeafCnt = (isLeaf ? 1u : 0u);
    write(reinterpret_cast<const char *>(&numKeys), sizeof(uint32_t));
    write(reinterpret_cast<const char *>(&isLeafCnt), sizeof(uint32_t));
    write(reinterpret_cast<const char *>(&parent), sizeof(Ptr));
    for (int i = 0; i <= numKeys; i++) {
        write(reinterpret_cast<const char *>(&children[i]), sizeof(Ptr));
    }
    for (int i = 0; i < numKeys; i++) {
        write(keys[i].val(), keys[i].size());
    }
}

std::tuple<Ptr, Off, bool> Tree::find(const Key &key) const {
    if (root == NullPtr)
        return std::make_tuple(NullPtr, NullPtr, false);
    Ptr currOffset = root;
    BM::PtrBlock blk = BM::readBlock(BM::makeID(filename, currOffset));
    auto curr = std::make_shared<Node>(fanout);
    curr->readFromBlock(blk, info);
    while (!curr->isLeaf) {
        Ptr nextOffset = NullPtr;
        for (int i = 0; i < curr->numKeys; i++) {
            if (curr->keys[i] == key) {
                nextOffset = curr->children[i + 1];
                break;
            } else if (curr->keys[i] > key) {
                nextOffset = curr->children[i];
                break;
            }
            nextOffset = curr->children[i + 1];
        }
        blk = BM::readBlock(BM::makeID(filename, nextOffset));
        curr->readFromBlock(blk, info);
        currOffset = nextOffset;
    }
    for (int i = 0; i < curr->numKeys; i++) {
        if (curr->keys[i] == key) {
            return std::make_tuple(currOffset, curr->children[i], true);
        }
    }
    return std::make_tuple(currOffset, NullPtr, false);
}

bool Tree::hasKey(const Key &key) const {
    auto result = find(key);
    return std::get<2>(result);
}

void Tree::remove(const Key &key) const {}

void Tree::insert(const Key &key, const Off &offset) {
    Ptr leafOffset = NullPtr;
    auto leaf = std::make_shared<Node>(fanout);
    if (root == NullPtr) {
        root = header.numBlocks++;
        leafOffset = root;
    } else {
        auto result = find(key);
        if (std::get<2>(result)) {
            BM::BlockID id = BM::makeID(filename, std::get<0>(result));
            BM::PtrBlock blk = BM::readBlock(id);
            auto leaf = std::make_shared<Node>(fanout);
            leaf->readFromBlock(blk, info);
            for (int i = 0; i < leaf->numKeys; i++) {
                if (leaf->keys[i] == key) {
                    leaf->children[i] = offset;
                    leaf->writeToBlock(id);
                    return;
                }
            }
        }
        leafOffset = std::get<0>(result);
        BM::PtrBlock blk = BM::readBlock(BM::makeID(filename, leafOffset));
        leaf->readFromBlock(blk, info);
    }
    if (leaf->numKeys < fanout - 1) {
        int i = leaf->numKeys;
        for (i--; i >= 0; i--) {
            if (leaf->keys[i] > key) {
                leaf->keys[i + 1] = leaf->keys[i];
                leaf->children[i + 1] = leaf->children[i];
            } else {
                break;
            }
        }
        leaf->keys[i + 1] = key;
        leaf->children[i + 1] = offset;
        leaf->numKeys++;
        leaf->writeToBlock(BM::makeID(filename, leafOffset));
    } else {
        Key *tmpKeys = new Key[fanout];
        Ptr *tmpChildren = new Ptr[fanout];

        std::copy(leaf->keys, leaf->keys + fanout - 1, tmpKeys);
        std::copy(leaf->children, leaf->children + fanout - 1, tmpChildren);

        int i = fanout - 1;
        for (i--; i >= 0; i--) {
            if (tmpKeys[i] > key) {
                tmpKeys[i + 1] = tmpKeys[i];
                tmpChildren[i + 1] = tmpChildren[i];
            } else {
                break;
            }
        }
        tmpKeys[i + 1] = key;
        tmpChildren[i + 1] = offset;

        int numKeys0 = (fanout + 1) / 2;
        int numKeys1 = fanout - numKeys0;

        auto leaf0 = leaf;
        auto leaf1 = std::make_shared<Node>(fanout);
        Ptr leafOffset0 = leafOffset;
        Ptr leafOffset1 = header.numBlocks++;

        leaf1->isLeaf = true;
        leaf1->parent = leaf0->parent;

        leaf0->numKeys = numKeys0;
        leaf1->numKeys = numKeys1;

        leaf1->children[fanout - 1] = leaf0->children[fanout - 1];
        leaf0->children[fanout - 1] = leafOffset1;

        std::fill_n(leaf0->children, fanout - 1, NullPtr);
        std::fill_n(leaf1->children, fanout - 1, NullPtr);

        for (int i = 0; i < numKeys0; i++) {
            leaf0->keys[i] = tmpKeys[i];
            leaf0->children[i] = tmpChildren[i];
        }

        if (numKeys0 < fanout - 1) {
            leaf0->children[numKeys0] = NullPtr;
        }

        for (int i = 0, j = numKeys0; i < numKeys1; i++, j++) {
            leaf1->keys[i] = tmpKeys[j];
            leaf1->children[i] = tmpChildren[j];
        }

        delete[] tmpKeys;
        delete[] tmpChildren;

        leaf0->writeToBlock(BM::makeID(filename, leafOffset0));
        leaf1->writeToBlock(BM::makeID(filename, leafOffset1));

        insert_in_parent(leafOffset0, leaf1->keys[0], leafOffset1);
    }
}

void Tree::insert_in_parent(const Ptr &nodeOffset0, const Key &key,
                            const Ptr &nodeOffset1) {
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, nodeOffset0));
    BM::PtrBlock blk1 = BM::readBlock(BM::makeID(filename, nodeOffset1));
    auto node0 = std::make_shared<Node>(fanout);
    auto node1 = std::make_shared<Node>(fanout);
    node0->readFromBlock(blk0, info);
    node1->readFromBlock(blk1, info);
    if (node0->isRoot()) {
        Ptr rootOffset = header.numBlocks++;
        auto root = std::make_shared<Node>(fanout);
        root->numKeys = 1;
        root->keys[0] = key;
        root->children[0] = nodeOffset0;
        root->children[1] = nodeOffset1;
        node0->parent = rootOffset;
        node1->parent = rootOffset;
        node0->writeToBlock(BM::makeID(filename, nodeOffset0));
        node1->writeToBlock(BM::makeID(filename, nodeOffset1));
        root->writeToBlock(BM::makeID(filename, rootOffset));
        this->root = rootOffset;
        this->header.rootOffset = rootOffset;
    } else {
        Ptr parentOffset = node0->parent;
        BM::PtrBlock blkP = BM::readBlock(BM::makeID(filename, parentOffset));
        auto parent = std::make_shared<Node>(fanout);
        parent->readFromBlock(blkP, info);
        if (parent->numKeys < fanout - 1) {
            int i = parent->numKeys;
            for (; i >= 0; i--) {
                if (parent->children[i] != nodeOffset0) {
                    parent->children[i + 1] = parent->children[i];
                    parent->keys[i] = parent->keys[i - 1];
                } else {
                    break;
                }
            }
            parent->children[i + 1] = nodeOffset1;
            parent->keys[i] = key;
            parent->numKeys++;
            parent->writeToBlock(BM::makeID(filename, parentOffset));
        } else {
            Key *tmpKeys = new Key[fanout];
            Ptr *tmpChildren = new Ptr[fanout + 1];

            std::fill_n(tmpChildren, fanout + 1, NullPtr);

            std::copy(parent->keys, parent->keys + fanout - 1, tmpKeys);
            std::copy(parent->children, parent->children + fanout, tmpChildren);

            int i = fanout;
            for (i--; i > 0; i--) {
                if (tmpChildren[i] != nodeOffset0) {
                    tmpChildren[i + 1] = tmpChildren[i];
                    tmpKeys[i] = tmpKeys[i - 1];
                } else {
                    break;
                }
            }
            tmpChildren[i + 1] = nodeOffset1;
            tmpKeys[i] = key;

            int numDiv2 = (fanout + 1) / 2;
            int numKeys0 = numDiv2 - 1;
            int numKeys1 = fanout - numDiv2;

            auto parent0 = parent;
            auto parent1 = std::make_shared<Node>(fanout);
            Ptr parentOffset0 = parentOffset;
            Ptr parentOffset1 = header.numBlocks++;

            parent1->parent = parent0->parent;

            parent0->numKeys = numKeys0;
            parent1->numKeys = numKeys1;

            std::fill_n(parent0->children, fanout, NullPtr);
            std::fill_n(parent1->children, fanout, NullPtr);

            std::copy(tmpKeys, tmpKeys + numKeys0, parent0->keys);
            std::copy(tmpChildren, tmpChildren + numKeys0 + 1,
                      parent0->children);
            Key key1 = tmpKeys[numKeys0];
            std::copy(tmpKeys + numKeys0 + 1, tmpKeys + fanout, parent1->keys);
            std::copy(tmpChildren + numKeys0 + 1, tmpChildren + fanout + 1,
                      parent1->children);

            for (int i = 0; i <= numKeys1; i++) {
                Ptr childOffset = parent1->children[i];
                BM::PtrBlock blkT =
                    BM::readBlock(BM::makeID(filename, childOffset));
                auto child = std::make_shared<Node>(fanout);
                child->readFromBlock(blkP, info);
                child->parent = parentOffset1;
                child->writeToBlock(BM::makeID(filename, childOffset));
            }

            delete[] tmpKeys;
            delete[] tmpChildren;

            insert_in_parent(parentOffset0, key1, parentOffset1);
        }
    }
}

} // namespace BPlusTree

} // namespace IM