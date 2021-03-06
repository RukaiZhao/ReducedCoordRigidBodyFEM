#include "World.h"

#include <iostream>
#include <fstream>
#include <json.hpp>

#include "Joint.h"
#include "JointNull.h"
#include "JointFixed.h"
#include "JointRevolute.h"
#include "JointSplineCurve.h"
#include "JointSplineSurface.h"

#include "Node.h"
#include "Body.h"
#include "SoftBodyNull.h"
#include "SoftBodyInvertibleFEM.h"
#include "SoftBody.h"
#include "FaceTriangle.h"

#include "MatrixStack.h"
#include "Program.h"
#include "SE3.h"
#include "JsonEigen.h"

#include "ConstraintJointLimit.h"
#include "ConstraintNull.h"
#include "ConstraintLoop.h"
#include "ConstraintAttachSpring.h"
#include "ConstraintAttachSoftBody.h"

#include "Deformable.h"
#include "DeformableSpring.h"
#include "DeformableNull.h"

#include "Comp.h"
#include "CompNull.h"
#include "CompSphere.h"
#include "CompCylinder.h"
#include "CompDoubleCylinder.h"

#include "WrapNull.h"
#include "WrapSphere.h"
#include "WrapCylinder.h"
#include "WrapDoubleCylinder.h"
#include "Vector.h"

using namespace std;
using namespace Eigen;
using json = nlohmann::json;

World::World() :
	nr(0), nm(0), nem(0), ner(0), ne(0), nim(0), nir(0), m_nbodies(0), m_njoints(0), m_ndeformables(0), m_constraints(0), m_countS(0), m_countCM(0),
	m_nsoftbodies(0), m_ncomps(0), m_nwraps(0)
{
	m_energy.K = 0.0;
	m_energy.V = 0.0;
}

World::World(WorldType type) :
	m_type(type),
	nr(0), nm(0), nem(0), ner(0), ne(0), nim(0), nir(0), m_nbodies(0), m_njoints(0), m_ndeformables(0), m_nconstraints(0), m_countS(0), m_countCM(0),
	m_nsoftbodies(0), m_ncomps(0), m_nwraps(0)
{
	m_energy.K = 0.0;
	m_energy.V = 0.0;
}

World::~World() {
}

