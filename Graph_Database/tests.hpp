#ifndef TESTS_HPP
#define TESTS_HPP

#include <vector>
#include <functional>
#include <type_traits>
#include <cassert>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include "graph_db.hpp"

template<typename ... T>
inline void spoil(T&&...)
{}

template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
inline std::string to_string_internal(const T& val) {
    return std::to_string(val);
}

inline const std::string to_string_internal(const std::string& val) {
    return val;
}

template<typename ...Ts, size_t ...Is>
inline std::string to_string_impl(const std::tuple<Ts ...>& t, std::index_sequence<Is...>) {
    std::stringstream ss;
    ss << "<";
    (((ss << (Is == 0 ? "" : ",") << to_string_internal(std::get<Is>(t)))), ...);
    ss << ">";
    return ss.str();
}

template<typename ...Ts>
inline std::string to_string_internal(const std::tuple<Ts ...>& t) {
    return to_string_impl(t, std::make_index_sequence<std::tuple_size_v<std::tuple<Ts ...>>>{});
}

template <bool IsVertex, class Element>
inline std::string to_string_internal(const Element& e) {
    std::stringstream ss;
    ss << (IsVertex ? "V(" : "E(") << to_string_internal(e.id()) << "):" << to_string_internal(e.get_properties());
    return ss.str();
}

template<class It, class Val>
inline std::string to_string_from_it(It b, const It& e) {
    std::stringstream ss;
    ss << "[";
    std::for_each(b, e, [&ss](const Val& el) {
        ss << to_string_internal(el.id()) << ",";
        });

    std::string str = ss.str();
    str.back() = ']';
    return str;
}

template<class Edge>
inline std::string to_string_edge(const Edge& e) {
    std::stringstream ss;
    ss << "(" << to_string_internal(e.src().id()) << ")->(" << to_string_internal(e.dst().id()) << ")";
    return ss.str();
}

class test_bench {
    template <class Gdb>
    static void print_gdb(Gdb& gdb) {
        std::cout << "-----GDB-----\n";
#ifdef PAIR_INTERFACE
        typename Gdb::vertex_it_t vertexes_begin = gdb.get_vertexes().first;
        typename Gdb::vertex_it_t vertexes_end = gdb.get_vertexes().second;
        std::for_each(vertexes_begin, vertexes_end, [](const typename Gdb::vertex_t& vertex) {
#else
        for (const typename Gdb::vertex_t& vertex : gdb.get_vertexes())
        {
#endif
            std::cout << to_string_internal<true>(vertex) << ":";
            auto [nb, ne] = vertex.edges();
            std::cout << to_string_from_it<typename Gdb::neighbor_it_t, typename Gdb::edge_t>(nb, ne) << "\n";
#ifdef PAIR_INTERFACE
        });
#else
            }
#endif

#ifdef PAIR_INTERFACE
        typename Gdb::edge_it_t edges_b = gdb.get_edges().first;
        typename Gdb::edge_it_t edges_e = gdb.get_edges().second;
        std::for_each(edges_b, edges_e, [](const typename Gdb::edge_t& edge) {
#else
        for (const typename Gdb::edge_t& edge : gdb.get_edges())
        {
#endif
            std::cout << to_string_internal<false>(edge) << ":" << to_string_edge(edge) << "\n";
#ifdef PAIR_INTERFACE
        });
#else
            }
#endif
    }

    static void test_prop_empty() {
        struct gs {
            using vertex_user_id_t = std::string;
            using vertex_property_t = std::tuple<>;

            using edge_user_id_t = float;
            using edge_property_t = std::tuple<>;
        };
        using gdb_t = graph_db<gs>;
        gdb_t gdb;

        auto v1 = gdb.add_vertex("v1");
        auto v2 = gdb.add_vertex("v2");
        auto v3 = gdb.add_vertex("v3");

        auto e12 = gdb.add_edge(1.2, v1, v2);
        auto e23 = gdb.add_edge(2.3, v2, v3);
        auto e31 = gdb.add_edge(3.1, v3, v1);

        spoil(e12, e23, e31);

        print_gdb(gdb);
    }

