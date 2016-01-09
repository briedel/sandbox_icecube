#include <SNPS/helpers/gsl_interp_wrapper.h>
#include <iostream>

double gsl_interp_base::eval(double t)
{

    return gsl_spline_eval(spline, t, accel);

}

gsl_cspline_wrapper::gsl_cspline_wrapper(std::vector<double> &t, std::vector<double> &quantity)
{
    
    int npts = quantity.size();

    gsl_interp_accel *accel_ptr;
    gsl_spline *spline_ptr;

    accel_ptr = gsl_interp_accel_alloc();
    spline_ptr = gsl_spline_alloc (gsl_interp_cspline, npts);

    gsl_spline_init(spline_ptr, &t[0], &quantity[0], npts);

    accel = accel_ptr;
    spline = spline_ptr;

}

gsl_linear_wrapper::gsl_linear_wrapper(std::vector<double> &t, std::vector<double> &quantity)
{

    int npts = quantity.size();

    gsl_interp_accel *accel_ptr;
    gsl_spline *spline_ptr;

    accel_ptr = gsl_interp_accel_alloc();
    spline_ptr = gsl_spline_alloc (gsl_interp_linear, npts);

    gsl_spline_init (spline_ptr, &t[0], &quantity[0], npts);

    accel = accel_ptr;
    spline = spline_ptr;

}