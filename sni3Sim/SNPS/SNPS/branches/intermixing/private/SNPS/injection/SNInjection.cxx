#include <SNPS/injection/SNInjection.h>

#include <SNPS/interactions/IBD.h>
#include <SNPS/interactions/escatter_flavor_selection.h>
// #include <cross-sections/Ointeraction.h>

#include <SNPS/constants/SNPSConstants.h>

#include <dataclasses/I3Constants.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_test.h>

#include <phys-services/I3RandomService.h>

#include <cmath>

using namespace SNPSConstants;

I3_MODULE(SNInjection);

SNInjection::SNInjection(const I3Context &context) : I3Module(context)
{

    AddParameter("NumParticlesInFrame",
                 "No. of primaries particles that should be put into a single frame. Default = 10000",
                 10000);

    AddParameter("InjectionVolumeHeight",
                 "Height of the forced interaction volume. Default = 1600. m",
                 1600. * I3Units::m);

    AddParameter("InjectionCylinderRadius",
                 "Radius of the forced interaction volume. Default = 800. m",
                 800. * I3Units::m);

    AddParameter("Center",
                 "New center for coordinate system",
                 I3Position(0, 0, 0));

    AddParameter("MaximumEnergy",
                 "Maximum neutrino energy to be considered. Default = 100 MeV",
                 100. * I3Units::MeV);

    AddParameter("RandomService",
                 "Random Service to be used",
                 c_.rng_);

    AddParameter("Interaction",
                 "Interaction to be considered for this sample\n"
                 "Options:\n"
                 "Inverse Beta Decay (IBD)\n"
                 "Neutrino-electron scattering (nue-e, nuebar-e, nux-e, nuxbar-e)\n"
                 "Neutrino-Oxygen (nue16O-ep, nue16O-e2p, nue16O-epgamma, nuebar16O-eplusn, nuebar16O-eplus, nuebar16O-eplusngamma, nue18O)\n"
                 "Default = Inverse beta decay\n",
                 "IBD");

    AddParameter("GenerationSpectrum",
                 "Desired generation spectrum: Flat, Garching, Gamma PDF, Gaussian",
                 "Flat");

    AddParameter("GarchingGenSpecMeanE",
                 "Mean Energy to be considered for Garching Spectrum. Default = 12.5",
                 12.5);

    AddParameter("GarchingGenSpecAlpha",
                 "Alpha-Parameter to be considered for Garching Spectrum. Default = 3",
                 3.);

    AddParameter("GammaFunctionGenSpecMeanE",
                 "Mean Energy to be considered for Gamma PDF. Default = 15",
                 15.);

    AddParameter("GammaFunctionGenSpecAlpha",
                 "Alpha-Parameter to be considered for Gamma PDF. Default = 3.0",
                 3.);

    AddParameter("GaussianMean",
                 "Mean value for Gaussian distribution. Default = 15",
                 15.);

    AddParameter("GaussianStandardDev",
                 "Standard Deviation value for Gaussian distribution. Default = 5",
                 5.);

    AddParameter("OutputI3MCTreeName",
                 "Name of the output I3MCTree",
                 "I3MCTree");

    AddParameter("OutputSNWeightMapName",
                 "Name of output SNWeightName",
                 "SNWeightMap");

    AddOutBox("OutBox");
}

