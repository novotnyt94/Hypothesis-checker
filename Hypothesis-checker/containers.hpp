#ifndef CONTAINERS_
#define CONTAINERS_

#include "settings.hpp"
#include "errors.hpp"
#include <vector>
#include <algorithm>

namespace cube {
	//The size of the container is reduced in DEBUG mode due to memory consumption
#ifdef _DEBUG
	static const llfi PRIME_MOD = 100267;
#else
	static const llfi PRIME_MOD = 10000357;
#endif //_DEBUG

	/* User-defined mini-container made for holding generated matchings in very comprimed state to spare memory consumption*/
	class result_set {
	public:
		/* Default constructor */
		result_set() : data(std::vector<std::vector<llfi>>(PRIME_MOD)) {}

		/* Tries to insert given 64-bit number (comprimed matching) into container. 
		   Returns true if the element was not there, otherwise returns false and the element is not inserted. */
		bool insert(llfi value); 

		/* Clears the container. */
		void clear();

		/* Returns container size. */
		llfi size() {
			return size_;
		}
		
		/* Returns reference to given block of data in container. */
		const std::vector<llfi>& operator[](llfi block_id) const {
			return data[block_id];
		}

		/* Returns reference to given block of data in container. */
		std::vector<llfi>& operator[](llfi block_id) {
			return data[block_id];
		}

	private:
		/* The major container data structure - It is a PRIME_MOD-long array of vectors.
		   When new element e is about to be inserted into container, The number e % PRIME_MOD is counted and e is inserted into data[e] (if it does not exist yet)*/
		std::vector<std::vector<llfi>> data;

		//Internal size counter
		llfi size_ = 0;
	};
}

#endif //CONTAINERS_