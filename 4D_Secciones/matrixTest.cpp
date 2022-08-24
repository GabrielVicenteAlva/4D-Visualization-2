#include "matrix.hpp"

using namespace simple_matrix;

struct Strc {
	matrix &m;
	Strc();
};
Strc::Strc() {
	m = matrix(0,0);
};

int main() {
	matrix A(3, 2, {1, 3, 5, 2, 4, 6});
	matrix &B = A;
	A(0,0) = 0;
	std::cout<<A<<std::endl;
	std::cout<<B<<std::endl;
	Strc s();
	return 0;
}
