//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/execution/index/art/node256.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once
#include "duckdb/execution/index/art/node.hpp"
#include "duckdb/execution/index/art/swizzleable_pointer.hpp"

namespace duckdb {

class Node256 : public Node {
public:
	explicit Node256();
	ARTPointer children[256];

public:
	static Node256 *New();
	//! Get position of a specific byte, returns DConstants::INVALID_INDEX if not exists
	idx_t GetChildPos(uint8_t k) override;
	//! Get the position of the first child that is greater or equal to the specific byte, or DConstants::INVALID_INDEX
	//! if there are no children matching the criteria
	idx_t GetChildGreaterEqual(uint8_t k, bool &equal) override;
	//! Get the position of the minimum element in the node
	idx_t GetMin() override;
	//! Get the next position in the node, or DConstants::INVALID_INDEX if there is no next position
	idx_t GetNextPos(idx_t pos) override;
	//! Get Node256 child
	Node *GetChild(ART &art, idx_t pos) override;
	//! Replace child pointer
	void ReplaceChildPointer(idx_t pos, Node *node) override;

	//! Insert a new child node at key_byte into the Node256
	static void InsertChild(Node *&node, uint8_t key_byte, Node *new_child);
	//! Erase the child at pos and (if necessary) shrink to Node48
	static void EraseChild(Node *&node, int pos, ART &art);
	//! Merge Node256 into l_node
	static bool Merge(MergeInfo &info, idx_t depth, Node *&l_parent, idx_t l_pos);
	//! Returns the size (maximum capacity) of the Node256
	static idx_t GetSize();
};
} // namespace duckdb
