/*
 * Integrator.h
 *
 *  Created on: Feb 5, 2012
 *      Author: Peter Vasil
 *
 *  Original version see Processing milkteacoffee example.
 */

#ifndef INTEGRATOR_H_
#define INTEGRATOR_H_

class Integrator {
public:
	Integrator()
	{
		init(0.0f, 0.5f, 0.2f);
	}
	Integrator(double val){
		init(val, 0.5f, 0.2f);
	}
	Integrator (double val, double damp, double attr)
	{
		init(val,damp,attr);
	}
	virtual ~Integrator()
	{

	}

	void set(double v)
	{
		value = v;
	}


	void update()
	{
		if (targeting)
		{
			force += attraction * (target - value);
		}

		accel = force / mass;
		vel = (vel + accel) * damping;
		value += vel;

		force = 0;
	}


	void setTarget(double t) {
		targeting = true;
		target = t;
	}


	void noTarget() {
		targeting = false;
	}

	double getValue()
	{
		return value;
	}

	bool isTargeting()
	{
		return targeting;
	}

private:
	void init(double val, double damp, double attr)
	{
		value = val;
		damping = damp;
		attraction = attr;
		mass = 1;
		targeting = false;
		target = 0;
		vel = 0.0f;
		force = 0.0;
		accel = 0.0f;
	}
	double value;
	double vel;
	double accel;
	double force;
	double mass;// = 1;

	double damping;
	double attraction;
	bool targeting;
	double target;
};

#endif /* INTEGRATOR_H_ */
