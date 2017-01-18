#ifndef TRANSFORMATION_
#define TRANSFORMATION_

#include "settings.hpp"

namespace cube {
	struct transformation {
		/* Default constructor - creates empty transformation. */
		transformation() { clear(); }

		/* Copy & move constuctors */
		transformation(const sfi & swap, const dimension_array & permutation) : dim_swap(swap), dim_permutation(permutation) {}
		transformation(sfi && swap,dimension_array && permutation) : dim_swap(swap), dim_permutation(permutation) {}

		/* Resets the permutation*/
		void clear() {
			FOR_DIMENSION(dim_id) {
				dim_permutation[dim_id] = DIMENSION_BITS;
			}
		}
		/* Returns if dim_permutation is completely set. */
		bool is_unique() {
			FOR_DIMENSION(dim_id) {
				if (one_cnt[dim_permutation[dim_id]] != 1)
					return false;
			}
			return true;
		}
		
		//i-th bit is set to one iff i-th coordinate is swapped
		sfi dim_swap; 

		//Set of possible permutations for all coordinates.
		dimension_array dim_permutation;
	};
}
#endif //TRANSFORMATION_