#ifndef ITERATOR
#define ITERATOR

template<class GraphSchema>
class vertex_it_t_class {
private:
    size_t index;
public:
    // Constructor
    vertex_it_t_class(size_t index) : index(index) {}

    // Dereference operator
    auto operator*() const {
        // Return the current index
        return index;
    }

    // Pre-increment operator
    vertex_it_t_class& operator++() {
        ++index; // Increment the index
        return *this;
    }

    // Post-increment operator
    vertex_it_t_class operator++(int) {
        vertex_it_t_class temp = *this; // Create a copy of the current iterator
        ++(*this); // Increment the index
        return temp; // Return the copy of the original iterator
    }

    // Equality comparison operator
    bool operator==(const vertex_it_t_class& other) const {
        return index == other.index;
    }

    // Inequality comparison operator
    bool operator!=(const vertex_it_t_class& other) const {
        return !(*this == other);
    }
};



#endif // !ITERATOR
