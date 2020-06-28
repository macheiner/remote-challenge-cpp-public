#pragma once

#include <vector>
#include <utility>
#include <algorithm>

#include "point.h"


template <typename Cont, typename CIter>
typename Cont::iterator remove_constness(Cont& c, CIter it)
{
	return c.erase(it, it);
}

namespace chal {
	using Points = std::vector<Point>;
	using It = Points::iterator;

	namespace detail {
		// helper to remember the position of a (later invalidated) iterator
		inline void adjust_index(const It& it, ptrdiff_t& pos, const It& where, ptrdiff_t diff)
		{
			if (it > where) {
				assert(pos > diff);
				pos -= diff;
			}
		}
	}

	// remove (first..last) from container
	It remove_elements(Points& points, const It& first, const It& last, It& it1, It& it2)
	{
		auto begin = points.begin();
		auto pos1 = std::distance(begin, it1), pos2 = std::distance(begin, it2);

		if (first < last) {
			detail::adjust_index(it1, pos1, first, std::distance(first, last) - 1);
			detail::adjust_index(it2, pos2, first, std::distance(first, last) - 1);
			It ret = points.erase(first + 1, last);
			it1 = begin + pos1;
			it2 = begin + pos2;
			return ret;
		}

		It ret;
		if (first + 1 != points.end())
			ret = points.erase(first + 1, points.end());
		if (last != begin) {
			detail::adjust_index(it1, pos1, last, std::distance(begin, last));
			detail::adjust_index(it2, pos2, last, std::distance(begin, last));
			ret = points.erase(begin, last);
			begin = points.begin();
			it1 = begin + pos1;
			it2 = begin + pos2;
		}
		return ret;
	}

	// helpers for a "wrap-around" iterator
	template<class Cont, class Iter>
	void inc_wrap_around(Cont& container, Iter& it)
	{
		if (++it == container.end())
			it = container.begin();
	}
	template<class Cont, class Iter>
	void dec_wrap_around(Cont& container, Iter& it)
	{
		if (it == container.begin())
			it = container.end() - 1;
		else
			--it;
	}
	template<class Cont, class Iter>
	void op_wrap_around(Cont& container, Iter& it, bool is_ctr_clockwise)
	{
		is_ctr_clockwise ? inc_wrap_around(container, it) : dec_wrap_around(container, it);
	}

	// find left- and rightmost index
	inline auto get_left_right_index(const Points& poly)
	{
		return std::minmax_element(poly.begin(), poly.end(), [](const auto& lhs, const auto& rhs) {
			if (lhs.x < rhs.x)
				return true;
			if (lhs.x > rhs.x)
				return false;
			return lhs.y < rhs.y;
		});
	}

	// test if 'poly' is x-monotone
	// PRE: is_simple_polygon(poly)
	bool is_x_monotone(const Points& poly)
	{
		auto [l, r] = get_left_right_index(poly);

		// check lower half
		for (auto it = l, it2 = it; it != r; it = it2) {
			inc_wrap_around(poly, it2);
			if (it2->x < it->x)
				return false;
		}
		// check upper half
		for (auto it = r, it2 = it; it != l; it = it2) {
			inc_wrap_around(poly, it2);
			if (it2->x > it->x)
				return false;
		}

		return true;
	}

	// find top and bottom index
	inline auto get_top_bottom_index(const Points& poly)
	{
		return std::minmax_element(poly.begin(), poly.end(), [](const auto& lhs, const auto& rhs) {
			if (lhs.y < rhs.y)
				return true;
			if (lhs.y > rhs.y)
				return false;
			return lhs.x < rhs.x;
		});
	}

	// test if 'poly' is y-monotone
	// PRE: is_simple_polygon(poly)
	bool is_y_monotone(const Points& poly)
	{
		auto [t, b] = get_top_bottom_index(poly);

		// check left half
		for (auto it = t, it2 = it; it != b; it = it2) {
			inc_wrap_around(poly, it2);
			if (it2->y < it->y)
				return false;
		}
		// check right half
		for (auto it = b, it2 = it; it != t; it = it2) {
			inc_wrap_around(poly, it2);
			if (it2->y > it->y)
				return false;
		}

		return true;
	}


	// base class for creating the x-monotone hull of an y-monotone polygon
	// 
	class HullCheckerBase
	{
	protected:
		Points& poly_;
		It l_, r_; // left- and rightmost point

	public:
		HullCheckerBase(Points& poly) : poly_(poly)
		{
			assert(is_y_monotone(poly));

			auto pr = get_left_right_index(poly);
			l_ = remove_constness(poly, pr.first);
			r_ = remove_constness(poly, pr.second);
		}
		virtual ~HullCheckerBase()
		{}

	protected:
		It& get_it(bool left) noexcept
		{
			return left ? l_ : r_;
		}
		// depending on half and direction, check if 'curr's successor 'next' is inside the hull
		bool is_bad_successor(const Point& curr, const Point& next, bool is_lower_half, bool left) const noexcept
		{
			bool cmp_x = left ? next.x < curr.x : next.x > curr.x;
			if (!cmp_x)
				return false;
			return is_lower_half ? next.y >= curr.y : next.y <= curr.y;
		}

		// customization point for the specific task
		virtual void process_bad_point(It& where, It pre, It last, bool is_lower_half, bool is_ctr_clockwise) = 0;

		// check one half, in one direction
		void check(bool is_lower_half, bool is_ctr_clockwise)
		{
			bool left = is_lower_half == is_ctr_clockwise; // left-to-right, or right-to-left
			auto &first = get_it(left), &last = get_it(!left);
			for (auto it = first, it2 = it; it != last; it = it2) {
				op_wrap_around(poly_, it2, is_ctr_clockwise);
				if (is_bad_successor(*it, *it2, is_lower_half, left))
					process_bad_point(it2, it, last, is_lower_half, is_ctr_clockwise);
			}
		}

	public:
		void create_x_monotone_hull()
		{
			// check lower and upper half, left-to-right and right-to-left
			check(true, true);
			check(true, false);
			check(false, true);
			check(false, false);
		}
	};
} //namespace chal