    static void test_vertex_prop_set() {
        struct gs {
            using vertex_user_id_t = std::string;
            using vertex_property_t = std::tuple<int, double, bool, std::string>;

            using edge_user_id_t = std::string;
            using edge_property_t = std::tuple<double>;
        };
        using gdb_t = graph_db<gs>;
        gdb_t gdb;

        auto v1 = gdb.add_vertex("v1");
        auto v2 = gdb.add_vertex("v2");
        auto v3 = gdb.add_vertex("v3");
        v3.template set_property<0>(3);
        v2.set_properties(2, 2.2, true, "dva");
        v3.template set_property<3>("tri");
        gdb.add_vertex("v4");
        spoil(v1);

        print_gdb(gdb);
    }

    static void test_vertex_prop_set_through_edge() {
        struct gs {
            using vertex_user_id_t = std::string;
            using vertex_property_t = std::tuple<int, double, bool, std::string>;

            using edge_user_id_t = std::string;
            using edge_property_t = std::tuple<double>;
        };
        using gdb_t = graph_db<gs>;
        gdb_t gdb;

        auto v1 = gdb.add_vertex("v1");
        auto v2 = gdb.add_vertex("v2");
        auto e12 = gdb.add_edge("e12", v1, v2);
        e12.dst().set_properties(2, 2.2, true, "-");
        e12.dst().template set_property<3>("dva");
        gdb.add_vertex("v3");
        e12.src().set_properties(0, 3.3, true, "tri");
        e12.src().template set_property<0>(3);
        gdb.add_vertex("v4");

        print_gdb(gdb);
    }

    static void test_edge_prop_set() {
        struct gs {
            using vertex_user_id_t = std::string;
            using vertex_property_t = std::tuple<int>;

            using edge_user_id_t = std::string;
            using edge_property_t = std::tuple<int, double, bool, std::string>;
        };
        using gdb_t = graph_db<gs>;
        gdb_t gdb;

        auto v1 = gdb.add_vertex("v1");
        auto v2 = gdb.add_vertex("v2");
        auto v3 = gdb.add_vertex("v3");
        auto v4 = gdb.add_vertex("v4");

        auto e12 = gdb.add_edge("e12", v1, v2);
        auto e23 = gdb.add_edge("e23", v2, v3, 23, 2.3, true, "p23");
        auto e34 = gdb.add_edge("e34", v3, v4);
        auto e41 = gdb.add_edge("e41", v4, v1);
        e41.template set_property<0>(41);
        e34.set_properties(34, 3.4, true, "p34");
        e41.template set_property<3>("p41");
        spoil(e12, e23);

        print_gdb(gdb);
    }

    static void test_edge_prop_set_through_neighbors() {
        struct gs {
            using vertex_user_id_t = std::string;
            using vertex_property_t = std::tuple<int>;

            using edge_user_id_t = std::string;
            using edge_property_t = std::tuple<int, double, bool, std::string>;
        };
        using gdb_t = graph_db<gs>;
        gdb_t gdb;

        auto v1 = gdb.add_vertex("v1");
        auto v2 = gdb.add_vertex("v2");
        auto v3 = gdb.add_vertex("v3");
        auto v4 = gdb.add_vertex("v4");

        auto e12 = gdb.add_edge("e12", v1, v2);
#ifdef PAIR_INTERFACE
        (*v1.edges().first)
#else
        (*v1.edges().begin())
#endif
            .set_properties(12, 1.2, true, "p12");
        auto e23 = gdb.add_edge("e23", v2, v3);
#ifdef PAIR_INTERFACE
        (*v2.edges().first)
#else
        (*v2.edges().begin())
#endif
            .template set_property<0>(23);
        auto e34 = gdb.add_edge("e34", v3, v4);
#ifdef PAIR_INTERFACE
        (*v3.edges().first)
#else
        (*v3.edges().begin())
#endif
            .template set_property<3>("p34");
        spoil(e12, e23, e34);

        print_gdb(gdb);
    }