void SNInjection::Configure()
{

    GetParameter("NumParticlesInFrame", nopart_);
    GetParameter("InjectionVolumeHeight", c_.height_);
    GetParameter("InjectionCylinderRadius", c_.radius_);
    GetParameter("Center", c_.center_);
    GetParameter("MaximumEnergy", c_.maxE_);
    GetParameter("RandomService", c_.rng_);
    GetParameter("Interaction", interaction_);
    GetParameter("GenerationSpectrum", c_.genspec_);
    GetParameter("GarchingGenSpecMeanE", c_.garchingmeanE_);
    GetParameter("GarchingGenSpecAlpha", c_.garchingalpha_);
    GetParameter("GammaFunctionGenSpecMeanE", c_.gammameanE_);
    GetParameter("GammaFunctionGenSpecAlpha", c_.gammaalpha_);
    GetParameter("GaussianMean", c_.gaussianmean_);
    GetParameter("GaussianStandardDev", c_.gaussianstddev_);
    GetParameter("OutputI3MCTreeName", outputI3MCTreeName_);
    GetParameter("OutputSNWeightMapName", outputSNWeightMapName_);

    if ( !c_.rng_ )
    {
        c_.rng_ = context_.Get<I3RandomServicePtr>();
        if ( !c_.rng_ )
        {
            log_fatal("CANNOT FIND RANDOM SERVICE");
        }
    }

    log_info("Injection volume height [m] = %4.2f\n"
              "Injection volume radius [m]= %4.2f\n"
              "Maximum Neutrino Energy [GeV] = %4.2f\n"
              "Interaction             = %s \n"
              "Generation Spectrum     = %s \n",
              c_.height_, c_.radius_, c_.maxE_, interaction_.c_str(), c_.genspec_.c_str());

    if ( c_.genspec_ == "Garching")
    {

        log_info("Garching mean energy = %4.2f \n"
                 "Garching alpha       = %4.2f \n",
                 c_.garchingmeanE_, c_.garchingalpha_);

    }

    if ( c_.genspec_ == "Gamma PDF")
    {
        log_info("Gamma PDF 'mean energy' = %4.2f \n"
                 "Gamma PDF 'alpha' = %4.2f \n",
                 c_.gammameanE_, c_.gammaalpha_);
    }

    if (interaction_ == "IBD")
    {

        c_.primarytype = I3Particle::NuEBar;
        c_.product1type = I3Particle::EPlus;
        c_.product2type = I3Particle::Neutron;
        c_.primaryshape = I3Particle::Primary;
        c_.product1shape = I3Particle::StartingTrack;
        c_.product2shape = I3Particle::StartingTrack;
        c_.inter = boost::shared_ptr<Interaction> (new IBD);
        /// SPECIAL NOTE: Needs to be > 1.8181 MeV because cross-sections
        /// for 1.8181 > energies > 1.806 the cross-section is too small to
        /// be properly represented by double precision 
        c_.minE_ = minEnu_IBD;

    }
    else if (interaction_ == "nue-e")
    {

        c_.primarytype = I3Particle::NuE;
        c_.product1type = I3Particle::EMinus;
        c_.product2type = I3Particle::NuE;
        c_.primaryshape = I3Particle::Primary;
        c_.product1shape = I3Particle::StartingTrack;
        c_.product2shape = I3Particle::StartingTrack;
        c_.inter = boost::shared_ptr<Interaction> (new EScatter_neutrino_e);
        c_.minE_ = minEnu_escatter;

    }
    else if (interaction_ == "nuebar-e")
    {

        c_.primarytype = I3Particle::NuEBar;
        c_.product1type = I3Particle::EMinus;
        c_.product2type = I3Particle::NuEBar;
        c_.primaryshape = I3Particle::Primary;
        c_.product1shape = I3Particle::StartingTrack;
        c_.product2shape = I3Particle::StartingTrack;
        c_.inter = boost::shared_ptr<Interaction> (new EScatter_antineutrino_e);
        c_.minE_ = minEnu_escatter;

    }
    else if (interaction_ == " nux-e")
    {

        c_.primarytype = I3Particle::NuMu;
        c_.product1type = I3Particle::EMinus;
        c_.product2type = I3Particle::NuMu;
        c_.primaryshape = I3Particle::Primary;
        c_.product1shape = I3Particle::StartingTrack;
        c_.product2shape = I3Particle::StartingTrack;
        c_.inter = boost::shared_ptr<Interaction> (new EScatter_neutrino_x);
        c_.minE_ = minEnu_escatter;

    }
    else if (interaction_ == " nuxbar-e")
    {

        c_.primarytype = I3Particle::NuMuBar;
        c_.product1type = I3Particle::EMinus;
        c_.product2type = I3Particle::NuMuBar;
        c_.primaryshape = I3Particle::Primary;
        c_.product1shape = I3Particle::StartingTrack;
        c_.product2shape = I3Particle::StartingTrack;
        c_.inter = boost::shared_ptr<Interaction> (new EScatter_antineutrino_x);
        c_.minE_ = minEnu_escatter;

    }
    else
    {

        log_fatal("PLEASE SELECT A SUPPORTED INTERACTION\n"
                  "The supported interactions are:\n"
                  "Inverse Beta Decay (IBD)\n"
                  "Neutrino-electron scattering (nue-e, nuebar-e, nux-e, nuxbar-e)\n"
                  "Neutrino-Oxygen (nue16O-ep, nue16O-e2p, nue16O-epgamma, nuebar16O-eplusn, nuebar16O-eplus, nuebar16O-eplusngamma, nue18O)"
                  );

    }
    
    if ( !c_.inter ) log_fatal("Interaction type not set");
    intweight = boost::shared_ptr<SNInteractionWeight>( new SNInteractionWeight(c_) );


}

