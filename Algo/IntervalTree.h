#pragma once

#include <algorithm>
#include <array>
#include <cstdlib>
#include <type_traits>
#include <functional>
#include <vector>
#include <memory>
#include <istream>
#include <queue>
#include "utils.h"


#ifndef _INTERVAL_TREE_H_
#define _INTERVAL_TREE_H_
//This interval tree only works for elements with <, == operators and with
//funtcion "median", which returns median element of type T
template <typename T, size_t N>
class IntervalDimensionalTreeNode:public std::enable_shared_from_this<IntervalDimensionalTreeNode<T,N>>
{
	static_assert (N < 32 && N>0, "In Multi-Dimensional Interval Tree: dimensions above 32 and lower than 1 are not supported");
public:
	//CONVENIENCE TYPE DEFINITIONS
	typedef IntervalDimensionalTreeNode ThisType;
	typedef std::shared_ptr<const ThisType> TPtr;
	typedef std::weak_ptr<const ThisType> WPtr;
	//BASIC TYPE DEFINITIONS
	typedef std::array<T, N> Point;
	typedef std::pair<Point, Point> Ival;
	typedef std::vector<Ival> Ivals;
	//FUNCTION DEFINITIONS
	typedef std::function<Point(const Ival&)> MedianFunction;
	typedef std::function<int64_t(const Ival&)> WeightFunction;
protected:
	
	struct IntervalTreeNodeVals {

		Point p_center_; //point for split
		std::vector<std::shared_ptr<Ival>> intervals_;
		std::array < std::vector<std::weak_ptr<Ival>>, 2*N> sorted_; // 2*c - left for coordinate c, 2*c+1 - right for coordinate c
		std::array<TPtr, 1 << N> children_; //children
		const ThisType* parent_;
	};

	mutable std::shared_ptr<IntervalTreeNodeVals> v_;

	template <typename Retn>
	Retn SecureCall(const Ival &vals, std::function<Retn(const Ival&)> F) const;

	IntervalDimensionalTreeNode(const Ivals &arr, const ThisType* ptr);

private:
	//Points are equal
	static bool Equals(const Point& a, const Point& b);
	
	static int64_t GetChildAddress(const Point& x_center, const Point& p);
	//Point lies in interval when all coordinates lie in corresponding coordinate interval
	static bool PointLiesInInterval(const Point& p, const Ival& i);

	//bitwise 0 - <= on coordinate. 1 - else (>)
	static 	int64_t GetChildAddress(const Point& x_center, const Ival& iv);

	static 	int64_t ChildAddressAndDiff(const Point& x_center, const Point& p, bool &equals);

	static 	int64_t ChildAddressAndDiff(const Point& x_center, const Ival& iv, bool &lies);

	void BuildTree(const Ivals& arr, const ThisType* ptr) const;

	size_t BinaryFind(const Point& p, size_t cx, bool right);
	
public:
	static const MedianFunction Median;
	static const WeightFunction Weight;
	
	
	//prepares for building
	IntervalDimensionalTreeNode();
	//builds tree. O(NlogN)
	IntervalDimensionalTreeNode(const Ivals &arr);
	virtual ~IntervalDimensionalTreeNode();
	//function for intersection
	typename Ivals Find(const Point & point, bool sort);

	//SERIALIZATION BLOCK AND PARSE BLOCK

	static IntervalDimensionalTreeNode Parse(std::istream &istr);
};

#include "IntervalTree.cpp"


#endif