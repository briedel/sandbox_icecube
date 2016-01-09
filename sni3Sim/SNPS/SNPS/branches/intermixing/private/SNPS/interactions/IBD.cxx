#include <SNPS/interactions/IBD.h>

#include <SNPS/constants/SNPSConstants.h>

#include <sim-services/I3SimConstants.h>

#include <dataclasses/I3Constants.h>

#include <icetray/I3Units.h>

#include <iostream>
#include <cmath>

using namespace SNPSConstants;

/***
 * private
 */

double IBD::dSigDt(const double sMinusU, const double sMinusMnuc, const double t) const
{

    // for anue + p -> e+ + n , sMinusU = s - u  and  sMinusMnuc = s - m_p^2
    // for nue  + n -> e- + p , sMinusU = u - s  and  sMinusMnuc = s - m_n^2
    // where s = (p_nu + p_p)^2 and u = (p_nu - p_n)^2

    const double numer = G_Fermi_sq * pow(I3SimConstants::cos_theta_C, 2);
    const double denom = ( 2. * I3Constants::pi ) * sMinusMnuc * sMinusMnuc;

    return ( (numer / denom) * MtxElm(sMinusU, t) );
}

double IBD::MtxElm(const double sMinusU, const double t) const
{

    // store multiply used variables to reduce number of calculations
    const double t4m      = t / (4. * M_sq);
    const double t2       = t * t;

    const double fdenomB  = 1. - ( t / M_V_sq );
    const double fdenom   = ( 1. - t4m ) * fdenomB * fdenomB;
    const double g1denom  = 1. - (t / M_A_sq);
    const double g2denom  = m_Pi * m_Pi - t;

    const double f1       = (1. - ( ( 1. + eta_AMM ) * t4m) ) / fdenom;
    const double f2       = eta_AMM / fdenom;
    const double g1       = -1.270 / (g1denom * g1denom);
    const double g2       = (2. * M * g1) / g2denom;

    const double f12      = f1 * f1;
    const double f124     = 4. * f12;
    const double f22      = f2 * f2;
    const double g12      = g1 * g1;
    const double g124     = 4. * g12;
    const double g22      = g2 * g2;
    const double g224meM2 = 4. * g22 * m_E_sq / M_sq;

    const double g1cFsumR   = g1 * (f1 + f2);
    const double f1cf2      = f1 * f2;
    const double g1cg2      = g1 * g2;
    const double f1cf2R8    = f1cf2 * 8.;
    const double g1cg2R16me = g1cg2 * 16. * m_E_sq;

    // Calculate elements
    const double mat = MAterm(t, t2, f124, f22, g124,
                              g224meM2, f1cf2R8, g1cg2R16me, g1cFsumR);
    const double mbt = MBterm(t, f1cf2, g1cg2, g1cFsumR, f22);
    const double mct = MCterm(t, f124, f22, g124);

    // Combine to form scattering matrix
    const double M2  = mat - (sMinusU * (mbt - (sMinusU * mct)));

    return std::abs( M2 / 16. );
}

double IBD::MAterm(const double t,
                   const double t2,
                   const double f124,
                   const double f22,
                   const double g124,
                   const double g224meM2,
                   const double f1cf2R8,
                   const double g1cg2R16me,
                   const double g1cFsumR)
{

    // for speed purposes, no check that |t|^2 = t2 is performed

    // store multiply used terms
    const double tmme2 = t - m_E_sq;
    const double tpme2 = t + m_E_sq;

    double r1  = f124 * ( (4. * M_sq) + tpme2);
    r1 += g124 * (-(4. * M_sq) + tpme2);
    r1 += f22  * ( (t2 / M_sq) + 4. * tpme2 );
    r1 += g224meM2 * t;
    r1 += f1cf2R8 * (2. * t + m_E_sq);
    r1 += g1cg2R16me;
    r1 *= tmme2;

    double r2  = (f124 + (t * (f22 / (M_sq) ))) * ( (4. * M_sq) + tmme2);
    r2 +=  g124 * ( (4. * M_sq) - tmme2);
    r2 += g224meM2 * tmme2;
    r2 += f1cf2R8 * (2. * t - m_E_sq);
    r2 += g1cg2R16me;
    r2 *= Delta * Delta;

    double r3 = 32. * m_E_sq * M * Delta * g1cFsumR;

    return ( r1 - r2 - r3 );
}

double IBD::MBterm(const double t,
                   const double f1cf2,
                   const double g1cg2,
                   const double g1cFsumR,
                   const double f22)
{

    double bterm  = 16. * t * g1cFsumR;
    bterm += ( 4. * m_E_sq * Delta * (f22 + (f1cf2 + 2.*g1cg2)) ) / M;

    return bterm;
}

double IBD::MCterm(const double t,
                   const double f124,
                   const double f22,
                   const double g124)
{

    return ( f124 + g124 - (t * ( f22 / M_sq ) ) );
}