void SNInjection::GeneratePrimary(I3Particle & prim)
{

    log_debug("=========Generate Primary=========");
    
    double phi = c_.rng_->Uniform(0, 2 * I3Constants::pi);
    double u = c_.rng_->Uniform() + c_.rng_->Uniform();
    double r = (u > 1 ? 2 - u : u) * c_.radius_;
    double x = r * cos(phi);
    double y = r * sin(phi);
    double z = c_.rng_->Uniform( -c_.height_ / 2, c_.height_ / 2 );

    x += c_.center_.GetX();
    y += c_.center_.GetY();
    z += c_.center_.GetZ();

    double zenith = acos(c_.rng_->Uniform(-1, 1));
    double azimuth = c_.rng_->Uniform(0, 2 * I3Constants::pi);

    double energy_nu;

    if ( c_.genspec_ == "Gamma PDF" || c_.genspec_ == "Garching")
    {
        const double alpha = ( c_.genspec_ == "Gamma PDF" ? c_.gammaalpha_ : c_.garchingalpha_);
        const double meanE = ( c_.genspec_ == "Gamma PDF" ? c_.gammameanE_ : c_.garchingmeanE_);

        do
        {
            energy_nu = gsl_ran_gamma( c_.rng_->GSLRng(), alpha + 1 , meanE / (alpha + 1) ) * I3Units::MeV;
        }
        while ( energy_nu > c_.maxE_ || energy_nu < c_.minE_ );

    }
    else if ( c_.genspec_ == "Flat") 
    {
        energy_nu = c_.rng_->Uniform(c_.minE_, c_.maxE_);
    }
    else if ( c_.genspec_ == "Gaussian")
    {
        energy_nu = c_.rng_->Gaus(c_.gaussianmean_, c_.gaussianstddev_);
    }
    else
    {
        log_fatal("Please select a supported generation spectrum");
    }

    log_debug("Neutrino Energy [GeV] %e", energy_nu);

    prim.SetEnergy( energy_nu );
    prim.SetDir( I3Direction( zenith, azimuth ) );
    prim.SetPos( I3Position( x, y, z ) );
    prim.SetTime( 0.0 );
    prim.SetSpeed( I3Constants::c );
    prim.SetLocationType(I3Particle::Anywhere );
    
}

// double SNInjection::GetInteractionWeight( I3Particle & primary )
// {

//     log_debug("=========Get Interaction Weight=========");

//     std::pair<double, double> intersection = GetIntersectionCylinderRotated( primary.GetPos() );

//     if ( intersection.first < 0. ) log_fatal("Your neutrino is traveling backwards. Seems wrong doesn't it");

//     // Dormant code for now. Depending on how we decide to handle injection volume
//     // std::pair<double, double> intersection = GetIntersection( primary.GetPos() );

//     // if ( intersection.first > 0. ) log_fatal("Your neutrino is traveling backwards. Seems wrong doesn't it");

//     log_debug("Distance from endcap [m] %e", intersection.first);
//     log_debug("Distance to endcap [m] %e", intersection.second);

//     intersection.first = std::abs( intersection.first );

