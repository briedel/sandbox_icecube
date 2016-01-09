// #include <cross-sections/Ointeraction.h>

// #include <SNPS/SNInjection.h>
// #include <SNPS/constants/SNPSConstants.h>

// #include <sim-services/I3SimConstants.h>

// #include <dataclasses/I3Constants.h>

// #include <icetray/I3Units.h>

// #include <iostream>
// #include <cmath>

// using namespace SNPSconstants;

// /***
//  * private
//  */


// OInteraction::OInteraction(){
// //     kHome (gSystem->Getenv ("HOME")),
// //     gr1 ((kHome+SIMGEANTPATH+"nu_e.data").c_str ()),
// //     kIntTotCs (0.),
// //     kThreshold(0.0104),
// //     kBinning(1000)
// // {
// //   cout << "   OXYGEN  ELECTRON NEUTRINO:" << endl;
// //
// //   double stepsize = (constants::kIBDMaximum - constants::kIBDThreshold) / double (kBinning);
// //   for (double E = constants::kIBDThreshold; E < constants::kIBDMaximum; E += stepsize) {
// //     if (E < kThreshold) continue;
// //     kIntTotCs += stepsize * gr1.Eval(E);
// //   }
// }



// /***
//  * public
//  */

// double OInteraction::GetE1(double Enu, double costheta) {
//   double masses = m_E_sq + m_O_sq - m_16F_sq;
//   return (masses + 2. * m_O * Enu) / (2. * m_O + Enu * costheta);
//  }

// double OInteraction::GetTotalCrossSection(double Enu){
//   if (Enu < kThreshold) return 0;
//   double xs_total = gr1.Eval(Enu);
//   if (xs_total<0.) xs_total = 0.;
//   return xs_total;
// }

// double OInteraction::GetDifferentialCrossSection(double Enu, double costheta, double &Ee){
//   if (Enu < kThreshold) {
//     Ee = 0;
//     return 0;
//   }
//   // not the whole angular range is allowed:
//   double THmax = -1e6, THmin = 1e6;
//   for (double testtheta = -1.; testtheta <= 1.; testtheta += 0.01) {
//     double E = GetEe (Enu, testtheta);
//     if (E <= 0.) continue;
//     THmax = max (testtheta, THmax);
//     THmin = min (testtheta, THmin);
//   }
//   Ee = GetEe (Enu, costheta);

//   if (Ee <= 0.) {
//     Ee = 0;
//     return 0;
//   }

//   double xsec = gr1.Eval(Enu);
//   return max (0., xsec / (THmax - THmin)); // Divided by the range of Energy Ee
// }

// double OInteraction::GetRandomEvent(double Enu, double &cospsi, double &Ee){

//   double cs = GetDifferentialCrossSection(Enu, -1., Ee);
//   cospsi = (m_O_sq + m_O_sq - pow(kAverageMass_nu, 2) + 2 * Enu * m_O - 2 * m_O_sq * Ee - 2 * Ee * Enu ) / (- 2 * Ee) ;
//   return cs;
// }
