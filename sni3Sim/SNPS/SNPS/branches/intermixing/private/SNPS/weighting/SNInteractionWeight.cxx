#include <SNPS/weighting/SNInteractionWeight.h>

#include <SNPS/constants/SNPSConstants.h>

#include <cmath>

using namespace SNPSConstants;

double SNInteractionWeight::GetInteractionWeight( const I3Particle & primary )
{

    log_debug("=========Get Interaction Weight=========");

    std::pair<double, double> intersection = GetIntersectionCylinderRotated( primary.GetPos() );

    if ( intersection.first < 0. ) log_fatal("Your neutrino is traveling backwards. Seems wrong doesn't it");

    // Dormant code for now. Depending on how we decide to handle injection volume
    // std::pair<double, double> intersection = GetIntersection( primary.GetPos() );

    // if ( intersection.first > 0. ) log_fatal("Your neutrino is traveling backwards. Seems wrong doesn't it");

    log_debug("Distance from endcap [m] %e", intersection.first);
    log_debug("Distance to endcap [m] %e", intersection.second);

    intersection.first = std::abs( intersection.first );

    if ( !std::isfinite(intersection.first) || !std::isfinite(intersection.second) ) log_fatal("You have a neutrino outside the injection volume...... Something weird happened");

    double w = InteractionProb( primary.GetEnergy(), intersection);

    log_debug("Position Weight %e", w);
    log_debug("Intersection First [m] %e", intersection.first);
    log_debug("Intersection Second [m] %e", intersection.second);

    return w;

}

double SNInteractionWeight::InteractionProb(double Enu, std::pair<double, double> distances)
{

    
    log_debug("=========Calculating Interaction Probability=========");

    double xsection = c_.inter->GetTotalCrossSection(Enu);

    log_debug("Total Cross-Section [m^2] %e", xsection);

    double totaltargetspervol = targetspervol * c_.inter->GetNumberOfTargets();

    log_debug("Total No. of Targets per m^3 %e", totaltargetspervol);

    double exponent_constant = -1. * totaltargetspervol * xsection;

    log_debug("Exponetial constant [m] %e", exponent_constant);

    double exponent = exponent_constant * distances.first;

    double exponent_normal = exponent_constant * ( distances.second + distances.first );

    log_debug("exp(exponent) %e", SmallNumExp(exponent));

    // double numer = ( distances.second + distances.first ) * SmallNumExp(exponent);

    // log_debug("Numerator for Interaction Probability %e", numer);

    // double denom = 1. - SmallNumExp(exponent_normal);
    //        denom /= (-1. * exponent_constant);

    // log_debug("Denominator for Interaction Probability %e", denom);

    // return ( numer / denom );

    double weight = -1. * exponent_constant * ( distances.second + distances.first ) * SmallNumExp(exponent);

    log_debug("Weight %e", weight);

    return weight;

}

inline void sort(std::pair<double, double> &pair)
{

    if (pair.first > pair.second)
    {
        double aux = pair.first;
        pair.first = pair.second;
        pair.second = aux;
    }

}

std::pair<double, double> SNInteractionWeight::GetIntersectionCylinderRotated( const I3Position &p) const
{

    std::pair<double, double> h(std::make_pair(NAN, NAN));

    double x = p.GetX() - c_.center_.GetX();
    double y = p.GetY() - c_.center_.GetY();
    double z = p.GetZ() - c_.center_.GetZ();

    h.first = c_.height_ / 2. - z;
    h.second = c_.height_ - h.first;

    return h;

}

std::pair<double, double> SNInteractionWeight::GetIntersection(const I3Position &p, const I3Direction &dir) const
{

    std::pair<double, double> h(std::make_pair(NAN, NAN)), r(std::make_pair(NAN, NAN));

    double x = p.GetX() - c_.center_.GetX();
    double y = p.GetY() - c_.center_.GetY();
    double z = p.GetZ() - c_.center_.GetZ();

    double sinph = sin(dir.GetAzimuth());
    double cosph = cos(dir.GetAzimuth());
    double sinth = sin(dir.GetZenith());
    double costh = cos(dir.GetZenith());

    double b = x * cosph + y * sinph;
    double d = b * b + c_.radius_ * c_.radius_ - x * x - y * y;

    if (d > 0)
    {
        d = sqrt(d);
        // down-track distance to the endcaps
        if (costh != 0)
        {
            h.first  = (z - c_.height_ / 2) / costh;
            h.second = (z + c_.height_ / 2) / costh;
            sort(h);
        }
        // down-track distance to the side surfaces
        if (sinth != 0)
        {
            r.first  = (b - d) / sinth;
            r.second = (b + d) / sinth;
            sort(r);
        }
        // Perfectly horizontal tracks never intersect the endcaps
        if (costh == 0)
        {
            if ((z > -c_.height_ / 2) && (z < c_.height_ / 2))
                h = r;
            else
                h = std::make_pair(NAN, NAN);
            // Perfectly vertical tracks never intersect the sides
        }
        else if (sinth == 0)
        {
            if (hypot(x, y) >= c_.radius_)
                h = std::make_pair(NAN, NAN);
            // For general tracks, take the last entrace and first exit
        }
        else
        {
            if (h.first >= r.second || h.second <= r.first)
                h = std::make_pair(NAN, NAN);
            else
            {
                h.first = std::max(r.first, h.first);
                h.second = std::min(r.second, h.second);
            }
        }
    }

    return h;
}


double SNInteractionWeight::SmallNumExp(double x)
{

    if ( std::abs(x) < 1e-16 )
    {
        return 1. + x + (0.5 * x * x) + ( x * x * x ) / 6;
    }
    else
    {
        return exp(x);
    }

}