    static void check_types() {
        struct gs {
            using vertex_user_id_t = std::string;
            using vertex_property_t = std::tuple<int>;

            using edge_user_id_t = std::string;
            using edge_property_t = std::tuple<int, double, bool, std::string>;
        };
        using gdb_t = graph_db<gs>;
        gdb_t gdb;

        auto v1 = gdb.add_vertex("v1");
        auto v2 = gdb.add_vertex("v2");
        auto e12 = gdb.add_edge("e12", v1, v2);

        static_assert(std::is_same_v<decltype(v1), typename gdb_t::vertex_t>, "Wrong vertex type from add_vertex()");
        static_assert(std::is_same_v<std::decay_t<decltype(
#ifdef PAIR_INTERFACE
            * v1.edges().first
#else
            * v1.edges().begin()
#endif
            )>, typename gdb_t::edge_t>,
            "Wrong edge type from neighbor_it_t()");
        static_assert(std::is_same_v<std::decay_t<decltype(
#ifdef PAIR_INTERFACE
            * gdb.get_vertexes().first
#else
            * gdb.get_vertexes().begin()
#endif
            )>, typename gdb_t::vertex_t>,
            "Wrong vertex type from get_vertexes()");

        static_assert(std::is_same_v<decltype(e12), typename gdb_t::edge_t>, "Wrong edge type from add_edge()");
        static_assert(std::is_same_v<std::decay_t<decltype(
#ifdef PAIR_INTERFACE
            * gdb.get_edges().first
#else
            * gdb.get_edges().begin()
#endif
            )>, typename gdb_t::edge_t>,
            "Wrong edge type from get_edges()");
        static_assert(std::is_same_v<decltype(e12.src()), typename gdb_t::vertex_t>, "Wrong vertex type from src()");
        static_assert(std::is_same_v<decltype(e12.dst()), typename gdb_t::vertex_t>, "Wrong vertex type from dst()");

        spoil(e12);
    }

    static void check_iterators() {
        struct gs {
            using vertex_user_id_t = std::string;
            using vertex_property_t = std::tuple<int>;

            using edge_user_id_t = std::string;
            using edge_property_t = std::tuple<int, double, bool, std::string>;
        };
        using gdb_t = graph_db<gs>;
        gdb_t gdb;

        auto v1 = gdb.add_vertex("v1");
        auto v2 = gdb.add_vertex("v2");
        auto e12 = gdb.add_edge("e12", v1, v2);

        auto [vb, ve] = gdb.get_vertexes();
        assert(vb != ve);
        auto [nb, ne] = (*vb).edges();
        ++vb;
        vb++;

        assert(nb != ne);
        auto nb2 = nb;
        assert((*nb).src().id() == v1.id());
        ++nb;
        nb2++;

        auto [eb, ee] = gdb.get_edges();
        auto eb2 = eb;
        assert((*eb).src().id() == v1.id());
        ++eb;
        eb2++;

        spoil(e12);
    }

    class test_example {
        struct gs {
            using vertex_user_id_t = std::string;
            using vertex_property_t = std::tuple<std::string, int, double, char>;

            using edge_user_id_t = float;
            using edge_property_t = std::tuple<std::string, bool>;
        };
        using gdb_t = graph_db<gs>;
        gdb_t gdb;

        void fill() {
            typename gdb_t::vertex_t v1 = gdb.add_vertex("prvni");
            v1.set_properties("str1", 1, 1, false);
            auto v2 = gdb.add_vertex("druhy");
            v2.set_property<2>(2.2);
            auto v3 = gdb.add_vertex("treti", "str3", 3, 3.3, 3);
            gdb.add_vertex("ctvrty");

            typename gdb_t::edge_t e1 = gdb.add_edge(1.2, v1, v2);
            e1.set_properties("str1", false);
            gdb.add_edge(2.3, v2, v3, "str2", true);
            auto e3 = gdb.add_edge(1.3, v1, v3);
            e3.set_property<1>(true);

            // A simple check that storage is columnar & get_property doesn't return a copy.
            assert(&v1.template get_property<0>() + 1 == &v2.template get_property<0>());
        }

        void print() {
            std::cout << "Graph:\n";
            auto [vertexes_begin, vertexes_end] = gdb.get_vertexes();
            static_assert(std::is_same_v<decltype(vertexes_begin), typename gdb_t::vertex_it_t>,
                "Wrong vertex iterator type");
            std::for_each(vertexes_begin, vertexes_end, [](const typename gdb_t::vertex_t& vertex) {
                std::cout << "(" << vertex.id() << ") with p1=='" << vertex.template get_property<0>()
                    << "', p2=='" << std::get<1>(vertex.get_properties()) << "'\n";
                auto [neigbor_edges_begin, neighbor_edges_end] = vertex.edges();
                static_assert(std::is_same_v<decltype(neigbor_edges_begin), typename gdb_t::neighbor_it_t>,
                    "Wrong neighbor iterator type");
                std::for_each(neigbor_edges_begin, neighbor_edges_end, [&vertex](auto&& edge) {
                    assert(edge.src().id() == vertex.id());
                    std::cout << "  (" << edge.src().id() << ")-[" << edge.id() << "]->("
                        << edge.dst().id() << ") with p1=='" << std::get<0>(edge.get_properties())
                        << "', p2=='" << edge.template get_property<1>() << "'\n";
                    });
                });
        }

