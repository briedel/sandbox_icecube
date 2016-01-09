#include <SNPS/helpers/integrationhelpers.h>
#include <SNPS/interactions/IBD.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_sf_gamma.h>

double gsl_integration_wrapper::gsl_thunk(double x, void *p)
{
  typedef boost::function<double (double)> func_t;
  func_t *f = static_cast<func_t*>(p);
  return (*f)(x);
}

double gsl_integration_wrapper::Integrate(boost::function<double (double)> f, double low, double high, double epsabs=1.49e-8, double epsrel=1.49e-8, size_t limit=50)
{
    assert(std::isfinite(low));
    assert(std::isfinite(high));

    gsl_function gf;
    gf.function = &gsl_thunk;
    gf.params = &f;

    double result;
    double abserr;

    gsl_integration_workspace *workspace = gsl_integration_workspace_alloc(limit);
    // Adaptive Gauss-Kronrod 21-point integration rule
    int err = gsl_integration_qags(&gf, low, high, epsabs, epsrel, limit, workspace, &result, &abserr);
    gsl_integration_workspace_free(workspace);

    return result;
}


// int main()
// {
//     Interaction *inter = new IBD;
//     double alpha = 4.;
//     double theta = 12.5 / alpha / 1000.;
//     typedef boost::function<double (double)> f1;
//     typedef boost::function<double (double, double, double)> f2;
//     f1 dfunc = boost::bind(gsl_ran_gamma_pdf, _1, alpha, theta);
//     f1 sigma = boost::bind(&Interaction::GetTotalCrossSection, inter, _1);
//     f1 specxsigma = gsl_integration_wrapper::multiply(dfunc, sigma);
//     double r = gsl_integration_wrapper::Integrate(specxsigma, 1.806/1000., 100/1000.);

//     std::cout << r << std::endl;

//     return 0;

// }