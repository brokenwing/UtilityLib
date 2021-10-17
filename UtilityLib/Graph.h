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

* Property Definition
	is_sparse_graph			//sparse or dense
	is_discrete_idx			//No := idx of Node is 0,1,2,3,... ; Yes := idx of Node is 1,3,9,...
	is_node_addable			//use resize if not addable
	is_node_erasable
	is_edge_erasable
	is_edge_duplicatable
	has_inverse_edge		//be able to find edge to p
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
concept _Is_sparse_graph = std::same_as<std::true_type, typename T::is_sparse_graph >;
template <typename T>
concept _Is_discrete_idx = std::same_as<std::true_type, typename T::is_discrete_idx >;
template <typename T>
concept _Is_dense_graph = !_Is_sparse_graph<T>;
template <typename T>
concept _Is_node_addable = std::same_as<std::true_type, typename T::is_node_addable >;
template <typename T>
concept _Is_edge_erasable = std::same_as<std::true_type, typename T::is_edge_erasable >;
template <typename T>
concept _Has_inverse_edge = std::same_as<std::true_type, typename T::has_inverse_edge >;

template <typename T>
concept _Has_graph_property =
node_type<typename T::Node> && edge_type<typename T::Edge>
&& std::_Is_iterator_v<typename T::iterator_node>
&& std::_Is_iterator_v<typename T::iterator_edge>
&& std::_Is_iterator_v<typename T::const_iterator_node>
&& std::_Is_iterator_v<typename T::const_iterator_edge>
&& _Is_constant_bool<typename T::is_sparse_graph>
&& _Is_constant_bool<typename T::is_discrete_idx>
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
(  _Is_sparse_graph<T> || _Has_dense_graph_interface<T> ) &&
( !_Is_node_addable<T> || requires( T & g )
{
	{		g.AddNode()	}->std::same_as<typename T::Node&>;
} ) &&
( !_Has_inverse_edge<T> || _Has_graph_inverse_edge_interface<T> ) &&
( !_Is_edge_erasable<T> || requires( T & g, typename T::Edge & e )
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



template <graph_type Graph, typename DistanceInfo>
using DistanceVector = std::conditional_t<_Is_discrete_idx<Graph>, LFA::unordered_map<int, DistanceInfo>, LFA::vector<DistanceInfo>>;
}