double IBD::RadiativeCorr(const double Ee) const
{

    double rc  = 6. + (1.5 * log( m_P / ( 2. * Ee ) ) );
    rc += 1.2 * pow( ( m_E / Ee ), 1.5 );
    rc *= I3SimConstants::alpha / I3Constants::pi;
    rc += 1.;

    return rc;
}

double IBD::FinalStateCorr(const double Ee) const
{

    double eta  = 2. * I3Constants::pi * I3SimConstants::alpha;
    eta /= sqrt( 1. - ( m_E_sq / ( Ee * Ee ) ) );

    double expn = exp( -1. * eta );

    return ( eta / ( 1. - expn ) );
}

double IBD::Jacobian(double Enu, double costheta, double Ee) const
{

    double epsilon = Enu / m_P;
    double pe = sqrt( Ee * Ee - m_E_sq);

    if ( std::isnan(pe)  ) return 0;
    else return pe * epsilon / ( 1. + epsilon * ( 1. - Ee / pe * costheta ) );
}

void IBD::GenerateTotalCrossSection(double Emin, double Emax, int nobins)
{

    double stepsize  = (Emin - Emax) / double (nobins);

    std::vector<double> Enu_vector;
    std::vector<double> totcs_vector;


    for (double Enu = Emin; Enu <= Emax; Enu += stepsize)
    {

        Enu_vector.push_back(Enu);

        // double xsec1 = GetDifferentialCrossSection (Enu, -1, Ee);
        // double xsec2 = GetDifferentialCrossSection (Enu,  1, Ee);

        double totcs = GetTotalCrossSection(Enu);
        totcs_vector.push_back(totcs);
        inteTotCs += totcs * stepsize;

    }
}

/***
 * public
 */

// IBD::IBD(){

//   // TODO: implement check if the total cross-section is in the input file, if not generate the table and write to file


// }

double IBD::GetTotalCrossSection(double Enu)
{

    double Ee;
    double totcs = 0;
    double resolution = 0.0001;

    for (double i = -1.; i <= 1.; i += resolution)
    {

        double cs = GetDifferentialCrossSection(Enu, i);

        totcs += cs * resolution;

    }

    return totcs;

}

double IBD::GetDifferentialCrossSection(double Enu, double costheta)
{
    double Ee = GetE1(Enu, costheta);

    if (Enu < 1.806 * I3Units::MeV) return 0.;

    const double sMinusU = ( (2. * m_P * (Enu + Ee) ) - m_E_sq );

    const double sMinusMp2 = 2. * m_P * Enu;

    double Ee2MinusEM2 = sqrt( Ee * Ee - m_E_sq );

    if ( std::isnan(Ee2MinusEM2) ) Ee2MinusEM2 = 0;

    const double t = m_N_sq - m_P_sq - 2. * m_P * (Enu - Ee); // m_E_sq - 2 * Enu * ( Ee - Ee2MinusEM2 * costheta);

    double xsec = 2. * m_P * dSigDt(sMinusU, sMinusMp2, t);

    // apply correction factors
    const double rdcf = RadiativeCorr(Ee);

    // const double fscf = FinalStateCorr(Ee);    no final state correction, this only is used for v + n -> e- + p
    xsec *= rdcf;

    const double J = Jacobian( Enu, costheta, Ee );

    xsec *= J;

    // Convert from energy to length
    xsec *= (hbarc * hbarc);

    if ( xsec < 0 || std::isnan(xsec) ) xsec = 0.;

    return xsec;
}

double IBD::GetE1(double Enu, double costheta)
{

    double epsilon = Enu / m_P;
    double kappa = ( ( 1. + epsilon ) * ( 1. + epsilon ) ) - ( ( epsilon * costheta ) * ( epsilon * costheta ) );

    double Ee = ( Enu - delta ) * ( 1. + epsilon );
    Ee += epsilon * costheta * sqrt( ( Enu - delta ) * ( Enu - delta ) - m_E_sq * kappa );
    Ee /= kappa;

    if ( Ee < m_E || std::isnan(Ee) ) Ee = m_E;

    return Ee;
}

double IBD::GetKE1(double E1)
{
    return E1 - m_E;
}

double IBD::GetE2(double Enu, double costheta)
{
    double E1 = GetE1(Enu, costheta);
    double En = Enu + m_P - E1;
    if ( En < m_N | std::isnan(En) ) En = m_N;
    return En;
}

double IBD::GetKE2(double E2)
{
    return E2 - m_N;
}

double IBD::GetPsi(double E1, double E2, double costheta)
{
    double theta = acos(costheta);
    double mom1 = sqrt( E1 * E1 - m_E * m_E );
    double mom2 = sqrt( E2 * E2 - m_N * m_N );
    double sinphi = (mom1 * sin(theta)) / ( -mom2 );
    return asin(sinphi);
}

double IBD::GetNumberOfTargets()
{

    return 2.;

}


double IBD::GetRandomEvent(double Enu, double &costheta, double &E1)
{
    // TODO: FINISH THIS
    return 0;

}

double IBD::GetMaximalCrossSection(double Enu)
{

    return Enu;

};

