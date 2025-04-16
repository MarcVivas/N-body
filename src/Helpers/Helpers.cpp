#include "Helpers.h" 


namespace helpers {

    std::ostream& operator<<(std::ostream& os, const glm::vec4& vec) {
        os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const glm::ivec2& vec) {
        os << "(" << vec.x << ", " << vec.y << ")";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Node& node) {
        os << "Node {\n"
            << "  Mass:         " << node.getMass() << "\n"
            << "  CenterOfMass: " << node.getCenterOfMass() << "\n"
            << "  MinBoundary:  " << node.getMinBoundary() << "\n"
            << "  MaxBoundary:  " << node.getMaxBoundary() << "\n"
            << "  FirstChild:  " << node.getFirstChild() << "\n"
            << "  Next:  " << node.getNext() << "\n"
            << "  IsLeaf:  " << node.isLeaf() << "\n"
            << "  IsOccupied:  " << node.isOccupied() << "\n"
            << "}";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Task& task) {
        os << "Task {\n"
			<< "  Root:         " << task.root << "\n"
			<< "  TotalParticles: " << task.totalParticles << "\n"
			<< "}";
		return os;
    }


    

} // namespace helpers
