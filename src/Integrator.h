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

template<class T>
class Integrator {
public:
//    Integrator ()
//        : value()
//        , target()
//        , vel()
//        , accel()
//        , force()
//        , attraction() // 0.2
//        , damping() // 0.5
//        , mass(1)
//        , targeting(false)
//    {
//    }

    Integrator (T val, T damp, T attr)
        : value(val)
        , target()
        , vel()
        , accel()
        , force()
        , attraction(attr) // 0.2
        , damping(damp) // 0.5
        , mass(1)
        , targeting(false)
	{
	}

    virtual ~Integrator()
	{

	}

    void set(T v)
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


    void setTarget(T t) {
		targeting = true;
		target = t;
	}


	void noTarget() {
		targeting = false;
	}

    T getValue()
	{
		return value;
	}

	bool isTargeting()
	{
		return targeting;
	}

private:

    T value;
    T target;

    T vel;
    T accel;
    T force;

    T attraction;
    T damping;
    double mass;// = 1;
    bool targeting;
};


#endif /* INTEGRATOR_H_ */
