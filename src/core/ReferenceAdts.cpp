#include "lacc/core/ReferenceAdts.hpp"
#include <sstream>

namespace lacc::core {

// string parsing by >
std::vector<std::string> ReferenceResultAggregator::split_path(std::string_view path) {
    std::vector<std::string> parts;
    // TODO: Iterate through the path string.
    // 1. Accumulate characters into a temporary string.
    // 2. When you hit '>', push the temp string to 'parts' and clear it.
    // 3. Don't forget to push the final part after the loop ends!
    std::string temp;
    for (const char c : path) {
        if (c == '>') {
            parts.push_back(temp);
            temp.clear();
            continue;
        }
        temp.push_back(c);
    }

    parts.push_back(temp);
    return parts;
}

// tree construction
void ReferenceResultAggregator::add_leaf_value(std::string_view path, std::int64_t value) {
    auto parts = split_path(path);
    Node* node = &root_;
    
    // TODO: Loop through each part of the path.
    // 1. Update the 'cur_path' string (e.g., "A" then "A>B").  ????
    // 2. Check if the current node has a child with the name parts[i].
    // 3. If not, use std::make_unique<Node>() to create it.
    // 4. Move the 'node' pointer to the child to continue the descent.
    
    // After the loop:
    // 5. Add 'value' to the final node's value and set is_leaf = true.
}


std::int64_t ReferenceResultAggregator::aggregate_rec(Node& n) {
    // Base Case: If the node has no children, just return its current value.
    
    // Recursive Step:
    // 1. Create a sum variable.
    // 2. Loop through n.children (a map of unique_ptrs).
    // 3. Call aggregate_rec on each child and add the result to your sum.
    // 4. Add that total sum to the current node's own value.
    // 5. Return the updated node value.
    if (n.children.empty()) {
        return n.value;
    }

    std::int64_t sum = 0;
    for (const auto& pair : n.children) {
        sum += aggregate_rec(*pair.second);
    }
    n.value += sum;
    return n.value;
}

// look up preorder traversal if this doesn't make sense
void ReferenceResultAggregator::preorder(const Node& n, std::size_t level, std::vector<Row>& out) {
    // 1. If the node is not the "ROOT" placeholder, create a 'Row' object.
    // 2. Fill the Row with n.path, n.name, n.value, etc.
    // 3. Push the Row into the 'out' vector.
    
    // 4. Recursively call preorder for every child in n.children.
    // Logic Hint: If parent is "ROOT", child level is 0. Otherwise, it's level + 1.
    if (n.name != "ROOT") {
        out.push_back(Row{.taxon_path = n.path, .taxon_name = n.name, .value = n.value, .level = level, .is_leaf = n.is_leaf});
    }
    for (const auto& pair : n.children) {
        preorder(*pair.second, level + 1, out);
    }
}

// Wrapper functions provided to the students
void ReferenceResultAggregator::aggregate() {
    (void)aggregate_rec(root_);
}

std::vector<IResultAggregator::Row> ReferenceResultAggregator::to_rows_preorder() const {
    std::vector<Row> rows;
    rows.reserve(1024);
    preorder(root_, 0, rows);
    return rows;
}

} // namespace lacc::core
