
#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include <vector>
#include <ostream>
#include "Node.h"
#include <glm/glm.hpp>
#include "ParallelOctreeCPU.h"

namespace helpers {
	std::ostream& operator<<(std::ostream& os, const glm::ivec2& vec);
	std::ostream& operator<<(std::ostream& os, const glm::vec4& vec);
	std::ostream& operator<<(std::ostream& os, const Node& node);
	std::ostream& operator<<(std::ostream& os, const Task& task);

	inline int ipow(int base, int exp) {
		if (exp < 0) return 0;
		int result = 1;
		while (exp) {
			if (exp & 1) result *= base;
			exp >>= 1;
			base *= base;
		}
		return result;
	}

	template <typename T>
	void log(const T& elem) {
		std::cout << elem << std::endl;
	}

	template <typename U>
	void log(const std::vector<U> &elem) {
		std::cout << "Total elements: " << elem.size() << " [";
		for (const auto& e : elem) {
			std::cout << e << " ";
		}
		
		std::cout << "]\n";
	}
	

	template <typename T, typename... Args>
	void log(const T& first, const Args&... rest) {
		std::cout << first << " ";
		log(rest...);
	}	
}

#endif //HELPERS_H
