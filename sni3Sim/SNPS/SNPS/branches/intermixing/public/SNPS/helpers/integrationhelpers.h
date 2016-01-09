#ifndef INTEGRATIONHELPERS_H_INCLUDED
#define INTEGRATIONHELPERS_H_INCLUDED

#include <iostream>
#include <cmath>
#include <vector>

#include <boost/function.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

#include <gsl/gsl_integration.h>


/**
 * Wrapper around gsl intergration
 */

class integrationhelpers
{

public:

    integrationhelpers() {};

    virtual ~integrationhelpers() {};

private:

};

class gsl_integration_wrapper : public integrationhelpers
{
public:

    gsl_integration_wrapper() {};

    virtual ~gsl_integration_wrapper() {};

    /**
     * Wrapper to integrate boost function. 
     * Taken from jvansanten
     *
     * @param Boost function that takes double and return double
     * @param lower integration bound
     * @param higher integration bound
     * @param absolute error
     * @param relative error
     * @param step size
     * @return result of integration
     */
    static double Integrate(boost::function<double (double)>, double, double, double , double, size_t);

    /**
     * Wrapper around boost function to make gsl happy
     * Taken from jvansanten
     */
    static double gsl_thunk(double, void *);

    /**
     * Multiply two boost::functions
     */
    struct multiply
    {
        typedef boost::function<double (double)> func_t;
        multiply(func_t f, func_t g) : f_(f), g_(g) {};
        func_t f_, g_;
        typedef double result_type;
        inline double operator()(double x) const
        {
            return f_(x) * g_(x);
        }
    };

    struct
    {
        boost::function<double (double, double)> bf;
        double alpha;
        double theta;
    } params;
};



// class trapezoidal_wrapper : public integrationhelpers
// {

// public:

//     trapezoidal_wrapper(int npts, double lower_limit, double upper_limit);

//     virtual ~trapezoidal_wrapper();

//     /* data */
// };

#endif // INTEGRATIONHELPERS_H_INCLUDED