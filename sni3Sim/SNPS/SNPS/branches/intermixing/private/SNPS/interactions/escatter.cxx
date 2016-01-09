// #include <SNPS/interactions/interaction.h>
// #include <SNPS/interactions/escatter.h>
// #include <SNPS/constants/SNPSConstants.h>

// #include <sim-services/I3SimConstants.h>

// #include <dataclasses/I3Constants.h>

// #include <icetray/I3Units.h>

// #include <iostream>
// #include <cmath>

// using namespace SNPSConstants;

/***
 * private
 */
// template<typename neutrino>
// double EScatter<neutrino>::Jacobian(double Enu, double costheta, double Ee) const {
  
//   double pe2 = Ee * Ee - m_E_sq;
//   double j = pe2 / (Enu * costheta * m_E);
  
//   if ( j != j ) return 0;
//   else return j;
// }

/***
 * public
 */
 
// EScatter::EScatter(){
//  
//   // TODO: Check HDF5 file for total cross-sections, if not there generate and write to file
// 
//}

// template<typename neutrino>
// double EScatter<neutrino>::GetE1(double Enu, double costheta){  
//   // From kinematics
//   double nomin = pow(Enu * costheta, 2.);
//   double denom = pow(Enu + m_E, 2.);

//   return m_E * (1. + nomin/denom) / (1. - nomin/denom);
// }

// template<typename neutrino>
// double EScatter<neutrino>::GetE2(double Enu, double Ee){
//   // Energy conversation
//   return (Enu + m_E - Ee);
// }


// template<typename neutrino>
// double EScatter<neutrino>::GetKE1(double E1){
//   return (E1 - m_E);
// }


// template<typename neutrino>
// double EScatter<neutrino>::GetKE2(double E2){
//   return E2;
// }


// template<typename neutrino>
// double EScatter<neutrino>::GetPsi(double Enu, double E1, double theta){

//   double mom1 = sqrt( E1 * E1 - m_E * m_E);
//   double sintheta = sin(theta);
//   return asin( -mom1 * sintheta/Enu );

// }

// template<typename neutrino>
// double EScatter<neutrino>::GetTotalCrossSection(double Enu){

//   // Coefficient * Enu
//   double xs_total = G_Fermi_sq * m_E * Enu / (2. * I3Constants::pi);
  
//   // Flavor- and type-dependent coefficient
//          // xs_total *= (1. + 4. * sinsqthetaW + (16./3.) * sinsqthetaW_sq);
//          xs_total *= (neutrino::coefficient1 + neutrino::coefficient2 + neutrino::coefficient3 );

//   // double conversion = 5.07e+15 * 0.01; // per GeV;

//   // Convert energy to length
//   xs_total *= (hbarc * hbarc);

//   return xs_total ;

// }


// template<typename neutrino>
// double EScatter<neutrino>::GetDifferentialCrossSection(double Enu, double costheta, double &Ee) {

//   if (costheta < 0.) {
//     Ee = 0;
//     return 0;
//   }
  
//   Ee = GetE1 (Enu, costheta);

//   //double Epsiplus = -sinsqthetaW;
//   //double Epsiminus = -0.5 - sinsqthetaW;
  
//   double y = ( Ee - m_E) / Enu;
//   double Ymax = 1. - ( m_E_sq / ( 2 * m_E * Enu + m_E_sq ) );

//   if (y > Ymax || y <= 0.) {
//     return 0;
//   }
  
//   double xsec = 2. * G_Fermi_sq * m_E_sq * Enu / I3Constants::pi;
//          // xsec *= (pow(-(1./2.) - sinsqthetaW,2) + pow(-sinsqthetaW * (1 - y),2) - y * (-(1./2.) - sinsqthetaW) * -sinsqthetaW * m_E  / Enu);
//          xsec *= (pow(neutrino::epsilon1,2) + pow(neutrino::epsilon2 * (1 - y),2) - y * neutrino::epsilon1 * neutrino::epsilon2 * m_E  / Enu);


//   // Taken from Lausanne implementation 
//   xsec *= Jacobian(Enu, costheta, Ee); //not sure where this comes from

//   xsec *= (hbarc * hbarc);

//   //double centimeter = 5.07e+15 * 0.01; // per GeV;
  
//   //xsec /= (centimeter*centimeter); //CORRECTION TO CM^2

//   //xsec *= I3Units::cm2;

//   return xsec;
// }

// template<typename neutrino>
// double EScatter<neutrino>::GetMaximalCrossSection (double Enu){
  
//   double Ee = 0;
//   double xs_max = GetDifferentialCrossSection( Enu, -1., Ee);
//   return xs_max;
// }


// template<typename neutrino>
// double EScatter<neutrino>::GetRandomEvent(double Enu, double &cospsi, double &Ee){

//   // double cs=0;
//   // double y;
//   // double Ymax = 1 - ( m_E_sq / ( 2 * m_E * Enu + m_E_sq));

//   // do {
//   //   y = SNInjection::rng_->Uniform(0., Ymax);
//   //   Ee = y * Enu + m_E;
//   //   cs = GetDifferentialCrossSection(Enu, -1., Ee);
//   // } while (cs / GetMaximalCrossSection (Enu) < SNInjection::rng_->Uniform());

//   // cospsi = ((Enu + m_E) * (Ee - m_E)) / (Enu * sqrt (Ee*Ee - m_E_sq));
  
//   // return cs;

//   return 0.0;
// }