        void clear() {
            // Clear
            auto [vertexes_begin, vertexes_end] = gdb.get_vertexes();
            std::for_each(vertexes_begin, vertexes_end, [](auto&& vertex) {
                vertex.set_properties("", 0, 0, 0);
                vertex.template set_property<0>(" ");
                auto [neigbor_edges_begin, neighbor_edges_end] = vertex.edges();
                std::for_each(neigbor_edges_begin, neighbor_edges_end, [&vertex](auto&& edge) {
                    assert(edge.src().id() == vertex.id());
                    edge.set_properties("", false);
                    edge.template set_property<0>(" ");
                    });
                });
        }

    public:
        void run() {
            fill();
            print();
            clear();
            print();
        }
    };

    class test_algorithms {
        struct gs {
            using vertex_user_id_t = size_t;
            using vertex_property_t = std::tuple<int, bool>;

            using edge_user_id_t = size_t;
            using edge_property_t = std::tuple<int, bool>;
        };
        using gdb_t = graph_db<gs>;
        gdb_t gdb;

        using v_t = typename gdb_t::vertex_t;
        using e_t = typename gdb_t::edge_t;

        std::vector<v_t> vertices;
        std::vector<e_t> edges;

        void fill(size_t n, size_t offset, size_t neighbor_count = 2, size_t stride = 1) {
            for (std::size_t i = 0; i < n; ++i) {
                vertices.push_back(gdb.add_vertex(i, -1, false));
            }

            auto [vb, ve] = gdb.get_vertexes();
            size_t eid = 0;
            size_t v = 0;
            for (auto&& v1 : vertices) {
                for (std::size_t i = 0; i < neighbor_count; ++i) {
                    edges.push_back(gdb.add_edge(eid++, v1, vertices[(v++ + offset + i * stride) % vertices.size()], -1,
                        false));
                }
            }
            spoil(vb, ve);
        }

        void bfs(const v_t& v) {
            static constexpr size_t prop_depth = 0;
            static constexpr size_t prop_seen = 1;

            std::queue<v_t> q;
            q.push(v);
            q.back().template set_property<prop_depth>(0);
            while (!q.empty()) {
                auto&& vv = q.front();
                vv.template set_property<prop_seen>(true);
                size_t depth = vv.template get_property<prop_depth>() + 1;
                auto [nb, ne] = vv.edges();
                q.pop();
                for (; nb != ne; nb++) {
                    auto&& e = *nb;
                    auto dst = e.dst();
                    assert(!e.template get_property<prop_seen>());
                    e.template set_property<prop_seen>(true);
                    if (!dst.template get_property<prop_seen>()) {
                        dst.template set_property<prop_seen>(true);
                        assert(dst.template get_property<prop_depth>() == -1);
                        dst.template set_property<prop_depth>(depth);
                        q.push(std::move(dst));
                    }
                }
            }
        }

    public:
        void run() {
            fill(10, 5);
            print_gdb(gdb);
            bfs(vertices[4]);
            print_gdb(gdb);
        }
    };

    std::vector<std::function<void()>> tests;
public:
    test_bench() {
        tests.push_back(check_types);
        tests.push_back(check_iterators);
        tests.push_back(test_prop_empty);
        tests.push_back(test_vertex_prop_set);
        tests.push_back(test_vertex_prop_set_through_edge);
        tests.push_back(test_edge_prop_set);
        tests.push_back(test_edge_prop_set_through_neighbors);
        tests.push_back([]() { test_algorithms t; t.run(); });
        tests.push_back([]() { test_example t; t.run(); });
    }

    void run_test(size_t i) const {
        tests[i]();
    }

    size_t test_count() const { return tests.size(); }

    void run_all_tests() const {
        for (auto&& t : tests) {
            t();
        }
    }
};

#endif //TESTS_HPP
