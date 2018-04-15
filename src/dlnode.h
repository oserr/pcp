/**
 * @file dlnode.h
 *
 * A simple doubly-linked node.
 */

#pragma once

#include <ostream>

template<typename T>
struct DlNode
{
    T value;
    DlNode *prev;
    DlNode *next;

    /**
     * Default ctor. Initalizes everything to default or nullptr.
     */
    DlNode()
        : value(), prev(nullptr), next(nullptr)
    {}

    /**
     * Initializes node with specific value, and sets prev/next to nullptr.
     * @param value The value to initialize the node with.
     */
    DlNode(T value)
        : value(value), prev(nullptr), next(nullptr)
    {}

    /**
     * Initializes node with a value, and with points to neighboring nodes.
     * @param value The value to initialize the node with.
     * @param prev A pointer to the previous node.
     * @param next A pointer to the next node.
     */
    DlNode(T value, DlNode *prev, DlNode *next)
        : value(value), prev(prev), next(next)
    {}

    /* Default behavior for assignment/moves/dtor */
    DlNode(const DlNode &node) = default;
    DlNode(DlNode &&node) = default;
    DlNode& operator=(const DlNode &node) = default;
    DlNode& operator=(DlNode &&node) = default;
    ~DlNode() = default;
};


/**
 * Output stream operator for DlNode.
 * @param os The output stream.
 * @param node The DlNode.
 * @return A reference to the output stream.
 * @details The format is: DLNode(value,prev,next).
 */
template<typename T>
std::ostream&
operator<<(std::ostream &os, const DlNode<T> &node)
{
    os << "DlNode(" << node.value
       << ',' << node.prev
       << ',' << node.next
       << ')';
    return os;
}
