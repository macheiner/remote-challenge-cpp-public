#pragma once

#include <cassert>
#include <exception>
#include <iostream>

#include "point.h"
#include "challenge_utils.h"


namespace chal {

	// PRE: all points already present
	class ExistingPointsHullChecker : public HullCheckerBase
	{
	public:
		ExistingPointsHullChecker(Points& poly) : HullCheckerBase(poly)
		{}
	protected:
		// find the next point after 'curr' with the same x-coordinate as 'prev' and remove everything in between
		void process_bad_point(It& curr, It prev, It last, bool /*is_lower_half*/, bool is_ctr_clockwise) override
		{
			auto it = curr;
			while (1) {
				op_wrap_around(poly_, it, is_ctr_clockwise);
				if (it->x == prev->x) {
					curr = remove_elements(poly_, is_ctr_clockwise ? prev:it, is_ctr_clockwise ? it:prev, l_, r_);
					return;
				}
				if (it == last) { // pre-condition violation
					assert(false);
					return;
				}
			}
		}
	};

	Points challenge_task_1(Points& poly)
	{
		try {
			ExistingPointsHullChecker checker(poly);
			checker.create_x_monotone_hull();
			return poly;
		}
		catch (const std::exception& e) {
			std::cout << "Exception: " << e.what() << '\n';
		}

		return {};
	}
} //namespace chal