void World::load(const std::string &RESOURCE_DIR) {

	//read a JSON file
	ifstream i(RESOURCE_DIR + "input.json");
	json js;
	i >> js;
	i.close();

	double density;
	Eigen::Vector3d sides;
	Matrix4d E;
	Vector3d p;

	switch (m_type)
	{
	case SERIAL_CHAIN:
	{
		m_h = 1.0e-2;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		Eigen::from_json(js["sides"], sides);
		//m_nbodies = 5;
		//m_njoints = 5;
		m_Hexpected = 10000; // todo
		m_tspan << 0.0, 5.0;
		m_t = 0.0;
		// Inits rigid bodies
		for (int i = 0; i < 5; i++) {

			auto body = addBody(density, sides, Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box10_1_1.obj");


			// Inits joints
			if (i == 0) {
				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
			}
			else {
				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, m_joints[i - 1]);
			}
		}
		break;
	}
	case DIFF_REVOLUTE_AXES:
		break;
	case BRANCHING:
	{
		m_h = 1.0e-2;
		m_tspan << 0.0, 50.0;
		m_t = 0.0;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		Eigen::from_json(js["sides"], sides);
		Vector3d sides_0;
		sides_0 << 1.0, 10.0, 1.0;
		Vector3d sides_1;
		sides_1 << 20.0, 1.0, 1.0;

		auto b0 = addBody(density, sides_0, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");
		auto b1 = addBody(density, sides_1, Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box20_1_1.obj");
		auto b2 = addBody(density, sides_0, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");
		auto b3 = addBody(density, sides_0, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");
		auto b4 = addBody(density, sides, Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box10_1_1.obj");
		auto b5 = addBody(density, sides_0, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");
		auto b6 = addBody(density, sides_0, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");
		auto b7 = addBody(density, sides, Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box10_1_1.obj");
		auto b8 = addBody(density, sides_0, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");
		auto b9 = addBody(density, sides_0, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");

		auto j0 = addJointRevolute(b0, Vector3d::UnitX(), Vector3d(0.0, 15.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
		auto j1 = addJointRevolute(b1, Vector3d::UnitY(), Vector3d(0.0, -10.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, j0);
		auto j2 = addJointRevolute(b2, Vector3d::UnitX(), Vector3d(-10.0, 0.0, 0.0), Matrix3d::Identity(), M_PI / 4.0, RESOURCE_DIR, j1);
		auto j3 = addJointRevolute(b3, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), M_PI / 4.0, RESOURCE_DIR, j1);
		auto j4 = addJointRevolute(b4, Vector3d::UnitY(), Vector3d(0.0, -10.0, 0.0), Matrix3d::Identity(), M_PI / 4.0, RESOURCE_DIR, j2);
		auto j5 = addJointRevolute(b5, Vector3d::UnitX(), Vector3d(-5.0, 0.0, 0.0), Matrix3d::Identity(), M_PI / 4.0, RESOURCE_DIR, j4);
		auto j6 = addJointRevolute(b6, Vector3d::UnitY(), Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), M_PI / 4.0, RESOURCE_DIR, j4);
		auto j7 = addJointRevolute(b7, Vector3d::UnitY(), Vector3d(0.0, -10.0, 0.0), Matrix3d::Identity(), M_PI / 4.0, RESOURCE_DIR, j3);
		auto j8 = addJointRevolute(b8, Vector3d::UnitX(), Vector3d(-5.0, 0.0, 0.0), Matrix3d::Identity(), M_PI / 4.0, RESOURCE_DIR, j7);
		auto j9 = addJointRevolute(b9, Vector3d::UnitY(), Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), M_PI / 4.0, RESOURCE_DIR, j7);
	}
	break;
	case SHPERICAL_JOINT:
		break;
	case LOOP:
	{
		m_h = 1.0e-2;
		m_tspan << 0.0, 50.0;
		m_t = 0.0;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		Eigen::from_json(js["sides"], sides);
		Vector3d sides_0;
		sides_0 << 1.0, 10.0, 1.0;
		Vector3d sides_1;
		sides_1 << 20.0, 1.0, 1.0;

		auto b0 = addBody(density, sides_1, Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box20_1_1.obj");
		auto b1 = addBody(density, sides_0, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");
		auto b2 = addBody(density, sides_0, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");
		auto b3 = addBody(density, sides_1, Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box20_1_1.obj");
		auto b4 = addBody(density, sides_0, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");

		auto j0 = addJointRevolute(b0, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
		auto j1 = addJointRevolute(b1, Vector3d::UnitZ(), Vector3d(-10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, j0);
		auto j2 = addJointRevolute(b2, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, j0);
		auto j3 = addJointRevolute(b3, Vector3d::UnitZ(), Vector3d(0.0, -10.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, j1);
		auto j4 = addJointRevolute(b4, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, j3);
		j4->m_qdot(0) = 5.0;

		auto constraint = make_shared<ConstraintLoop>(b2, b3);
		m_constraints.push_back(constraint);
		constraint->setPositions(Vector3d(0.0, -5.0, 0.0), Vector3d(10.0, 0.0, 0.0));
		m_nconstraints++;

	}
	break;
	case JOINT_TORQUE:
		break;
	case JOINT_LIMITS:
	{
		m_h = 1.0e-2;
		m_tspan << 0.0, 50.0;
		m_t = 0.0;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		Eigen::from_json(js["sides"], sides);

		for (int i = 0; i < 6; i++) {
			auto body = addBody(density, sides, Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box10_1_1.obj");

			// Inits joints
			if (i == 0) {
				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
			}
			else {
				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, m_joints[i - 1]);
			}

			// Init constraints
			if (i > 0) {
				addConstraintJointLimit(m_joints[i], -M_PI / 4, M_PI / 4);
			}
		}

	}

	break;
	case EQUALITY_CONSTRAINED_ANGLES:
		break;
	case EQUALITY_AND_LOOP:
		break;
	case HYBRID_DYNAMICS:
		break;
	case EXTERNAL_WORLD_FORCE:
		break;
	case JOINT_STIFFNESS:
	{	
		m_h = 1.0e-2;
		density = 1.0;
		m_grav << 0.0, 0.0, 0.0;
		Eigen::from_json(js["sides"], sides);
		
		m_stiffness = 1.0e4;
		m_damping = 1.0e3;
		m_Hexpected = 10000; // todo
		m_tspan << 0.0, 5.0;
		m_t = 0.0;
		// Inits rigid bodies
		for (int i = 0; i < 3; i++) {

			auto body = addBody(density, sides, Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box10_1_1.obj");

			// Inits joints
			if (i == 0) {
				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
			}
			else {
				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, m_joints[i - 1]);
			}
			m_joints[i]->setStiffness(m_stiffness);
			m_joints[i]->setDamping(m_damping);

		}

		m_joints[0]->m_qdot(0) = 1.0;

		break;

	}

		break;
	case SPRINGS:
	{
		m_h = 1.0e-2;
		m_tspan << 0.0, 50.0;
		m_t = 0.0;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		m_stiffness = 5.0e3;
		Eigen::from_json(js["sides"], sides);

		for (int i = 0; i < 2; i++) {
			auto body = addBody(density, sides, Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box10_1_1.obj");

			// Inits joints
			if (i == 0) {
				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
			}
			else {
				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, m_joints[i - 1]);
			}
		}

		// Init springs
		auto deformable0 = addDeformableSpring(sides(0)*sides(1)*sides(2)*density, 3, nullptr, Vector3d(10.0 * m_nbodies + 10.0, 10.0, 0.0), m_bodies[m_nbodies - 1], Vector3d(5.0, 0.0, 0.0));
		deformable0->setStiffness(m_stiffness);
		auto deformable1 = addDeformableSpring(sides(0)*sides(1)*sides(2)*density, 2, m_bodies[0], Vector3d(0.0, 0.0, 0.0), m_bodies[m_nbodies - 1], Vector3d(0.0, 0.0, 0.0));
		deformable1->setStiffness(m_stiffness);
		for (int i = 0; i < (int)m_deformables.size(); i++) {
			m_deformables[i]->load(RESOURCE_DIR);
		}

	}
	break;
	case SOFT_BODIES:
	{
		m_h = 1.0e-2;
		m_tspan << 0.0, 50.0;
		m_t = 0.0;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		Eigen::from_json(js["sides"], sides);
		double young = 1e1;
		double possion = 0.25;

		for (int i = 0; i < 2; i++) {
			auto body = addBody(density, sides, Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "cylinder_9.obj");

			// Inits joints
			if (i == 0) {
				//addJointFixed(body, Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0);

				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
			}
			else {
				auto joint = addJointRevolute(body, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, m_joints[i - 1]);
				
			}
			if (i > 0) {
			//addConstraintJointLimit(m_joints[i], -M_PI / 4, M_PI / 4);
			}
		}

		//m_joints[0]->m_qdot(0) = 5.0;
		//m_joints[1]->m_qdot(0) = -20.0;
		// Init constraints
		
		auto softbody = addSoftBody( 0.001 * density, young, possion, NEO_HOOKEAN, RESOURCE_DIR, "muscle_cyc_cyc");
		softbody->transform(Vector3d(10.0, 0.0, 0.0));
		softbody->setColor(Vector3f(255.0, 204.0, 153.0) / 255.0);

		// auto softbody1 = addSoftBody(0.01 * density, young, possion, RESOURCE_DIR, "cylinder");
		// softbody1->transform(Vector3d(20.0, 0.0, 0.0));

	}
	break;

	case COMPONENT:
	{
		m_h = 1.0e-2;
		m_tspan << 0.0, 50.0;
		m_t = 0.0;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		Eigen::from_json(js["sides"], sides);

		for (int i = 0; i < 3; i++) {
			auto body = addBody(density, sides, Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box10_1_1.obj");

			// Inits joints
			if (i == 0) {
				//addJointFixed(body, Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0);

				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
			}
			else {
				auto joint = addJointRevolute(body, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, m_joints[i - 1]);
				joint->m_qdot(0) = -5.0;
			}
		}

		auto compSphere = addCompSphere(2.0, m_bodies[0], Matrix4d::Identity(), RESOURCE_DIR);
		auto compCylinder = addCompCylinder(1.0, m_bodies[1], Matrix4d::Identity(), Vector3d(0.0, 0.0, 1.0), Vector3d(0.0, 0.0, 0.0), RESOURCE_DIR, "obstacle.obj");
		auto compDoubleCylinder = addCompDoubleCylinder(0.5, m_bodies[0], Matrix4d::Identity(), 0.5, m_bodies[2], Matrix4d::Identity(), RESOURCE_DIR, "obstacle.obj", "obstacle.obj");
	}
	break;

	case WRAP_SPHERE:
	{
		m_h = 1.0e-2;
		m_tspan << 0.0, 50.0;
		m_t = 0.0;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		Eigen::from_json(js["sides"], sides);

		for (int i = 0; i < 3; i++) {
			auto body = addBody(density, sides, Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box10_1_1.obj");

			// Inits joints
			if (i == 0) {
				//addJointFixed(body, Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0);
				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
			}
			else {
				auto joint = addJointRevolute(body, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, m_joints[i - 1]);
				joint->m_qdot(0) = -5.0;
			}
		}

		auto compSphere = addCompSphere(1.0, m_bodies[1], Matrix4d::Identity(), RESOURCE_DIR);
		auto wrapSphere = addWrapSphere(m_bodies[0], Vector3d(1.0, 0.0, 0.0), m_bodies[2], Vector3d(1.0, 0.0, 0.0), compSphere, 20, RESOURCE_DIR);

	}
	break;

	case WRAP_CYLINDER:
	{
		m_h = 1.0e-2;
		m_tspan << 0.0, 50.0;
		m_t = 0.0;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		Eigen::from_json(js["sides"], sides);

		for (int i = 0; i < 3; i++) {
			auto body = addBody(density, sides, Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box10_1_1.obj");

			if (i == 0) {
				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
			}
			else {
				auto joint = addJointRevolute(body, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, m_joints[i - 1]);
				//joint->m_qdot(0) = -5.0;
			}
		}

		Matrix4d E = SE3::RpToE(SE3::aaToMat(Vector3d(1.0, 0.0, 0.0), 0.0), Vector3d(0.0, 1.0, 0.0));
		auto compCylinder = addCompCylinder(0.5, m_bodies[1], E, Vector3d(0.0, 0.0, 1.0), Vector3d(0.0, 0.0, 0.0), RESOURCE_DIR, "obstacle.obj");
		auto wrapCylinder = addWrapCylinder(m_bodies[0], Vector3d(1.0, 0.0, 0.0), m_bodies[2], Vector3d(1.0, 0.0, 0.0), compCylinder, 20, RESOURCE_DIR);

	}
	break;

	case WRAP_DOUBLECYLINDER:
	{
		m_h = 1.0e-2;
		m_tspan << 0.0, 50.0;
		m_t = 0.0;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		Eigen::from_json(js["sides"], sides);

		for (int i = 0; i < 3; i++) {
			auto body = addBody(density, sides, Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box10_1_1.obj");

			if (i == 0) {
				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
			}
			else {
				auto joint = addJointRevolute(body, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, m_joints[i - 1]);
				joint->m_qdot(0) = -5.0;
			}
		}

		Matrix4d E = SE3::RpToE(SE3::aaToMat(Vector3d(1.0, 0.0, 0.0), 0.0), Vector3d(3.0, 1.0, 0.0));
		auto compDoubleCylinder = addCompDoubleCylinder(0.5, m_bodies[0], E, 0.5, m_bodies[2], E, RESOURCE_DIR, "obstacle.obj", "obstacle.obj");

		auto wrapDoubleCylinder = addWrapDoubleCylinder(
			m_bodies[0], Vector3d(-5.0, 0.5, 0.0), 
			m_bodies[2], Vector3d(5.0, 0.5, 0.0), 
			Vector3d(0.0, 0.0, 0.0), Vector3d(0.0, 0.0, 0.0), 
			Vector3d(0.0, 0.0, -1.0), Vector3d(0.0, 0.0, 1.0), 
			compDoubleCylinder, 20, RESOURCE_DIR);

	}
	break;

	case SPLINE_CURVE_JOINT:
	{
		m_h = 1.0e-2;
		m_tspan << 0.0, 50.0;
		m_t = 0.0;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		Eigen::from_json(js["sides"], sides);

		auto body0 = addBody(density, sides, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "cylinder_y_9.obj");
		auto joint0 = addJointRevolute(body0, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
		auto body1 = addBody(density, sides, Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "cylinder_9.obj");
		auto joint1 = make_shared<JointSplineCurve>(body1, joint0);
		m_joints.push_back(joint1);
		m_njoints++;
		Matrix4d E = SE3::RpToE(SE3::aaToMat(Vector3d(1.0, 0.0, 0.0), 0.0), Vector3d(0.0, -10.0, 0.0));
		joint1->setJointTransform(E);
		joint1->load(RESOURCE_DIR, "joint_spline_curve2.obj");
		Matrix4d cf0 = SE3::RpToE(SE3::aaToMat(Vector3d(0.0, 0.0, 1.0), PI), Vector3d(-10.0, 0.0, 0.0));
		Matrix4d cf1 = SE3::RpToE(SE3::aaToMat(Vector3d(0.0, 0.0, 1.0), PI / 2.0), Vector3d(0.0, 2.0, 0.0));
		Matrix4d cf2 = SE3::RpToE(SE3::aaToMat(Vector3d(0.0, 0.0, 1.0), 0.0), Vector3d(10.0, 0.0, 0.0));
		Matrix4d cf3 = SE3::RpToE(SE3::aaToMat(Vector3d(0.0, 0.0, 1.0), -PI / 2.0), Vector3d(0.0, -2.0, 0.0));

		joint1->addControlFrame(cf0);
		joint1->addControlFrame(cf1);
		joint1->addControlFrame(cf2);
		joint1->addControlFrame(cf3);

		auto body2 = addBody(density, sides, Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "cylinder_9.obj");
		auto joint2 = addJointRevolute(body2, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, joint1);
		E = SE3::RpToE(SE3::aaToMat(Vector3d(1.0, 0.0, 0.0), 0.0), Vector3d(10.0, 0.0, 0.0));

		joint2->setJointTransform(E);
		joint1->m_q(0) = -PI/4.0;
		joint2->m_q(0) = 15.0 * PI / 16.0;

	}
	break;
	case SPLINE_SURFACE_JOINT:
	{
		m_h = 1.0e-2;
		m_tspan << 0.0, 50.0;
		m_t = 0.0;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		Eigen::from_json(js["sides"], sides);

		auto body0 = addBody(density, sides, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");
		auto joint0 = addJointRevolute(body0, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
		auto body1 = addBody(density, sides, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");

		auto joint1 = make_shared<JointSplineSurface>(body1, joint0);
		m_joints.push_back(joint1);
		m_njoints++;
		Matrix4d E = SE3::RpToE(SE3::aaToMat(Vector3d(1.0, 0.0, 0.0), 0.0), Vector3d(0.0, -14.0, 0.0));
		joint1->setJointTransform(E);

		double t0 = 15.0;
		double r0 = PI * 0.25;
		double x, y, z, a, b, c, s1, s2;
		for (int i = 0; i < 4; ++i) {
			s1 = i / 3.0;
			x = (1 - s1)*(-t0) + s1 * t0;
			a = (1 - s1) * (-r0) + s1 * r0;
			for (int j = 0; j < 4; ++j) {
				s2 = j / 3.0;
				y = (1 - s2) * (-t0) + s2 * t0;
				z = 0.05 * (x * x + y * y);
				b = (1 - s1) * (-r0) + s1 * r0;
				c = 0;
				Vector6d ctf;
				ctf << x, z, y, a, c, b;
				joint1->addControlFrame(i,j,ctf);
			}
		}

		auto body2 = addBody(density, sides, Vector3d(0.0, -5.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "box1_10_1.obj");
		auto joint2 = addJointRevolute(body2, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, joint1);
		E = SE3::RpToE(SE3::aaToMat(Vector3d(1.0, 0.0, 0.0), 0.0), Vector3d(0.0, -10.0, 0.0));
		joint2->setJointTransform(E);
		joint0->m_q(0) = PI / 8.0;
		joint1->m_q(0) = 0.5;
		joint1->m_q(1) = 0.5;
		joint2->m_q(0) = PI / 4.0;

	}
	break;
	case SOFT_BODIES_INVERTIBLE:
	{
		m_h = 1.0e-2;
		m_tspan << 0.0, 50.0;
		m_t = 0.0;
		density = 1.0;
		m_grav << 0.0, -98, 0.0;
		Eigen::from_json(js["sides"], sides);
		double young = 1e3;
		double possion = 0.40;

		for (int i = 0; i < 2; i++) {
			auto body = addBody(density, sides, Vector3d(5.0, 0.0, 0.0), Matrix3d::Identity(), RESOURCE_DIR, "cylinder_9.obj");

			// Inits joints
			if (i == 0) {
				//addJointFixed(body, Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0);

				addJointRevolute(body, Vector3d::UnitZ(), Vector3d(0.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR);
			}
			else {
				auto joint = addJointRevolute(body, Vector3d::UnitZ(), Vector3d(10.0, 0.0, 0.0), Matrix3d::Identity(), 0.0, RESOURCE_DIR, m_joints[i - 1]);

			}
		}

		m_joints[0]->m_qdot(0) = 5.0;
		m_joints[1]->m_qdot(0) = -20.0;

		auto softbody = addSoftBodyInvertibleFEM(0.001 * density, young, possion, NEO_HOOKEAN, RESOURCE_DIR, "muscle_cyc_cyc");
		softbody->transform(Vector3d(10.0, 0.0, 0.0));
		softbody->setColor(Vector3f(255.0, 204.0, 153.0) / 255.0);

		// auto softbody1 = addSoftBody(0.01 * density, young, possion, RESOURCE_DIR, "cylinder");
		// softbody1->transform(Vector3d(20.0, 0.0, 0.0));

	}
	break;

	default:
		break;
	}

}

shared_ptr<SoftBody> World::addSoftBody(double density, double young, double possion, Material material, const string &RESOURCE_DIR, string file_name) {
	auto softbody = make_shared<SoftBody>(density, young, possion, material);
	softbody->load(RESOURCE_DIR, file_name);
	m_softbodies.push_back(softbody);
	m_nsoftbodies++;
	return softbody;
}

shared_ptr<SoftBodyInvertibleFEM> World::addSoftBodyInvertibleFEM(double density, double young, double possion, Material material, const string &RESOURCE_DIR, string file_name) {
	auto softbody = make_shared<SoftBodyInvertibleFEM>(density, young, possion, material);
	softbody->load(RESOURCE_DIR, file_name);
	m_softbodies.push_back(softbody);
	m_nsoftbodies++;
	return softbody;
}

shared_ptr<Body> World::addBody(double density, Vector3d sides, Vector3d p, Matrix3d R, const string &RESOURCE_DIR, string file_name) {
	auto body = make_shared<Body>(density, sides);
	Matrix4d E = SE3::RpToE(R, p);
	body->setTransform(E);
	body->load(RESOURCE_DIR, file_name);
	m_bodies.push_back(body);
	m_nbodies++;
	return body;
}

shared_ptr<JointRevolute> World::addJointRevolute(shared_ptr<Body> body, 
	Vector3d axis, 
	Vector3d p, 
	Matrix3d R, 
	double q, 
	const string &RESOURCE_DIR,
	shared_ptr<Joint> parent) {
	auto joint = make_shared<JointRevolute>(body, axis, parent);
	Matrix4d E = SE3::RpToE(R, p);
	joint->setJointTransform(E);
	joint->m_q(0) = q;
	joint->load(RESOURCE_DIR, "sphere2.obj");
	m_joints.push_back(joint);
	m_njoints++;
	return joint;
}

shared_ptr<JointFixed> World::addJointFixed(shared_ptr<Body> body, Vector3d p, Matrix3d R, double q, std::shared_ptr<Joint> parent) {
	auto joint = make_shared<JointFixed>(body, parent);
	Matrix4d E = SE3::RpToE(R, p);
	joint->setJointTransform(E);
	
	m_joints.push_back(joint);
	m_njoints++;
	return joint;
}

shared_ptr<ConstraintJointLimit> World::addConstraintJointLimit(shared_ptr<Joint> joint, double ql, double qu) {
	auto constraint = make_shared<ConstraintJointLimit>(joint);
	m_constraints.push_back(constraint);
	constraint->setLimits(ql, qu);
	m_nconstraints++;
	return constraint;
}

shared_ptr<DeformableSpring> World::addDeformableSpring(double mass, int n_points, shared_ptr<Body> body0, Vector3d r0, shared_ptr<Body> body1, Vector3d r1) {

	auto deformable = make_shared<DeformableSpring>(n_points, m_countS, m_countCM);
	m_deformables.push_back(deformable);
	deformable->setStiffness(m_stiffness);
	deformable->setMass(mass);
	deformable->setAttachments(body0, r0, body1, r1);
	m_ndeformables++;
	return deformable;
}

shared_ptr<CompSphere> World::addCompSphere(double r, shared_ptr<Body> parent, Matrix4d E, const string &RESOURCE_DIR) {
	auto comp = make_shared<CompSphere>(parent, r);
	m_comps.push_back(comp);
	comp->setTransform(E);
	comp->load(RESOURCE_DIR);
	m_ncomps++;
	return comp;
}

shared_ptr<CompCylinder> World::addCompCylinder(double r, shared_ptr<Body> parent, Matrix4d E, Vector3d z, Vector3d o, const string &RESOURCE_DIR, string shape) {
	auto comp = make_shared<CompCylinder>(parent, r);
	m_comps.push_back(comp);
	comp->setTransform(E);
	auto z_axis = make_shared<Vector>();
	z_axis->dir0 = z;
	comp->setZAxis(z_axis);
	auto origin = make_shared<Node>();
	origin->x0 = o;
	comp->setOrigin(origin);
	comp->load(RESOURCE_DIR, shape);

	m_ncomps++;
	return comp;

}

shared_ptr<CompDoubleCylinder> World::addCompDoubleCylinder(double rA, shared_ptr<Body> parentA, Matrix4d EA, double rB, shared_ptr<Body> parentB, Matrix4d EB, const string &RESOURCE_DIR, string shapeA, string shapeB) {
	auto comp = make_shared<CompDoubleCylinder>(parentA, rA, parentB, rB);
	m_comps.push_back(comp);
	comp->setTransformA(EA);
	comp->setTransformB(EB);
	comp->load(RESOURCE_DIR, shapeA, shapeB);
	m_ncomps++;
	return comp;
}

shared_ptr<ConstraintNull> World::addConstraintNull() {

	auto constraint = make_shared<ConstraintNull>();
	m_nconstraints++;
	m_constraints.push_back(constraint);
	return constraint;

}

shared_ptr<JointNull> World::addJointNull() {
	auto joint = make_shared<JointNull>();
	m_njoints++;
	m_joints.push_back(joint);
	return joint;
}

shared_ptr<DeformableNull> World::addDeformableNull() {

	auto deformable = make_shared<DeformableNull>();
	m_ndeformables++;
	m_deformables.push_back(deformable);
	return deformable;
}

shared_ptr<CompNull> World::addCompNull() {
	auto comp = make_shared<CompNull>();
	m_ncomps++;
	m_comps.push_back(comp);
	return comp;
}

shared_ptr<WrapObst> World::addWrapNull() {
	auto wrap = make_shared<WrapNull>();
	m_nwraps++;
	m_wraps.push_back(wrap);
	return wrap;

}

shared_ptr<WrapSphere> World::addWrapSphere(shared_ptr<Body> b0, Vector3d r0, shared_ptr<Body> b1, Vector3d r1, shared_ptr<CompSphere> compSphere, int num_points, const string &RESOURCE_DIR) {
	auto P = make_shared<Node>();
	P->x0 = r0;
	P->setParent(b0);
	auto S = make_shared<Node>();
	S->x0 = r1;
	S->setParent(b1);

	auto wrapSphere = make_shared<WrapSphere>(P, S, compSphere, num_points);
	m_nwraps++;
	wrapSphere->load(RESOURCE_DIR);
	m_wraps.push_back(wrapSphere);
	return wrapSphere;
}

shared_ptr<WrapCylinder> World::addWrapCylinder(shared_ptr<Body> b0, Vector3d r0, shared_ptr<Body> b1, Vector3d r1, shared_ptr<CompCylinder> compCylinder, int num_points, const string &RESOURCE_DIR) {
	auto P = make_shared<Node>();
	P->x0 = r0;
	P->setParent(b0);

	auto S = make_shared<Node>();
	S->x0 = r1;
	S->setParent(b1);

	auto wrapCylinder = make_shared<WrapCylinder>(P, S, compCylinder, num_points);
	m_nwraps++;
	wrapCylinder->load(RESOURCE_DIR);
	m_wraps.push_back(wrapCylinder);

	return wrapCylinder;
}

shared_ptr<WrapDoubleCylinder> World::addWrapDoubleCylinder(shared_ptr<Body> b0, Vector3d r0, shared_ptr<Body> b1, Vector3d r1, Vector3d u, Vector3d v, Vector3d z_u, Vector3d z_v, shared_ptr<CompDoubleCylinder> compDoubleCylinder, int num_points, const string &RESOURCE_DIR) {
	auto z_axis_U = make_shared<Vector>();
	z_axis_U->dir0 = z_u;
	auto z_axis_V = make_shared<Vector>();
	z_axis_V->dir0 = z_v;
	compDoubleCylinder->setZAxisA(z_axis_U);
	compDoubleCylinder->setZAxisB(z_axis_V);

	auto origin_U = make_shared<Node>();
	origin_U->x0 = u;
	compDoubleCylinder->setOriginA(origin_U);
	auto origin_V = make_shared<Node>();
	origin_V->x0 = v;
	compDoubleCylinder->setOriginB(origin_V);

	auto P = make_shared<Node>();
	P->x0 = r0;
	P->setParent(b0);
	auto S = make_shared<Node>();
	S->x0 = r1;
	S->setParent(b1);
	auto wrapDoubleCylinder = make_shared<WrapDoubleCylinder>(P, S, compDoubleCylinder, num_points);
	wrapDoubleCylinder->load(RESOURCE_DIR);
	m_wraps.push_back(wrapDoubleCylinder);
	m_nwraps++;
	return wrapDoubleCylinder;
}

void World::init() {
	for (int i = 0; i < m_nbodies; i++) {

		m_bodies[i]->init(nm);
		if (i < m_nbodies - 1) {
			m_bodies[i]->next = m_bodies[i + 1];
		}
	}

	nm = 0;
	/*for (int i = 0; i < m_njoints; i++) {
	m_joints[i]->init(nm, nr);
	}*/

	//joint ordering
	// todo

	for (int i = m_njoints - 1; i > -1; i--) {
		m_joints[i]->init(nm, nr);
	}

	for (int i = 0; i < m_njoints; i++) {
		if (i < m_njoints - 1) {
			m_joints[i]->next = m_joints[i + 1];
		}
		if (i > 0) {
			m_joints[i]->prev = m_joints[i - 1];
		}
	}

	for (int i = 0; i < m_ncomps; ++i) {
		m_comps[i]->init();
		if (i < m_ncomps - 1) {
			m_comps[i]->next = m_comps[i + 1];
		}
	}

	for (int i = 0; i < m_nwraps; ++i) {
		m_wraps[i]->init();
		if (i < m_nwraps - 1) {
			m_wraps[i]->next = m_wraps[i + 1];
		}
	}

	if (m_njoints == 0) {
		addJointNull();
	}

	if (m_ncomps == 0) {
		addCompNull();
	}

	if (m_nwraps == 0) {
		addWrapNull();
	}

	m_joints[0]->update();
	m_comps[0]->update();//todo
	m_wraps[0]->update();

	

	for (int i = 0; i < m_ndeformables; i++) {
		m_deformables[i]->countDofs(nm, nr);

		m_deformables[i]->init();
		// Create attachment constraints
		auto constraint = make_shared<ConstraintAttachSpring>(m_deformables[i]);
		m_constraints.push_back(constraint);
		m_nconstraints++;
		if (i < m_ndeformables - 1) {
			m_deformables[i]->next = m_deformables[i + 1];
		}
	}

	
	if (m_ndeformables == 0) {
		addDeformableNull();
	}

	if (m_type == SOFT_BODIES) {
		//m_softbodies[0]->setAttachments(0, m_bodies[0]);
		//m_softbodies[0]->setAttachments(3, m_bodies[0]);
		//m_softbodies[0]->setAttachments(6, m_bodies[0]);
		//m_softbodies[0]->setAttachments(9, m_bodies[0]);
		//m_softbodies[0]->setAttachments(12, m_bodies[0]);
		//m_softbodies[0]->setAttachments(19, m_bodies[0]);
		//m_softbodies[0]->setAttachments(25, m_bodies[0]);
		////m_softbodies[0]->setAttachments(30, m_bodies[0]);

		//m_softbodies[0]->setAttachments(60, m_bodies[1]);
		//m_softbodies[0]->setAttachments(63, m_bodies[1]);
		//m_softbodies[0]->setAttachments(67, m_bodies[1]);
		//m_softbodies[0]->setAttachments(69, m_bodies[1]);
		//m_softbodies[0]->setAttachments(72, m_bodies[1]);


		/*Vector3d direction, origin;
		direction = m_softbodies[0]->m_trifaces[0]->m_normal;
		origin << 9.0, 0.0, 0.0;
		m_softbodies[0]->setAttachmentsByLine(direction, origin, m_bodies[0]);
		origin << 10.0, 0.0, 0.0;
		m_softbodies[0]->setAttachmentsByLine(-direction, origin, m_bodies[1]);*/

		/*m_softbodies[1]->setAttachments(0, m_bodies[1]);
		m_softbodies[1]->setAttachments(3, m_bodies[1]);
		m_softbodies[1]->setAttachments(6, m_bodies[1]);
		m_softbodies[1]->setAttachments(9, m_bodies[1]);
		m_softbodies[1]->setAttachments(12, m_bodies[1]);
		m_softbodies[1]->setAttachments(19, m_bodies[1]);
		m_softbodies[1]->setAttachments(60, m_bodies[2]);
		m_softbodies[1]->setAttachments(63, m_bodies[2]);
		m_softbodies[1]->setAttachments(67, m_bodies[2]);
		m_softbodies[1]->setAttachments(69, m_bodies[2]);
		m_softbodies[1]->setAttachments(72, m_bodies[2]);*/

		//m_softbodies[1]->setAttachments(0, m_bodies[1]);
		//	m_softbodies[1]->setAttachments(3, m_bodies[1]);
		//m_softbodies[1]->setAttachments(6, m_bodies[2]);

		//m_softbodies[0]->setAttachmentsByXYSurface(0.5, Vector2d(5.0, 7.0), Vector2d(-0.5, 0.5), m_bodies[0]);
		//m_softbodies[0]->setAttachmentsByXYSurface(-0.5, Vector2d(5.0, 7.0), Vector2d(-0.5, 0.5), m_bodies[0]);

		//m_softbodies[0]->setAttachmentsByXYSurface(0.5, Vector2d(13.0, 15.0), Vector2d(-0.5, 0.5), m_bodies[1]);
		//m_softbodies[0]->setAttachmentsByXYSurface(-0.5, Vector2d(13.0, 15.0), Vector2d(-0.5, 0.5), m_bodies[1]);
		
		//m_softbodies[0]->setAttachmentsByXZSurface(0.5, Vector2d(5.0, 9.5), Vector2d(-0.5, 0.5), m_bodies[0]);
		//m_softbodies[0]->setAttachmentsByXZSurface(-0.5, Vector2d(5.0, 9.5), Vector2d(-0.5, 0.5), m_bodies[0]);

		//m_softbodies[0]->setAttachmentsByXZSurface(0.5, Vector2d(10.5, 15.0), Vector2d(-0.5, 0.5), m_bodies[1]);
		//m_softbodies[0]->setAttachmentsByXZSurface(-0.5, Vector2d(10.5, 15.0), Vector2d(-0.5, 0.5), m_bodies[1]);

		//m_softbodies[0]->setSlidingNodes(0, m_bodies[0], Vector3d(0.0, 1.0, 0.0));
		//m_softbodies[0]->setSlidingNodes(4, m_bodies[0], Vector3d(0.0, 1.0, 0.0));
		//m_softbodies[0]->setSlidingNodes(7, m_bodies[0], Vector3d(0.0, 1.0, 0.0));
		//m_softbodies[0]->setAttachments(6, m_bodies[0]);
		//m_softbodies[0]->setAttachments(3, m_bodies[0]);
		//m_softbodies[0]->setSlidingNodesByXZSurface(0.5, Vector2d(3.0, 8.0), Vector2d(-0.5, 0.5), -1.0, m_bodies[0]);
		
		//m_softbodies[0]->setSlidingNodesByXZSurface(-0.5, Vector2d(8.0, 9.5), Vector2d(-0.5, 0.5), 1.0, m_bodies[0]);
		//m_softbodies[0]->setSlidingNodesByXZSurface(-0.5, Vector2d(10.5, 12.5), Vector2d(-0.5, 0.5), 1.0, m_bodies[1]);

		//m_softbodies[0]->setSlidingNodesByXYSurface(0.5, Vector2d(0.0, 10.0), Vector2d(-0.5, 0.5), -1.0, m_bodies[0]);
		//m_softbodies[0]->setSlidingNodesByXYSurface(-0.5, Vector2d(0.0, 10.0), Vector2d(-0.5, 0.5), 1.0, m_bodies[0]);

		//m_softbodies[0]->setSlidingNodesByXYSurface(0.5, Vector2d(10.5, 12.5), Vector2d(-0.5, 0.5), -1.0, m_bodies[1]);
		//m_softbodies[0]->setSlidingNodesByXYSurface(-0.5, Vector2d(10.50, 12.5), Vector2d(-0.5, 0.5), 1.0, m_bodies[1]);
		//m_softbodies[0]->setAttachmentsByYZCircle(5.0, Vector2d(0.0, 0.0), 0.7, m_bodies[0]);
		//m_softbodies[0]->setAttachmentsByYZCircle(5.0, Vector2d(0.0, 0.0), 0.8, m_bodies[0]);
		//m_softbodies[0]->setAttachmentsByYZCircle(5.0, Vector2d(0.0, 0.0), 0.9, m_bodies[0]);
		//m_softbodies[0]->setAttachmentsByYZCircle(5.0, Vector2d(0.0, 0.0), 0.6, m_bodies[0]);


		m_softbodies[0]->setAttachmentsByYZCircle(5.0, Vector2d(0.0, 0.0), 0.5, m_bodies[0]);
		m_softbodies[0]->setSlidingNodesByYZCircle(7.5, Vector2d(0.0, 0.0), 0.5, m_bodies[0]);
		m_softbodies[0]->setSlidingNodesByYZCircle(12.5, Vector2d(0.0, 0.0), 0.5, m_bodies[1]);

		//m_softbodies[0]->setSlidingNodesByYZCircle(9.0, Vector2d(0.0, 0.0), 0.5, m_bodies[0]);
		//m_softbodies[0]->setSlidingNodesByYZCircle(6.0, Vector2d(0.0, 0.0), 0.5, m_bodies[0]);
		//m_softbodies[0]->setSlidingNodesByYZCircle(7.0, Vector2d(0.0, 0.0), 0.5, m_bodies[0]);
		//m_softbodies[0]->setSlidingNodesByYZCircle(10.0, Vector2d(0.0, 0.0), 0.5, m_bodies[0]);

		//m_softbodies[0]->setSlidingNodesByYZCircle(8.0, Vector2d(0.0, 0.0), 0.5, m_bodies[0]);
		//m_softbodies[0]->setSlidingNodesByYZCircle(11.0, Vector2d(0.0, 0.0), 0.5, m_bodies[1]);

		//m_softbodies[0]->setSlidingNodesByYZCircle(14.0, Vector2d(0.0, 0.0), 0.5, m_bodies[1]);
		//m_softbodies[0]->setSlidingNodesByYZCircle(12.0, Vector2d(0.0, 0.0), 0.5, m_bodies[1]);
		//m_softbodies[0]->setSlidingNodesByYZCircle(13.0, Vector2d(0.0, 0.0), 0.5, m_bodies[1]);
		//m_softbodies[0]->setSlidingNodesByYZCircle(15.0, Vector2d(0.0, 0.0), 0.5, m_bodies[1]);

	}

	for (int i = 0; i < m_nsoftbodies; i++) {
		m_softbodies[i]->countDofs(nm, nr);
		m_softbodies[i]->init();
		// Create attachment constraints
		auto constraint = make_shared<ConstraintAttachSoftBody>(m_softbodies[i]);
		m_constraints.push_back(constraint);
		m_nconstraints++;


		if (i < m_nsoftbodies - 1) {
			m_softbodies[i]->next = m_softbodies[i + 1];
		}
	}

	if (m_nsoftbodies == 0) {
		//todo
		auto softbody = make_shared<SoftBodyNull>();
		m_softbodies.push_back(softbody);
		m_nsoftbodies++;
	}

	// init constraints

	for (int i = 0; i < m_nconstraints; i++) {
		m_constraints[i]->countDofs(nem, ner, nim, nir);
		if (i < m_nconstraints - 1) {
			m_constraints[i]->next = m_constraints[i + 1];
		}
	}

	if (m_nconstraints == 0) {
		addConstraintNull();
	}
}

void World::update() {
	for (int i = 0; i < m_nbodies; i++) {
		//m_bodies[i]->update();
	}

	m_comps[0]->update();
	m_wraps[0]->update();
}

int World::getNsteps() {
	// Computes the number of results
	int nsteps = int((m_tspan(1) - m_tspan(0)) / m_h);
	return nsteps;
}

void World::draw(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog, const shared_ptr<Program> progSimple, const shared_ptr<Program> progSoft, shared_ptr<MatrixStack> P) {
	// Draw rigid bodies
	for (int i = 0; i < m_nbodies; i++) {
		m_bodies[i]->draw(MV, prog, P);
	}

	// Draw joints
	for (int i = 0; i < m_njoints; i++) {
		m_joints[i]->draw(MV, prog, progSimple, P);
	}

	// Draw springs
	for (int i = 0; i < m_ndeformables; i++) {
		m_deformables[i]->draw(MV, prog, progSimple, P);
	}

	// Draw soft bodies
	for (int i = 0; i < m_nsoftbodies; i++) {
		m_softbodies[i]->draw(MV, prog, progSimple, P);
	}

	// Draw components
	for (int i = 0; i < m_ncomps; i++) {
		m_comps[i]->draw(MV, prog, P);
	}

	// Draw wrappings
	for (int i = 0; i < m_nwraps; ++i) {
		m_wraps[i]->draw(MV, prog, progSimple, P);
	}

}

Energy World::computeEnergy() {
	m_energy.K = 0.0;
	m_energy.V = 0.0;

	m_joints[0]->computeEnergies(m_grav, m_energy);
	m_deformables[0]->computeEnergies(m_grav, m_energy);
	m_energy = m_softbodies[0]->computeEnergies(m_grav, m_energy);

	if (m_t == 0.0) {
		m_energy0 = m_energy;
	}

	m_energy.V -= m_energy0.V;

	return m_energy;
}

shared_ptr<Body> World::getBody(int uid) {
	MapBodyUID::const_iterator it = m_bodyUID.find(uid);
	return (it == m_bodyUID.end() ? NULL : it->second);
}

shared_ptr<Body> World::getBody(const string &name) {
	MapBodyName::const_iterator it = m_bodyName.find(name);
	return (it == m_bodyName.end() ? NULL : it->second);
}

shared_ptr<Joint> World::getJoint(int uid) {
	MapJointUID::const_iterator it = m_jointUID.find(uid);
	return (it == m_jointUID.end() ? NULL : it->second);
}

shared_ptr<Joint> World::getJoint(const string &name) {
	MapJointName::const_iterator it = m_jointName.find(name);
	return (it == m_jointName.end() ? NULL : it->second);
}