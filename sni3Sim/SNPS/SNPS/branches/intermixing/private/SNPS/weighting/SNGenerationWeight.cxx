#include <SNPS/weighting/SNGenerationWeight.h>
#include <SNPS/constants/SNPSConstants.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_test.h>
#include <gsl/gsl_sf_gamma.h>


double SNGenerationWeight::GetGenWeight( SNPSBaseClass::genweigtinfo &gwi)
{
    log_debug("=========Calculating Generation Weight=========");

    // double pPhys = GetPhysicsProb( gwi.physicsspectrum, gwi.Enu * 1000 , gwi.mEnu * 1000., gwi.alpha );

    // double pGen = GetGenProb( gwi.inputspectrum , gwi.interaction, gwi.maxE * 1000. , gwi.Enu * 1000, gwi.gammameanE, gwi.gammaalpha );

    // double pPhys = GetPhysicsProb( gwi.physicsspectrum, gwi.Enu, gwi.mEnu, gwi.alpha );

    double pPhys = GetWeightedPhysProb( gwi.physicsspectrum, gwi.interaction, gwi.Enu, gwi.mEnu, gwi.alpha, gwi.maxE );

    double pGen = GetGenProb( gwi.inputspectrum, gwi.interaction, gwi.maxE, gwi.Enu, gwi.gammameanE / 1000., gwi.gammaalpha );

    // if (pPhys > 1 || pPhys < 0) log_fatal("Physics Probability > 1, < 0. Physics probability is %e", pPhys);

    // if (pGen > 1 || pGen < 0) log_fatal("Generation Probability > 1, < 0. Generation probability is %e", pGen);

    log_debug("Physics Probability %e", pPhys );

    log_debug("Generation Probability %e", pGen );

    log_debug( "Num of interacted neutrinos %e", gwi.N_SN);

    log_debug( "Num of neutrinos in sample %e", gwi.N_gen);

    /// Have to do a unit correction to MeV here
    double norm = gwi.N_SN * pPhys;
    double denom = gwi.N_gen * pGen;

    return ( norm / denom );
}

double SNGenerationWeight::GetTotalNoNeutrinos( double L, double d, double mEnu, double deltat, double area )
{

    if ( mEnu <= 0. )
    {
        log_debug("Mean Energy is <= 0. Returning 0");
        return 0.;
    }

    log_debug("=========Getting total number of neutrinos in detector=========");

    double denom = 4. * I3Constants::pi * d * d * mEnu;
    double numer = area * L * deltat;

    log_debug( "Total Number of Neutrinos %e", numer / denom );

    return numer / denom;

}

double SNGenerationWeight::GetTotalNoInteractedNeutrinos( double L, double d, double mEnu, \
        double deltat, double area, double height, \
        double alpha, double Emax, \
        std::string interaction, \
        std::string physicsspectrum )
{

    if ( mEnu <= 0. || alpha <= 0.)
    {
        log_debug("Mean Energy is <= 0. Returning 0");
        return 0.;
    }
    
    boost::shared_ptr<Interaction> inter = GetInteractionPtr(interaction);

    double neutrinos = GetTotalNoNeutrinos(L, d, mEnu, deltat, area);
    // No. of particles needs to be relative to interaction
    double constant = neutrinos * height * SNPSConstants::targetspervol * inter->GetNumberOfTargets();

    log_debug("Neutrinos times total number of targets %e", constant );

    double integral = GetIntegralCrossSectionSpectrum( physicsspectrum, interaction, mEnu, alpha, Emax);

    log_debug("Integral CrossSection x Spectrum %e", integral);

    return constant * integral;
}

double SNGenerationWeight::GetIntegralCrossSectionSpectrum( std::string physicsspectrum, std::string interaction, \
        double mEnu, double alpha, double Emax)
{

    double integral;
    imap::iterator i = integralmap.find( std::make_pair(alpha, mEnu) );
    if ( i == integralmap.end() )
    {
        boost::function<double (double)> specxsigma = GetWeightedPhysSpectrum( physicsspectrum, interaction, mEnu, alpha);
        integral = integrate->Integrate(specxsigma, GetEmin(interaction), Emax, 1.49e-8, 1.49e-8, 100);
        integralmap.insert( std::make_pair( std::make_pair(alpha, mEnu), integral ) );
    }
    else
    {
        integral = i->second;
    }

    log_debug("Emax %e", Emax);

    return integral;
}

double SNGenerationWeight::GetGenArea(double r)
{
    return r * r * I3Constants::pi;
}

boost::shared_ptr<Interaction> SNGenerationWeight::GetInteractionPtr(std::string interaction)
{
    if (interaction == "IBD") return boost::shared_ptr<Interaction>(new IBD);
    if (interaction != "IBD")
    {
        log_fatal("Please select a supported interaction");
        return boost::shared_ptr<Interaction>();
    }

    return boost::shared_ptr<Interaction>();
}

double SNGenerationWeight::GetPhysicsProb(std::string physicsspectrum, double Enu, double mEnu, double alpha)
{
    log_debug("=========Calculating Physics Probability=========");

    log_debug("Alpha %e", alpha + 1);
    log_debug("Theta %e", mEnu / (alpha + 1));
    log_debug("Physics Prob %e", gsl_ran_gamma_pdf( Enu, alpha + 1 , mEnu / (alpha + 1) ) );
    log_debug("Physics Prob MeV %e", gsl_ran_gamma_pdf( Enu, alpha + 1., mEnu / (alpha + 1.) ) );

    if ( physicsspectrum == "Garching" )
    {
        return gsl_ran_gamma_pdf( Enu , alpha + 1 , mEnu / (alpha + 1) );
    }
    else
    {
        log_fatal("The selected physics prob is not yet supported.");
    }

}

