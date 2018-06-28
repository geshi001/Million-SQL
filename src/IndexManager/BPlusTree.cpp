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

bool Tree::hasKey(const Key &key) const {}

void Tree::remove(const Key &key) const {}

void Tree::insert(const Key &key, const Off &offset) {}

void Tree::insert_in_leaf(const Ptr &leaf, const Key &key, const Off &offset) {}

void Tree::insert_in_parent(const Ptr &node0, const Key &key,
                            const Ptr &node1) {}

} // namespace BPlusTree

} // namespace IM