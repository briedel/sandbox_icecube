#ifndef ESCATTER_H
#define ESCATTER_H

#include <SNPS/interactions/interaction.h>
#include <SNPS/interactions/escatter.h>
#include <SNPS/constants/SNPSConstants.h>

#include <sim-services/I3SimConstants.h>

#include <dataclasses/I3Constants.h>

#include <icetray/I3Units.h>

#include <iostream>
#include <cmath>


/*
 * Class to calculate the cross-sections and energies
 * for neutrino-electron scattering. Taken from Marciano and Parsa,
 * arXiv:hep-ph/0403168
 * http://arxiv.org/abs/hep-ph/0403168
 */

using namespace SNPSConstants;

template<typename neutrino>
class EScatter : public Interaction
{

private:

    /*
     * Jacobian for correction of differential cross-section
     */
    double Jacobian(double Enu, double costheta, double Ee) const
    {

        double pe2 = Ee * Ee - m_E_sq;
        double j = pe2 / (Enu * costheta * m_E);

        if ( std::isnan(j)  ) return 0;
        else return j;
    };

public:

    EScatter() {};

    virtual ~EScatter() {};

    double GetE1(double Enu, double costheta)
    {
        // From kinematics
        double nomin = pow(Enu * costheta, 2.);
        double denom = pow(Enu + m_E, 2.);

        return m_E * (1. + nomin / denom) / (1. - nomin / denom);
    };

    double GetE2(double Enu, double costheta)
    {
        // Energy conversation
        double E1 = GetE1(Enu, costheta);
        return (Enu + m_E - E1);
    };

    double GetKE1(double E1)
    {
        return (E1 - m_E);
    };

    double GetKE2(double E2)
    {
        return E2;
    };

    double GetPsi(double Enu, double E1, double costheta)
    {
        double theta = acos(costheta);
        double mom1 = sqrt( E1 * E1 - m_E * m_E);
        double sintheta = sin(theta);
        return asin( -mom1 * sintheta / Enu );

    };

    double GetTotalCrossSection(double Enu)
    {

        // Coefficient * Enu
        double xs_total = G_Fermi_sq * m_E * Enu / (2. * I3Constants::pi);

        // Flavor- and type-dependent coefficient
        // xs_total *= (1. + 4. * sinsqthetaW + (16./3.) * sinsqthetaW_sq);
        xs_total *= (neutrino::coefficient1 + neutrino::coefficient2 + neutrino::coefficient3 );

        // double conversion = 5.07e+15 * 0.01; // per GeV;

        // Convert energy to length
        xs_total *= (hbarc * hbarc);

        return xs_total ;

    };

    double GetDifferentialCrossSection(double Enu, double costheta)
    {

        if (costheta < 0.)
        {
            double Ee = 0;
            return 0;
        }

        double Ee = GetE1(Enu, costheta);

        //double Epsiplus = -sinsqthetaW;
        //double Epsiminus = -0.5 - sinsqthetaW;

        double y = ( Ee - m_E) / Enu;
        double Ymax = 1. - ( m_E_sq / ( 2 * m_E * Enu + m_E_sq ) );

        if (y > Ymax || y <= 0.)
        {
            return 0;
        }

        double xsec = 2. * G_Fermi_sq * m_E_sq * Enu / I3Constants::pi;
        // xsec *= (pow(-(1./2.) - sinsqthetaW,2) + pow(-sinsqthetaW * (1 - y),2) - y * (-(1./2.) - sinsqthetaW) * -sinsqthetaW * m_E  / Enu);
        xsec *= (pow(neutrino::epsilon1, 2) + pow(neutrino::epsilon2 * (1 - y), 2) - y * neutrino::epsilon1 * neutrino::epsilon2 * m_E  / Enu);


        // Taken from Lausanne implementation
        xsec *= Jacobian(Enu, costheta, Ee); //not sure where this comes from

        xsec *= (hbarc * hbarc);

        //double centimeter = 5.07e+15 * 0.01; // per GeV;

        //xsec /= (centimeter*centimeter); //CORRECTION TO CM^2

        //xsec *= I3Units::cm2;

        return xsec;
    };

    double GetNumberOfTargets()
    {

        return 10.;

    };

    double GetMaximalCrossSection(double Enu)
    {

        double Ee = 0;
        double xs_max = GetDifferentialCrossSection( Enu, -1.);
        return xs_max;

    };

    double GetRandomEvent(double Enu, double &costheta, double &E1)
    {

        return 0;
    };

};


#endif // ESCATTER_H
