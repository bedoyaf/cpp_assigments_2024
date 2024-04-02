#ifndef EDGE
#define EDGE

template<class GraphSchema>
class vertex;

template<class GraphSchema>
class edge {
public:
    /**
     * @brief Returns the immutable user id of the element.
     */
    auto id() const
    {
        return eid;
    }

    /**
     * @brief Returns all immutable properties of the element in tuple.
     * @note The return type is GraphSchema::vertex_property_t for vertexes and GraphSchema::edge_property_t for edges.
     */
    auto get_properties() const
    {
        return properties;
    }

    /**
     *
     * @brief Returns a single immutable property of the I-th element.
     * @tparam I An index of the property.
     * @return The value of the property.
     * @note The first property is on index 0.
     */
    template<size_t I>
    decltype(auto) get_property() const
    {
        return std::get<I>(properties);
    }

    /**
     * @brief Sets the values of properties of the element.
     * @tparam PropsType Types of the properties.
     * @param props The value of each individual property.
     * @note Should not compile if not provided with all properties.
     */
    template<typename ...PropsType>
    void set_properties(PropsType &&...props)
    {
        properties = std::make_tuple(std::forward<PropsType>(props)...);
    }

    /**
     * @brief Set a value of the given property of the I-th element
     * @tparam I An index of the property.
     * @tparam PropType The type of the property.
     * @param prop The new value of the property.
     * @note The first property is on index 0.
     */
    template<size_t I, typename PropType>
    void set_property(const PropType& prop)
    {
        std::get<I>(properties) = prop;
    }

    /**
     * @brief Returns the source vertex of the edge.
     * @return The vertex.
     */
    auto src() const
    {
        return src_;
    }

    /**
     * @brief Returns the destination vertex of the edge.
     * @return The vertex.
     */
    auto dst() const
    {
        return dst_;
    }
private:
    // vertex_user_id_t id;
    GraphSchema::edge_user_id_t eid;
    GraphSchema::edge_property_t properties;
    vertex src_;
    vertex dst_;
};
#endif // !EDGE