//     if ( !std::isfinite(intersection.first) || !std::isfinite(intersection.second) ) log_fatal("You have a neutrino outside the injection volume...... Something weird happened");

//     double w = InteractionProb( primary.GetEnergy(), intersection);

//     log_debug("Position Weight %e", w);
//     log_debug("Intersection First [m] %e", intersection.first);
//     log_debug("Intersection Second [m] %e", intersection.second);

//     return w;

// }

std::vector<I3Particle> SNInjection::GenerateProducts( I3Particle & primary )
{

    log_debug("=========Generate Products=========");

    std::vector<I3Particle> tempproducts;

    I3Particle product1(c_.product1shape, c_.product1type);

    double costhetatargetframe = c_.rng_->Uniform(-1, 1);
    double thetatargetframe = acos(costhetatargetframe);
    double phitargetframe = c_.rng_->Uniform(0, 2 * I3Constants::pi);

    double product1energy = c_.inter->GetE1(primary.GetEnergy(), costhetatargetframe);
    double product1KE = c_.inter->GetKE1( product1energy );

    log_debug("Product 1 kinetic Energy [GeV] %e", product1KE);

    if ( product1KE < 0. ) log_fatal("Negative product1 kinetic energy");

    I3Direction product1dir = RotateToIceCubeFrame( primary.GetDir(), I3Direction( thetatargetframe, phitargetframe ) );

    product1.SetEnergy( product1KE );
    product1.SetDir( product1dir );
    product1.SetPos( primary.GetPos() );
    product1.SetTime( 0.0 );
    product1.SetSpeed( I3Constants::c );
    product1.SetLocationType( I3Particle::InIce );

    tempproducts.push_back(product1);

    I3Particle product2(c_.product2shape, c_.product2type);

    double product2energy = c_.inter->GetE2(primary.GetEnergy(), costhetatargetframe);
    double product2KE = c_.inter->GetKE2( product2energy );

    log_debug("Product 2 kinetic Energy [GeV] %e", product2KE);

    if ( product2KE < 0. ) log_fatal("Negative product2 kinetic energy");

    double psitargetframe = c_.inter->GetPsi( product1energy, product2energy, costhetatargetframe);
    
    I3Direction product2dir = RotateToIceCubeFrame( primary.GetDir(), I3Direction( psitargetframe, phitargetframe ) );

    product2.SetEnergy( product2KE );
    product2.SetDir( product2dir );
    product2.SetPos( primary.GetPos() );
    product2.SetTime( 0.0 );
    product2.SetSpeed( I3Constants::c );
    product2.SetLocationType( I3Particle::InIce );


    tempproducts.push_back(product2);

    return tempproducts;
}

void SNInjection::GenerateParticles(I3MCTreePtr &tree, boost::shared_ptr<SNWeightMap> &weightmap)
{

    I3Particle neutrino(c_.primaryshape, c_.primarytype);
    SNWeight w;
    
    do
    {
        GeneratePrimary(neutrino);
        w.InteractionWeight = intweight->GetInteractionWeight(neutrino);

        if ( !std::isfinite(w.InteractionWeight) )
        {
            log_warn("Weight is not finite!\n"
                     "Generate new primary!\n"
                     "Quantities of old primary\n"
                     "Weight = %e\n"
                     "Neutrino Energy [GeV]= %e\n",
                     w.InteractionWeight, neutrino.GetEnergy());
        }

    } while ( !std::isfinite(w.InteractionWeight) );

    w.GenerationWeight = 1;
    w.OscillationWeight = 1;
    w.TotalWeight = w.InteractionWeight * w.GenerationWeight * w.OscillationWeight;
    w.GenOscWeight = w.GenerationWeight * w.OscillationWeight;
    
    I3MCTree::iterator treeiter = tree->insert(tree->end(), neutrino);
    weightmap->insert( std::make_pair( neutrino, w ) );

    std::vector<I3Particle> products = GenerateProducts( neutrino );

    std::vector<I3Particle>::const_iterator prodit;
    for ( prodit = products.begin(); prodit != products.end(); prodit++)
    {
        tree->append_child(treeiter, *prodit);
    }

}


