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

    Integrator (double val, double damp, double attr)
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

    double getValue() const
    {
        return value;
    }

    bool isTargeting() const
    {
        return targeting;
    }

private:

    double value;
    double target;

    double vel;
    double accel;
    double force;

    double attraction;
    double damping;
    double mass;// = 1;
    bool targeting;
};


#endif /* INTEGRATOR_H_ */
