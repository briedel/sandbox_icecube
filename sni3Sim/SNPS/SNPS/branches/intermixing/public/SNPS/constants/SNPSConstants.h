#ifndef SNPSCONSTANTS_H_INCLUDE
#define SNPSCONSTANTS_H_INCLUDE

#include <icetray/I3Units.h>
#include <dataclasses/I3Constants.h>
#include <sim-services/I3SimConstants.h>
#include <cmath>


namespace SNPSConstants{

  // Density of ice
  static const double rho_ice = 0.92 * I3Units::g / I3Units::cm3;

  // Molar mass of ice
  static const double M_ice = 18.02 * I3Units::g;

  // Avogadros Number
  static const double N_A = I3Constants::NA;

  // Expected number of targets per unit volume
  static const double targetspervol = rho_ice * N_A / M_ice;

  // Mass of proton in natural units
  static const double m_P = I3SimConstants::m_p * I3Constants::c * I3Constants::c;
  
  // Mass of electron in natural units
  static const double m_E = I3SimConstants::m_e * I3Constants::c * I3Constants::c;
  
  // Mass of neutron in natural units
  static const double m_N = I3SimConstants::m_n * I3Constants::c * I3Constants::c;
  
  // Fermi constant
  static const double G_Fermi = I3SimConstants::G_Fermi;
  
  // Fermi constant squared
  static const double G_Fermi_sq = G_Fermi * G_Fermi;
  
  // Mass difference between neutron and proton
  static const double Delta = (m_N - m_P);
 
  // Average mass between neutron and proton
  static const double M = (m_N + m_P) / 2.;
  
   // Average mass between neutron and proton squared
  static const double M_sq = M * M;
  
  // Mass of electron in natural units squared
  static const double m_E_sq = m_E * m_E;
  
  // Mass of proton in natural units squared
  static const double m_P_sq = m_P * m_P;

  // Mass of neutron in natural units squared
  static const double m_N_sq = m_N * m_N;

  // Small delta needed for inverse beta decay calculation
  static const double delta = (m_N_sq - m_P_sq - m_E_sq) / (2. * m_P);
  
  // Mass of oxygen nucleus
  static const double m_O = 8. * m_P + 8. * m_N;
  
  // Mass of oxygen nucleus squared
  static const double m_O_sq = m_O * m_O;
  
  // Mass of fluorine-16 nucleus
  static const double m_16F = 9. * m_P + 7. * m_N;
  
  // Mass of fluorine-16 nucleus squared
  static const double m_16F_sq = m_16F * m_16F;
  
  // Mass of oxygen-15 nucleus
  static const double m_15O = 8. * m_P + 7. * m_N;
  
  // Mass of oxygen-15 nucleus squared
  static const double m_15O_sq = m_15O * m_15O;
  
  // Mass of pion in natural units
  static const double m_Pi = 139.57018 * I3Units::MeV;
  
  // Constants for inverse beta decay calculation
  static const double kappa_N = -1.913;
  
  static const double kappa_P = 1.792;
  
  static const double eta_AMM = kappa_P - kappa_N;
  
  static const double M_V_sq = 0.71 * I3Units::GeV * I3Units::GeV;
  
  static const double M_A_sq = 1. * I3Units::GeV * I3Units::GeV;
  
  static const double minEnu = 1.806 * I3Units::MeV;
  
  // Weinbery angles
  static const double sinsqthetaW = I3SimConstants::sinsq_theta_W;
  
  static const double sinsqthetaW_sq = I3SimConstants::sinsq_theta_W * I3SimConstants::sinsq_theta_W;
  
  // Thresholds for interactions
  static const double thresholdIBD = 1.806 * I3Units::MeV;
  
  static const double thresholdOnu = 13. * I3Units::MeV;
  
  static const double thresholdOnubar = 18. * I3Units::MeV;
  
  static const double threshold18Onu = 6.5 * I3Units::MeV;

  // Hbar c
  static const double hbarc = I3SimConstants::hbarc;


  // Conversion from ergs to MeV
  static const double MeVperergs = 1. / 1.60217657e-6;

  /// SPECIAL NOTE: Needs to be > 1.8181 MeV because cross-sections
  /// for 1.8181 > energies > 1.806 the cross-section is too small to
  /// be properly represented by double precision 

  // Thresholds for interactions
  static const double minEnu_IBD = 1.8181 * I3Units::MeV;

  static const double minEnu_escatter = 0. * I3Units::MeV;

  static const double minEnu_nu16O = 15.4 * I3Units::MeV;

  static const double minEnu_18O = 1.66 * I3Units::MeV;

  static const double minEnu_nubar16O = 11.4 * I3Units::MeV;

}

#endif // SNPSCONSTANTS_H