I3Direction SNInjection::RotateToIceCubeFrame(I3Direction neutrinodir, I3Direction framedir)
{

    I3Direction p(framedir);
    p.RotateZ(-neutrinodir.GetAzimuth());
    p.RotateY(-neutrinodir.GetZenith());
    p.RotateZ(neutrinodir.GetAzimuth());

    return p;

}

// inline void sort(std::pair<double, double> &pair)
// {

//     if (pair.first > pair.second)
//     {
//         double aux = pair.first;
//         pair.first = pair.second;
//         pair.second = aux;
//     }

// }

// std::pair<double, double> SNInjection::GetIntersectionCylinderRotated( const I3Position &p) const
// {

//     std::pair<double, double> h(std::make_pair(NAN, NAN));

//     double x = p.GetX() - c_.center_.GetX();
//     double y = p.GetY() - c_.center_.GetY();
//     double z = p.GetZ() - c_.center_.GetZ();

//     h.first = c_.height_ / 2. - z;
//     h.second = c_.height_ - h.first;

//     return h;

// }

// std::pair<double, double> SNInjection::GetIntersection(const I3Position &p, const I3Direction &dir) const
// {

//     std::pair<double, double> h(std::make_pair(NAN, NAN)), r(std::make_pair(NAN, NAN));

//     double x = p.GetX() - c_.center_.GetX();
//     double y = p.GetY() - c_.center_.GetY();
//     double z = p.GetZ() - c_.center_.GetZ();

//     double sinph = sin(dir.GetAzimuth());
//     double cosph = cos(dir.GetAzimuth());
//     double sinth = sin(dir.GetZenith());
//     double costh = cos(dir.GetZenith());

//     double b = x * cosph + y * sinph;
//     double d = b * b + c_.radius_ * c_.radius_ - x * x - y * y;

//     if (d > 0)
//     {
//         d = sqrt(d);
//         // down-track distance to the endcaps
//         if (costh != 0)
//         {
//             h.first  = (z - c_.height_ / 2) / costh;
//             h.second = (z + c_.height_ / 2) / costh;
//             sort(h);
//         }
//         // down-track distance to the side surfaces
//         if (sinth != 0)
//         {
//             r.first  = (b - d) / sinth;
//             r.second = (b + d) / sinth;
//             sort(r);
//         }
//         // Perfectly horizontal tracks never intersect the endcaps
//         if (costh == 0)
//         {
//             if ((z > -c_.height_ / 2) && (z < c_.height_ / 2))
//                 h = r;
//             else
//                 h = std::make_pair(NAN, NAN);
//             // Perfectly vertical tracks never intersect the sides
//         }
//         else if (sinth == 0)
//         {
//             if (hypot(x, y) >= c_.radius_)
//                 h = std::make_pair(NAN, NAN);
//             // For general tracks, take the last entrace and first exit
//         }
//         else
//         {
//             if (h.first >= r.second || h.second <= r.first)
//                 h = std::make_pair(NAN, NAN);
//             else
//             {
//                 h.first = std::max(r.first, h.first);
//                 h.second = std::min(r.second, h.second);
//             }
//         }
//     }

//     return h;
// }


// double SNInjection::SmallNumExp(double x)
// {

//     if ( std::abs(x) < 1e-16 )
//     {
//         return 1. + x + (0.5 * x * x) + ( x * x * x ) / 6;
//     }
//     else
//     {
//         return exp(x);
//     }

// }

// double SNInjection::InteractionProb(double Enu, std::pair<double, double> distances)
// {

    
//     log_debug("=========Calculating Interaction Probability=========");

//     double xsection = c_.inter->GetTotalCrossSection(Enu);

//     log_debug("Total Cross-Section [m^2] %e", xsection);

//     double totaltargetspervol = targetspervol * c_.inter->GetNumberOfTargets();

//     log_debug("Total No. of Targets per m^3 %e", totaltargetspervol);

//     double exponent_constant = -1. * totaltargetspervol * xsection;

//     log_debug("Exponetial constant [m] %e", exponent_constant);

//     double exponent = exponent_constant * distances.first;

