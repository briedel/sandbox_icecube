#include <SNPS/weighting/SNOscillationWeight.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_test.h>
#include <gsl/gsl_sf_gamma.h>

double SNOscillationWeight::GetOscillationWeight(oscparams params, \
        I3Particle particle, I3Time starttime, I3Time abstime)
{

    double oscprob = SetFlavorOscillationProb(params, particle);
    
    log_debug("Osc prob = %f", oscprob);

    double fluxratio = GetInitialFluxesRatio(particle, starttime, abstime);
    
    log_debug("Flux ratio = %f", fluxratio);

    return CalculateWeight(fluxratio, oscprob);

}

void SNOscillationWeight::SetOscillationProbability(oscparams params)
{

    // Check that this is right
    if (params.hierarchy == "normal" || params.hierarchy == "Normal")
    {
        p = params.sintheta13 * params.sintheta13;
        pbar = (1. - params.sintheta12 * params.sintheta12) * (1 - params.sintheta13 * params.sintheta13);
    }
    else if (params.hierarchy == "inverted" || params.hierarchy == "Inverted")
    {
        p = params.sintheta12 * params.sintheta12 * (1 - params.sintheta13 * params.sintheta13);
        pbar = params.sintheta13 * params.sintheta13;
    }
    else
    {
        log_fatal("Please select a hierarchy");
    }
}

double SNOscillationWeight::SetFlavorOscillationProb(oscparams params, I3Particle particle)
{

    SetOscillationProbability(params);

    if (particle.GetType() == I3Particle::NuE || particle.GetType() == I3Particle::NuMu)
    {
        return p;
    }
    else if (particle.GetType() == I3Particle::NuEBar || particle.GetType() == I3Particle::NuMuBar)
    {
        return pbar;
    }
    else
    {
        log_fatal("Please selected supported particle type");
        return 0;
    }

}

double SNOscillationWeight::GetInitialFluxesRatio(I3Particle particle, I3Time starttime, I3Time abstime)
{


    
    
    
    if ( starttime - abstime  <  c_.startendtime.first ) timecorr = c_.startendtime.first - (starttime - abstime);
    
    
    // 22298000.000000
    // double time =  particle.GetTime() + (starttime - abstime) + timecorr;
    
    double time = starttime - abstime + timecorr;
    
    // if ( timebefore != time )
    // {
        if (particle.GetType() == I3Particle::NuE)
        {
            lumin_nuinit = c_.Lnue_cspline_ptr->eval(time);
            lumin_nufinal = c_.Lnux_cspline_ptr->eval(time);
            alpha_nuinit = c_.anue_cspline_ptr->eval(time);
            alpha_nufinal = c_.anux_cspline_ptr->eval(time);
            mEnu_nuinit = c_.mEnue_cspline_ptr->eval(time);
            mEnu_nufinal = c_.mEnux_cspline_ptr->eval(time);

        }
        else if (particle.GetType() == I3Particle::NuEBar)
        {
            lumin_nuinit = c_.Lnuebar_cspline_ptr->eval(time);
            lumin_nufinal = c_.Lnux_cspline_ptr->eval(time);
            alpha_nuinit = c_.anuebar_cspline_ptr->eval(time);
            alpha_nufinal = c_.anux_cspline_ptr->eval(time);
            mEnu_nuinit = c_.mEnuebar_cspline_ptr->eval(time);
            mEnu_nufinal = c_.mEnux_cspline_ptr->eval(time);
        }
        else if (particle.GetType() == I3Particle::NuMu)
        {
            lumin_nuinit = c_.Lnux_cspline_ptr->eval(time);
            lumin_nufinal = c_.Lnue_cspline_ptr->eval(time);
            alpha_nuinit = c_.anux_cspline_ptr->eval(time);
            alpha_nufinal = c_.anue_cspline_ptr->eval(time);
            mEnu_nuinit = c_.mEnux_cspline_ptr->eval(time);
            mEnu_nufinal = c_.mEnue_cspline_ptr->eval(time);
        }
        else if (particle.GetType() == I3Particle::NuMuBar)
        {
            lumin_nuinit = c_.Lnux_cspline_ptr->eval(time);
            lumin_nufinal = c_.Lnuebar_cspline_ptr->eval(time);
            alpha_nuinit = c_.anux_cspline_ptr->eval(time);
            alpha_nufinal = c_.anuebar_cspline_ptr->eval(time);
            mEnu_nuinit = c_.mEnux_cspline_ptr->eval(time);
            mEnu_nufinal = c_.mEnuebar_cspline_ptr->eval(time);
        }
        else
        {
            log_fatal("Please selected supported particle type");
        }
        // timebefore = time;
    // }

    double pspec_nuorig = GetSpectralProbability(particle.GetEnergy(), alpha_nuinit, mEnu_nuinit);
    double pspec_nufinal = GetSpectralProbability(particle.GetEnergy(), alpha_nufinal, mEnu_nufinal);

    double origflux_nuinit = (lumin_nuinit / mEnu_nuinit) * pspec_nuorig;
    double origflux_nufinal = (lumin_nufinal / mEnu_nufinal) * pspec_nufinal;

    log_debug("Initial flux of interacted neutrino = %e", origflux_nuinit);
    log_debug("Initial flux of flavor partner = %e", origflux_nufinal);

    return origflux_nufinal / origflux_nuinit;
}

double SNOscillationWeight::GetSpectralProbability(double Enu, double alpha, double mEnu)
{
    return gsl_ran_gamma_pdf( Enu , alpha + 1 , mEnu / (alpha + 1) );
}

double SNOscillationWeight::CalculateWeight(double fluxratio, double oscprob)
{
    return oscprob + (1. - oscprob) * fluxratio;
}
