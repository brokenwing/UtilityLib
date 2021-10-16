#pragma once
#include "pch.h"
#include "CommonDef.h"

/*	Goal: Be able to choose the most effecient Graph according to the needs
*	Define Graph Interface via template
*/
namespace Util::GraphTheory
{
/*
* Goal: Be able to choose the most effecient Graph according to the needs
*********	Standard Graph
* Common Type
Node
Edge
iterator_node
iterator_edge
const_iterator_node
const_iterator_edge

* Common Interface
iterator_node begin_node();
iterator_node end_node();
const_iterator_node cbegin_node()const;
const_iterator_node cend_node()const;
iterator_edge begin_edge();
iterator_edge end_edge();
const_iterator_edge cbegin_edge()const;
const_iterator_edge cend_edge()const;

bool HasNode(int idx)const;
Node* GetNode(int idx);
const Node* GetNode(int idx)const;
Edge& AddEdge(s,t);
Edge* GetNextEdge(Node&);
const Edge* GetNextEdge(Node&)const;
Edge* GetNextEdge(Edge&);
const Edge* GetNextEdge(Edge&)const;
size_t CountOutDegree(Node&)const;

size_t size_node()const;
size_t size_edge()const;

clear();
reserve(size_t n, size_t m); //node and edge (not necessary implemented)
resize(size_t n); //node

* Inverse Edge Option
Edge* GetNextInverseEdge(Node&);
const Edge* GetNextInverseEdge(Node&)const;
Edge* GetNextInverseEdge(Edge&);
const Edge* GetNextInverseEdge(Edge&)const;
size_t CountInDegree(Node&)const;

* Edge Random Access Option
Edge* GetEdge(int idx);
const Edge* GetEdge(int idx)const;
Edge* GetEdge(int st,int ed);
const Edge* GetEdge(int st, int ed)const;

* Add Option
Node& AddNode();

* Erase Option
void EraseNode(Node&);
void EraseEdge(Edge&);
*/

//Base class of Node
class BasicNode
{
protected:
	int idx = -1;
public:
	friend void _Reset_idx( BasicNode&, const int );
	BasicNode()
	{}
	int GetIdx()const noexcept	{		return idx;	}
	bool valid()const noexcept	{		return idx >= 0;	}
};
//Node class with weight
template <typename WeightType = int>
class WeightedNode :public BasicNode
{
public:
	using weight_type = WeightType;
protected:
	WeightType w = WeightType();
public:
	WeightedNode() :BasicNode()
	{}
	const WeightType& GetWeight()const noexcept	{		return w;	}
	WeightType& GetWeight() noexcept	{		return w;	}
};
//Base class of Edge
class BasicEdge
{
protected:
	int idx = -1;
	int source = -1;
	int destination = -1;
public:
	friend void _Reset_idx( BasicEdge&, const int );
	friend void _Reset_source( BasicEdge&, const int );
	friend void _Reset_destination( BasicEdge&, const int );
	BasicEdge()
	{}
	int GetIdx()const noexcept	{		return idx;	}
	int GetSource()const noexcept	{		return source;	}
	int GetDestination()const noexcept	{		return destination;	}
	bool valid()const noexcept	{		return idx >= 0 && source >= 0 && destination >= 0;	}
};
//Edge class with weight
template <typename WeightType = int>
class WeightedEdge :public BasicEdge
{
public:
	using weight_type = WeightType;
protected:
	WeightType w = WeightType();
public:
	WeightedEdge() :BasicEdge()
	{}
	const WeightType& GetWeight()const noexcept	{		return w;	}
	WeightType& GetWeight() noexcept	{		return w;	}
};

//Reset Private Member

inline void _Reset_idx( BasicNode& val, const int idx ){	val.idx = idx;}
inline void _Reset_idx( BasicEdge& val, const int idx ){	val.idx = idx;}
inline void _Reset_source( BasicEdge& val, const int idx ){	val.source = idx;}
inline void _Reset_destination( BasicEdge& val, const int idx ){	val.destination = idx;}

//Concept

template <typename T>
concept node_type = std::is_base_of_v<BasicNode, T>;
template <typename T>
concept edge_type = std::is_base_of_v<BasicEdge, T>;

template <typename T>
concept weighted_node_type = node_type<T> && std::is_base_of_v<WeightedNode<typename T::weight_type>, T>;
template <typename T>
concept weighted_edge_type = edge_type<T> && std::is_base_of_v<WeightedEdge<typename T::weight_type>, T>;

template <typename T>
concept _Is_constant_bool = std::same_as<std::true_type, T > || std::same_as<std::false_type, T >;

template <typename T>
concept _Has_graph_property =
node_type<typename T::Node> && edge_type<typename T::Edge>
&& std::_Is_iterator_v<typename T::iterator_node>
&& std::_Is_iterator_v<typename T::iterator_edge>
&& std::_Is_iterator_v<typename T::const_iterator_node>
&& std::_Is_iterator_v<typename T::const_iterator_edge>
&& _Is_constant_bool<typename T::is_sparse_graph>
&& _Is_constant_bool<typename T::is_node_addable>
&& _Is_constant_bool<typename T::is_node_erasable>
&& _Is_constant_bool<typename T::is_edge_erasable>
&& _Is_constant_bool<typename T::is_edge_duplicatable>
&& _Is_constant_bool<typename T::has_inverse_edge>;

template <typename T>
concept _Has_graph_node_and_edge_begin_end = 
requires( T & g )//iterator check
{
	{		g.begin_node()	}->std::same_as<typename T::iterator_node>;
	{		g.end_node()	}->std::same_as<typename T::iterator_node>;
	{		g.begin_edge()	}->std::same_as<typename T::iterator_edge>;
	{		g.end_edge()	}->std::same_as<typename T::iterator_edge>;
}&&
requires( const T& g )//const_iterator check
{
	{		g.begin_node()	}->std::same_as<typename T::const_iterator_node>;
	{		g.end_node()	}->std::same_as<typename T::const_iterator_node>;
	{		g.begin_edge()	}->std::same_as<typename T::const_iterator_edge>;
	{		g.end_edge()	}->std::same_as<typename T::const_iterator_edge>;
};

template <typename T>
concept _Has_graph_common_interface =
_Has_graph_node_and_edge_begin_end<T> &&
requires( T& g, size_t n, int idx, const typename T::Node& p, const typename T::Edge& e )
{
	T();
	{		g.clear()				}->std::same_as<void>;
	{		g.reserve( n, n )		}->std::same_as<void>;
	{		g.resize( n )			}->std::same_as<void>;
	{		g.GetNode( idx )		}->std::same_as<typename T::Node*>;
	{		g.AddEdge( idx, idx )	}->std::same_as<typename T::Edge&>;
	{		g.GetNextEdge( p )		}->std::same_as<typename T::Edge*>;
	{		g.GetNextEdge( e )		}->std::same_as<typename T::Edge*>;
}&&
requires( const T& g, int idx, const typename T::Node& p, const typename T::Edge& e )
{
	{		g.size_node()			}->std::same_as<size_t>;
	{		g.size_edge()			}->std::same_as<size_t>;
	{		g.HasNode( idx )		}->std::same_as<bool>;
	{		g.GetNode( idx )		}->std::same_as<const typename T::Node*>;
	{		g.GetNextEdge( p )		}->std::same_as<const typename T::Edge*>;
	{		g.GetNextEdge( e )		}->std::same_as<const typename T::Edge*>;
	{		g.CountOutDegree( p )	}->std::same_as<size_t>;
};

template <typename T>
concept _Has_dense_graph_interface = 
requires( T & g, int idx )
{
	{		g.GetEdge( idx, idx )	}->std::same_as<typename T::Edge*>;
}&& 
requires( const T& g, int idx )
{
	{		g.GetEdge( idx, idx )	}->std::same_as<const typename T::Edge*>;
};

template <typename T>
concept _Has_graph_inverse_edge_interface =
requires( const T & g, const typename T::Node & p, const typename T::Edge & e )
{
	{		g.GetNextInverseEdge( p )		}->std::same_as<const typename T::Edge*>;
	{		g.GetNextInverseEdge( e )		}->std::same_as<const typename T::Edge*>;
	{		g.CountInDegree( p )			}->std::same_as<size_t>;
}&&
requires( T & g, const typename T::Node & p, const typename T::Edge & e )
{
	{		g.GetNextInverseEdge( p )		}->std::same_as<typename T::Edge*>;
	{		g.GetNextInverseEdge( e )		}->std::same_as<typename T::Edge*>;
};

template <typename T>
concept _Has_graph_optional_interface =
( std::same_as<typename T::is_sparse_graph, std::true_type> || _Has_dense_graph_interface<T> ) &&
( std::same_as<typename T::is_node_addable, std::false_type> || requires( T & g )
{
	{		g.AddNode()	}->std::same_as<typename T::Node&>;
} ) &&
( std::same_as<typename T::has_inverse_edge, std::false_type> || _Has_graph_inverse_edge_interface<T> ) &&
( std::same_as<typename T::is_edge_erasable, std::false_type> || requires( T & g, typename T::Edge & e )
{
	{		g.EraseEdge( e )	}->std::same_as<void>;
});


template <typename T>
concept graph_type = _Has_graph_property<T> && _Has_graph_common_interface<T> && _Has_graph_optional_interface<T>;

//Self Check

static_assert( node_type<BasicNode>,"BasicNode Check Fail" );
static_assert( node_type<WeightedNode<>>,"WeightedNode Check Fail" );
static_assert( edge_type<BasicEdge>,"BasicEdge Check Fail" );
static_assert( edge_type<WeightedEdge<>>,"WeightedEdge Check Fail" );
}