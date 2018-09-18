// SpringSerial Non-zero rest-length serial springs
//		A simple spring made up of a series of nodes. In the future, we may
//		want to make this an abstract class and derive from it. 

#pragma once
#ifndef MUSCLEMASS_SRC_SPRINGSERIAL_H_
#define MUSCLEMASS_SRC_SPRINGSERIAL_H_

#include "Spring.h"

class Body;
class Node;

class SpringSerial : public Spring 
{
public:
	SpringSerial();
	SpringSerial(int n_nodes, int &countS, int &countCM);

	void setStiffness(double K) { m_K = K; }
	void setMass(double mass) { m_mass = mass; }
	void setAttachments(std::shared_ptr<Body> body0, std::shared_ptr<Body> body1);

	virtual void countDofs_();
	virtual void gatherDofs_(Eigen::VectorXd &y, int nr);
	virtual void gatherDDofs_(Eigen::VectorXd &ydot, int nr);
	virtual void scatterDofs_(Eigen::VectorXd &y, int nr);
	virtual void scatterDDofs_(Eigen::VectorXd &ydot, int nr);
	virtual void computeMassForce_(Eigen::Vector3d grav, Eigen::MatrixXd &M, Eigen::VectorXd &f);
	virtual void computeEnergies_(Eigen::Vector3d grav, double &T, double &V);


	double m_K;
	std::shared_ptr<Body> m_body0;
	std::shared_ptr<Body> m_body1;

	double m_mass;
	std::vector<std::shared_ptr<Node>> m_nodes;

};


#endif // MUSCLEMASS_SRC_SPRINGSERIAL_H_