//     double exponent_normal = exponent_constant * ( distances.second + distances.first );

//     log_debug("exp(exponent) %e", SmallNumExp(exponent));

//     // double numer = ( distances.second + distances.first ) * SmallNumExp(exponent);

//     // log_debug("Numerator for Interaction Probability %e", numer);

//     // double denom = 1. - SmallNumExp(exponent_normal);
//     //        denom /= (-1. * exponent_constant);

//     // log_debug("Denominator for Interaction Probability %e", denom);

//     // return ( numer / denom );

//     double weight = -1. * exponent_constant * ( distances.second + distances.first ) * SmallNumExp(exponent);

//     log_debug("Weight %e", weight);

//     return weight;

// }

void SNInjection::DAQ(I3FramePtr frame)
{

    boost::shared_ptr<I3MCTree> mctree(new I3MCTree);

    boost::shared_ptr<SNWeightMap> weights(new SNWeightMap);

    I3MCTree::iterator it = mctree->begin();

    for (int i = 0; i < nopart_; i++)
    {
        log_debug("Number of primaries %d", i );
        GenerateParticles(mctree, weights);

    }

    frame->Put(outputI3MCTreeName_, mctree);
    frame->Put(outputSNWeightMapName_, weights);
    PushFrame(frame);

}


    // particleinfo Ointeraction_nue16O-ep_info = {
    //
    //   I3Particle::Nue,
    //   I3Particle::EMinus,
    //   I3Particle::Proton,
    //   I3Particle::Primary,
    //   I3Particle::StartingTrack,
    //   I3Particle::StartingTrack,
    //   &OInteraction::GetEe,
    //   &OInteraction::GetEp
    //
    // };

    // particleinfo Ointeraction_nue16O-e2p_info = {
    //
    //    I3Particle::Nue,
    //    I3Particle::EMinus,
    //    I3Particle::Proton,
    //    I3Particle::Primary,
    //    I3Particle::StartingTrack,
    //    I3Particle::StartingTrack,
    //    &OInteraction::GetEe,
    //    &OInteraction::GetE2p
    //
    //
    //  };

    // particleinfo Ointeraction_nue16O-epgamma_info = {
    //
    //     I3Particle::Nue,
    //     I3Particle::EMinus,
    //     I3Particle::Proton,
    //     I3Particle::Primary,
    //     I3Particle::StartingTrack,
    //     I3Particle::StartingTrack,
    //     &OInteraction::GetEe,
    //     &OInteraction::GetEpGamma
    //
    //
    //   };

    // particleinfo Ointeraction_nuebar16O-eplusn_info = {
    //
    //     I3Particle::NueBar,
    //     I3Particle::EPlus,
    //     I3Particle::Neutron,
    //     I3Particle::Primary,
    //     I3Particle::StartingTrack,
    //     I3Particle::StartingTrack,
    //     &OInteraction::GetEe,
    //     &OInteraction::GetEn
    //
    //   };

    // particleinfo Ointeraction_nuebar16O-eplus_info = {
    //
    //     I3Particle::NueBar,
    //     I3Particle::EPlus,
    //     I3Particle::NueBar,
    //     I3Particle::Primary,
    //     I3Particle::StartingTrack,
    //     I3Particle::Dark,
    //     &OInteraction::GetEe,
    //     &OInteraction::GetZeroE;
    //
    //   };

    // particleinfo Ointeraction_nuebar16O-eplusngamma_info = {
    //
    //     I3Particle::NueBar,
    //     I3Particle::EPlus,
    //     I3Particle::Neutron,
    //     I3Particle::Primary,
    //     I3Particle::StartingTrack,
    //     I3Particle::StartingTrack,
    //     &OInteraction::GetEe,
    //     &OInteraction::GetEnGamma,
    //
    //   };

    // particleinfo Ointeraction_nue18O_info = {
    //
    //     I3Particle::Nue,
    //     I3Particle::EMinus,
    //     I3Particle::Nue,
    //     I3Particle::Primary,
    //     I3Particle::StartingTrack,
    //     I3Particle::Dark,
    //     &OInteraction::GetEe
    //
    //   };

