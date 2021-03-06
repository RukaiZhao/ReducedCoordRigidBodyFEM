#include "JointRevolute.h"

#include <iostream>

#include "Body.h"
#include "SE3.h"
#include "Shape.h"
#include "Program.h"
#include "MatrixStack.h"

using namespace std;
using namespace Eigen;

JointRevolute::JointRevolute() {

}


JointRevolute::JointRevolute(std::shared_ptr<Body> body, Eigen::Vector3d axis, std::shared_ptr<Joint> parent):
Joint(body, 1, parent)
{
	m_axis = axis;
}

void JointRevolute::load(const std::string &RESOURCE_DIR, std::string joint_shape) {

	m_jointShape = make_shared<Shape>();
	m_jointShape->loadMesh(RESOURCE_DIR + "sphere2.obj");

}


JointRevolute::~JointRevolute() {

}

void JointRevolute::updateSelf() {
	Matrix3d R = SE3::aaToMat(m_axis, m_q(0));
	Matrix4d Q;
	Q.setIdentity();
	Q.block<3, 3>(0, 0) = R;
	m_Q = Q;
	//E_pj = E_pj0 * Q;
	
	m_S.block<3, 1>(0, 0) = m_axis;
}

void JointRevolute::drawSelf(shared_ptr<MatrixStack> MV, const shared_ptr<Program> prog, const shared_ptr<Program> progSimple, shared_ptr<MatrixStack> P) const {
	prog->bind();

	float r = 0.5f;
	if (m_jointShape) {
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniform3f(prog->getUniform("lightPos1"), 66.0f, 25.0f, 25.0f);
		glUniform1f(prog->getUniform("intensity_1"), 0.6f);
		glUniform3f(prog->getUniform("lightPos2"), -66.0f, 25.0f, 25.0f);
		glUniform1f(prog->getUniform("intensity_2"), 0.2f);
		glUniform1f(prog->getUniform("s"), 300.0f);
		glUniform3f(prog->getUniform("ka"), 0.2f, 0.2f, 0.2f);
		glUniform3f(prog->getUniform("kd"), 0.8f, 0.7f, 0.7f);
		glUniform3f(prog->getUniform("ks"), 1.0f, 0.9f, 0.8f);

		MV->pushMatrix();
		MV->multMatrix(eigen_to_glm(E_wj));
		MV->scale(r);
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		m_jointShape->draw(prog);
		MV->popMatrix();
	}
	prog->unbind();
}

