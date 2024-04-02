#ifndef GRAPH_DB_HPP
#define GRAPH_DB_HPP
#include "vertex_class.hpp"
#include "edge_class.hpp"
#include "vertex_edge_iterators.hpp"

struct gs {
	using vertex_user_id_t = std::string;
	using vertex_property_t = std::tuple<int, double, bool, std::string>;

	using edge_user_id_t = std::string;
	using edge_property_t = std::tuple<double>;
};




template<typename T>
struct add_vector {
	using type = std::vector<T>;
};

template<typename Tuple, template<typename> typename F>
struct type_transform;
template<template<typename P> class F, typename ... L>
struct type_transform<std::tuple<L ...>, F>
{
	using type = std::tuple<F<L>...>;
};

template<typename Tuple, template<typename> typename F>
using type_transform_t = typename type_transform<Tuple, F>::type;
template<typename T>
using add_vector_t = add_vector<T>::type;


//struct add_vector { template< typename T> using type = std::vector< T>; };


/**
 * @brief A graph database that takes its schema (types and number of vertex/edge properties, user id types) from a given trait
 * @tparam GraphSchema A trait which specifies the schema of the graph database.
 * @see graph_schema
 */
template<class GraphSchema>
class graph_db {
public:

	using vertex_properties_t = type_transform_t<vertex_property_t, add_vector_t>;
	using edge_properties_t = type_transform_t<edge_property_t, add_vector_t>;
	//int lastIndex = 0;

	/**
	 * @brief A type representing a vertex.
	 * @see vertex
	 */
	using vertex_t = vertex<GraphSchema>;
		/**
		 * @brief A type representing an edge.
		 * @see edge
		 */
	using edge_t = edge<GraphSchema>;

		/**
		 * @brief A type representing a vertex iterator. Must be at least of output iterator. Returned value_type is a vertex.
		 * @note Iterate in insertion order.
		 */
		using vertex_it_t = vertex_it_t_class<GraphSchema>;

		/**
		 * @brief A type representing a edge iterator. Must be at least an output iterator. Returned value_type is an edge.
		 * @note Iterate in insertion order.
		 */
		using edge_it_t = edge_it_t_class<GraphSchema>;

		/**
		 * @brief A type representing a neighbor iterator. Must be at least an output iterator. Returned value_type is an edge.
		 * @note Iterate in insertion order.
		 */
		using neighbor_it_t = edge_it_t; //TODO oddìlenej iterator

		/**
		 * @brief Insert a vertex into the database.
		 * @param vuid A user id of the newly created vertex.
		 * @return The newly created vertex.
		 * @note The vertex's properties have default values.
		 */
		vertex_t add_vertex(typename GraphSchema::vertex_user_id_t&& vuid)
		{
			vertex_ids.push_back(vuid);

		}
		vertex_t add_vertex(const typename GraphSchema::vertex_user_id_t& vuid)
		{

	}

	/**
	 * @brief Insert a vertex into the database with given values of the vertex's properties.
	 * @tparam Props All types of properties.
	 * @param vuid A user id of the newly created vertex.
	 * @param props Properties of the new vertex.
	 * @return The newly created vertex.
	 * @note Should not compile if not provided with all properties.
	 */
	template<typename ...Props>
	vertex_t add_vertex(typename GraphSchema::vertex_user_id_t&& vuid, Props &&...props);
	template<typename ...Props>
	vertex_t add_vertex(const typename GraphSchema::vertex_user_id_t& vuid, Props &&...props);

	/**
	 * @brief Returns begin() and end() iterators to all vertexes in the database.
	 * @return A ranges::subrange(begin(), end()) of vertex iterators.
	 */
	std::ranges::subrange<vertex_it_t> get_vertexes() const;

	/**
	 * @brief Insert a directed edge between v1 and v2 with a given user id.
	 * @param euid An user id of the edge.
	 * @param v1 A source vertex of the edge.
	 * @param v2 A destination vertex of the edge.
	 * @return The newly create edge.
	 * @note The edge's properties have default values.
	 */
	edge_t add_edge(typename GraphSchema::edge_user_id_t&& euid, const vertex_t& v1, const vertex_t& v2);
	edge_t add_edge(const typename GraphSchema::edge_user_id_t& euid, const vertex_t& v1, const vertex_t& v2);

	/**
	 * @brief Insert a directed edge between v1 and v2 with a given user id and given properties.
	 * @tparam Props Types of properties
	 * @param euid An user id of the edge.
	 * @param v1 A source vertex of the edge.
	 * @param v2 A destination vertex of the edge.
	 * @param props All properties of the edge.
	 * @return The newly create edge.
	 * @note Should not compile if not provided with all properties.
	 */
	template<typename ...Props>
	edge_t add_edge(typename GraphSchema::edge_user_id_t&& euid, const vertex_t& v1, const vertex_t& v2, Props &&...props);
	template<typename ...Props>
	edge_t add_edge(const typename GraphSchema::edge_user_id_t& euid, const vertex_t& v1, const vertex_t& v2, Props &&...props);

	/**
	 * @brief Returns begin() and end() iterators to all edges in the database.
	 * @return A ranges::subrange(begin(), end()) of edge iterators.
	 */
	std::ranges::subrange<edge_it_t> get_edges() const;
private:
	//std::vector<vertex_t> vertices_;
//	std::vector<edge_t> edges_;

	std::vector<vertex_user_id_t> vertex_ids;
	std::vector<edge_user_id_t> edge_ids;
	vertex_properties_t vertex_properties;
	edge_properties_t edge_properties;
};

#endif //GRAPH_DB_HPP
