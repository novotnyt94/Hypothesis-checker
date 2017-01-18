#include "containers.hpp"

namespace cube {
	bool result_set::insert(llfi value) {
		llfi index = value % PRIME_MOD;
		if (std::find(data[index].cbegin(), data[index].cend(), value) == data[index].cend()) {
			data[index].push_back(value);
			size_++;
			return true;
		}
		return false;
	}

	void result_set::clear() {
		for (llfi index = 0; index < data.size(); index++) {
			data[index].clear();
		}
		size_ = 0;
	}
}