#pragma once
#include <BufferManager/Block.h>
#include <DataType.h>
#include <FileSpec.h>
#include <cstdint>
#include <tuple>
#include <vector>

namespace IM {

namespace BPlusTree {

using Ptr = uint32_t;
constexpr Ptr NullPtr = 0U;
using Key = Value;
using Off = uint32_t;

struct Node {
    Key *keys;
    Ptr parent;
    Ptr *children;
    bool isLeaf;
    int numKeys;
    Node(int);
    ~Node();
    bool isRoot();
    void readFromBlock(BM::PtrBlock, const std::pair<ValueType, size_t> &);
    void writeToBlock(BM::BlockID);
};

struct Tree {
    Ptr root;
    int fanout;
    std::pair<ValueType, size_t> info;
    std::string filename;
    File::indexFileHeader header;
    Tree(int fanout) : fanout(fanout), root(NullPtr) {}
    std::tuple<Ptr, Off, bool> find(const Key &) const;
    bool hasKey(const Key &) const;
    void remove(const Key &) const;
    void insert(const Key &, const Off &);

  private:
    void insert_in_parent(const Ptr &, const Key &, const Ptr &);
};

} // namespace BPlusTree

} // namespace IM