#pragma once

#include <cmath>
#include <cassert>
#include <exception>
#include <iostream>

#include "point.h"
#include "challenge_utils.h"


namespace chal {

	class HullChecker : public HullCheckerBase
	{
	public:
		HullChecker(Points& poly) : HullCheckerBase(poly)
		{}
	protected:
		// find the next point after 'curr' with the x-coordinate >= 'prev',
		// create the point if not existing and remove everything in between
		void process_bad_point(It& curr, It prev, It last, bool is_lower_half, bool is_ctr_clockwise) override
		{
			bool left = is_lower_half == is_ctr_clockwise; // left-to-right, or right-to-left
			auto it = curr;
			for (int n = 0; ; ++n) {
				op_wrap_around(poly_, it, is_ctr_clockwise);
				if (it->x == prev->x) { // point already exists
					curr = remove_elements(poly_, is_ctr_clockwise ? prev:it, is_ctr_clockwise ? it:prev, l_, r_);
					break;
				} else if (left ? it->x > prev->x : it->x < prev->x) { // "create" the point
					curr = it;
					// re-use the predecessor (no need to create a new point)
					op_wrap_around(poly_, curr, !is_ctr_clockwise);
					curr->y += (it->y - curr->y) / std::fabs(it->x - curr->x) * std::fabs(prev->x - curr->x);
					curr->x = prev->x;
					// re-set 'curr'
					curr = it;
					if (n > 0) {
						//@@FIXME: remove points
					}
					return;
				}
				if (it == last) { // prev-condition violation
					assert(false);
					return;
				}
			}
		}
	};

	Points challenge_task_2(Points& poly)
	{
		try {
			HullChecker checker(poly);
			checker.create_x_monotone_hull();
			return poly;
		}
		catch (const std::exception& e) {
			std::cout << "Exception: " << e.what() << '\n';
		}

		return {};
	}

} //namespace chal