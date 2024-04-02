#ifndef VERTEX
#define VERTEX

class neighbour_it_t;


/*
// Define the template struct to create a tuple from vertex_property_t types
template<typename Tuple>
struct make_vertex_tuple;

// Define the specialization of make_vertex_tuple for std::tuple
template<typename... Ts>
struct make_vertex_tuple<std::tuple<Ts...>> {
    using type = std::tuple<Ts...>; // Define the resulting tuple type
};*/


template<class GraphSchema>
class vertex {
public:
    //using VertexTuple = typename make_vertex_tuple< vertex_property_t>::type;
    //using PropertyTuple = typename make_vertex_tuple<GraphSchema::vertex_property_t>::type;

    template<typename ID, typename ...PropsType>//TODO not compile part
    vertex(ID newid,): vid(newid),properties()

    /**
     * @brief Returns the immutable user id of the element.
     */
    auto id() const {
        return vid;
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
    template<typename ...PropsType>//TODO not compile part
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
     * @see graph_db::neighbor_it_t
     */
  //  using neighbor_it_t =

        /**
         * @brief Returns begin() and end() iterators to all forward edges from the vertex
         * @return A ranges::subrange(begin(), end()) of a neighbor iterators.
         * @see graph_db::neighbor_it_t
         */
     //   std::ranges::subrange<neighbor_it_t> edges() const;
private:
    // vertex_user_id_t id;
    GraphSchema::vertex_user_id_t vid;
    GraphSchema::vertex_property_t properties;
    //std::vector<GraphSchema::neighbour_it_t> neighbours;
};
#endif // !VERTEX
