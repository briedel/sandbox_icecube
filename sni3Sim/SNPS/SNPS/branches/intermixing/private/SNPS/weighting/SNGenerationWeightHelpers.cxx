#include <SNPS/weighting/SNGenerationWeightHelpers.h>

#include <numeric>

I3VectorDouble SNGenerationWeightHelpers::GetTotalNumInteractedNeutrinoTimeBin(
    std::string model, double distance, \
    double radius, double height, \
    double Emax, std::string interaction)
{
    c.modelfile =  std::string(std::getenv("I3_BUILD")) + "/SNPS/resources/supernovaspectra.h5";
    c.model = model;
    double t = c.startendtime.first;

    I3VectorDouble nuperbin;
    std::string physpec =  SNEventSelectionBaseClass::GetPhysSpec(c.model);

    SNEventSelectionBaseClass::ConfigureTimeInterpolation(c);

    boost::shared_ptr<gsl_linear_wrapper> L;
    boost::shared_ptr<gsl_linear_wrapper> mE;
    boost::shared_ptr<gsl_linear_wrapper> a;

    if ( interaction == "IBD" )
    {
        L = c.Lnuebar_cspline_ptr;
        mE = c.mEnuebar_cspline_ptr;
        a = c.anuebar_cspline_ptr;
    }
    else
    {
        log_fatal( "Please select a supported interaction type" );
    }

    std::vector<double>::const_iterator vecit;
    for (vecit = c.itdt.begin(); vecit != c.itdt.end(); vecit++)
    {
        if ( vecit == c.itdt.begin() ) vecit++;
        double numnu = SNGenerationWeight::GetTotalNoInteractedNeutrinos(L->eval(t), distance, mE->eval(t),
                       *vecit, GetGenArea(radius),
                       height, a->eval(t),
                       Emax * I3Units::MeV,
                       interaction, physpec);
        nuperbin.push_back(numnu);
        t += *vecit;

    }

    return nuperbin;

}

double SNGenerationWeightHelpers::GetTotalNumInteractedNeutrinoModel(
    std::string model, double distance, \
    double radius, double height, \
    double Emax, std::string interaction)
{
    I3VectorDouble nuperbin = GetTotalNumInteractedNeutrinoTimeBin(model, distance, radius, height, Emax, interaction );
    double nusum = std::accumulate(nuperbin.begin(), nuperbin.end(), 0);
    return nusum;
}
