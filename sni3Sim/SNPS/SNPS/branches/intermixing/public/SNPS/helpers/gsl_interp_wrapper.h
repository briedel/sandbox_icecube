#ifndef GSL_INTERP_WRAPPER_H_INCLUDED
#define GSL_INTERP_WRAPPER_H_INCLUDED

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

#include <vector>

/**
 * Wrapper class around gsl spline interpolation
 */

class gsl_interp_base
{

protected:

    gsl_interp_accel *accel;
    gsl_spline *spline;

public:

    double eval(double);

    virtual ~gsl_interp_base()
    {

        gsl_spline_free (spline);
        gsl_interp_accel_free(accel);

    };

};

class gsl_cspline_wrapper : public gsl_interp_base
{

public:

    gsl_cspline_wrapper(std::vector<double> &, std::vector<double> &);

    virtual ~gsl_cspline_wrapper() {};

};

class gsl_linear_wrapper : public gsl_interp_base
{

public:

    gsl_linear_wrapper(std::vector<double> &, std::vector<double> &);

    virtual ~gsl_linear_wrapper() {};

};

#endif //GSL_INTERP_WRAPPER_H_INCLUDED