#include "BodyCuboid.h"
#include "SE3.h"

using namespace std;
using namespace Eigen;


BodyCuboid::BodyCuboid(double density, Vector3d sides):
Body(density), m_sides(sides)
{

}

void BodyCuboid::computeInertia_() {
	// Computes inertia at body
	I_i = SE3::inertiaCuboid(m_sides, m_density);

}