double SNGenerationWeight::GetWeightedPhysProb(std::string physicsspectrum, std::string interaction, \
        double Enu, double mEnu, double alpha, double Emax)
{
    double specxsigma = GetWeightedPhysSpecValue( physicsspectrum, interaction, Enu, mEnu, alpha);

    double integral = GetIntegralCrossSectionSpectrum( physicsspectrum, interaction, mEnu, alpha, Emax);

    log_debug( "Spec x sigma integral %e", integral );

    log_debug( "specxsigma %e", specxsigma );

    return specxsigma / integral;
}

boost::function<double (double)> SNGenerationWeight::GetWeightedPhysSpectrum(std::string physicsspectrum, std::string interaction, double mEnu, double alpha)
{
    
    if ( physicsspectrum != "Garching" ) log_fatal("The selected physics prob is not yet supported.");

    boost::shared_ptr<Interaction> inter = GetInteractionPtr(interaction);

    if ( !inter ) log_fatal("Interaction Shared Pointer not set");

    alpha += 1.;
    double theta = mEnu / alpha;

    log_debug("Alpha %e", alpha);
    log_debug("Theta %e", theta);

    typedef boost::function<double (double)> f1;
    f1 dfunc = boost::bind(gsl_ran_gamma_pdf, _1, alpha, theta);
    f1 sigma = boost::bind(&Interaction::GetTotalCrossSection, inter, _1);

    log_debug("dfunc for mEnu %e", dfunc(mEnu) );
    log_debug("sigma for mEnu %e", sigma(mEnu) );

    f1 specxsigma = multiply(dfunc, sigma);

    log_debug("specxsigma for mEnu %e", specxsigma(mEnu) );

    return specxsigma;
}

double SNGenerationWeight::GetWeightedPhysSpecValue(std::string physicsspectrum, std::string interaction, double Enu, double mEnu, double alpha)
{
    wps.physicsspectrum = physicsspectrum;
    wps.interaction = interaction;
    wps.mEnu = mEnu;
    wps.alpha = alpha;
    boost::function<double (double)> specxsigma;
    fmap::iterator tempit = functionmap.find(wps);
    if ( tempit == functionmap.end() )
    {
        specxsigma = GetWeightedPhysSpectrum( physicsspectrum, interaction, mEnu, alpha);
        functionmap.insert( std::make_pair( wps, specxsigma ) );
    }
    else
    {
        specxsigma = tempit->second;
    }

    double result = specxsigma(Enu);

    return result;
}

double SNGenerationWeight::GetGenProb(std::string inputspectrum, std::string interaction, double Emax, double Enu, double mEnu, double alpha )
{

    log_debug("=========Calculating Generation Probability=========");

    if ( inputspectrum == "Flat" )
    {
        return gsl_ran_flat_pdf(Enu, GetEmin(interaction), Emax);
    }
    else if ( inputspectrum == "Gamma PDF" || inputspectrum == "Garching")
    {
        /// Have to renormalize the Gamma PDF because we are limiting our energy range
        /// according to interaction and a maximum energy
        alpha += 1.;
        double theta = mEnu / alpha;

        log_debug("Alpha %e", alpha);
        log_debug("Theta %e", theta);
        log_debug("MeanE %e", mEnu);

        double norm = gsl_sf_gamma ( alpha );

        log_debug("Emax %e", Emax);

        log_debug("Emin %e", GetEmin(interaction));

        log_debug("Incomplete first part %e ", gsl_sf_gamma_inc( alpha , GetEmin(interaction) / theta ) );

        log_debug("Incomplete second part %e", gsl_sf_gamma_inc( alpha , Emax / theta ) );

        double renorm = ( gsl_sf_gamma_inc( alpha , GetEmin(interaction) / theta ) - gsl_sf_gamma_inc( alpha , Emax / theta ) );

        log_debug("Original normalization factor %e", norm );
        log_debug("New normalization factor %e", renorm );
        log_debug("Renormalization factor %e", norm / renorm );
        log_debug("Energy [GeV] %e", Enu );
        log_debug("Generation Probability %e", gsl_ran_gamma_pdf( Enu , alpha , theta ) );

        return (norm / renorm) * gsl_ran_gamma_pdf( Enu, alpha , theta);

        // return gsl_ran_gamma_pdf( Enu, alpha , theta );

    }

    log_fatal("Please select a supported generation function");

}

double SNGenerationWeight::GetEmin(std::string interactions)
{
    //TODO: Add support for O-interactions
    if ( interactions == "IBD" ) return 1.806 * I3Units::MeV;
    if ( interactions != "IBD" ) return 0.;
    log_fatal("Please select a supported interaction");
}

bool operator==(const SNGenerationWeight::weightedphysspec &lhs, const SNGenerationWeight::weightedphysspec &rhs)
{
    return lhs.physicsspectrum == rhs.physicsspectrum &&
           lhs.interaction == rhs.interaction &&
           lhs.mEnu == rhs.mEnu &&
           lhs.alpha == rhs.alpha;
}

bool operator<(const SNGenerationWeight::weightedphysspec &lhs, const SNGenerationWeight::weightedphysspec &rhs)
{
    if ( lhs.physicsspectrum < rhs.physicsspectrum ) return true;
    if ( lhs.physicsspectrum > rhs.physicsspectrum ) return false;
    if ( lhs.interaction < rhs.interaction ) return true;
    if ( lhs.interaction > rhs.interaction ) return false;
    if ( lhs.mEnu < rhs.mEnu ) return true;
    if ( lhs.mEnu > rhs.mEnu  ) return false;
    if ( lhs.alpha < rhs.alpha ) return true;
    if ( lhs.alpha > rhs.alpha ) return false;
    return false;

};