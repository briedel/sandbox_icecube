#ifndef IBD_H
#define IBD_H

#include <SNPS/interactions/interaction.h>

#include <vector>

/*
 * Class to calculate the cross-sections and energies
 * for inverse beta-decay. Taken from Strumia and Vissani,
 * arXiv:astro-ph/0302055
 * http://arxiv.org/abs/astro-ph/0302055
 */


class IBD : public Interaction
{

private:
    double inteTotCs;

    double inttotcrosssection;

    /*
     * Returns the differential cross-section in terms of
     * the Mandelstam variables, in the case t, Eqn. 3
     */
    double dSigDt(const double sMinusU, const double sMinusMnuc, const double t) const;

    /*
     * Returns the matrix element for the weak interactions
     * needed to determine the cross-section, , Eqn. 5
     */
    double MtxElm(const double sMinusU, const double t) const;

    /*
     * Returns the 16-times A(t)-term in the matrix element, Eqn. 6
     */
    static double MAterm(const double t,
                         const double t2,
                         const double f124,
                         const double f22,
                         const double g124,
                         const double g224meM2,
                         const double f1cf2R8,
                         const double g1cg2R16me,
                         const double g1cFsumR);

    /*
     * Returns the 16 times B(t)-term in the matrix element, Eqn. 6
     */
    static double MBterm(const double t,
                         const double f1cf2,
                         const double g1cg2,
                         const double g1cFsumR,
                         const double f22);

    /*
     * Returns the 16-times C(t)-term in the matrix element, Eqn. 6
     */
    static double MCterm(const double t,
                         const double f124,
                         const double f22,
                         const double g124);

    /*
     * Returns the radiative correction term, Eqn. 14
     * Only valid for Enu << m_p!
     */
    double RadiativeCorr(const double Ee) const;

    /*
     * Returns the final state correction, Eqn. 15
     * Only relevant for nu_e + n -> e- + p
     * Sommerfeld factor; correction for final state interactions.
     */
    double FinalStateCorr(const double Ee) const;

    /*
     * Returns the Jacobian correction to the cross-section
     * Taken from Lausanne Group Code
     */
    double Jacobian(double E_nu, double costheta, double Ee) const;

    void GenerateTotalCrossSection(double Emin, double Emax, int nobins);

public:

    IBD() {};

    virtual ~IBD() {};

    /*
     * Returns energy of first product
     * Assumed to be positron
     */
    double GetE1(double Enu, double costheta);

    /*
     * Returns energy of second product
     * Assumed to be neutron
     */
    double GetE2(double Enu, double costheta);


    /*
     * Returns kinetic energy of first product
     * Needed because GEANT4+CLSim only work with
     * kinetic energies
     */
    double GetKE1(double E1);

    /*
     * Returns kinetic energy of second product
     * Needed because GEANT4+CLSim only work with
     * kinetic energies
     */
    double GetKE2(double E2);

    /*
     * Returns scattering angle of second product
     * Needed because for MCTree wants a direction
     */
    double GetPsi(double Enu, double E1, double costheta);

    /*
     * Return the total cross-section for a given Enu
     * Integrates differential cross-section over angle
     * using rectangle method
     * Future TODO: Improve integration method
     */
    double GetTotalCrossSection(double Enu);

    /*
     * Return the differential cross-section for a given Enu
     * and cos of the scattering angle of the first product
     */
    double GetDifferentialCrossSection(double Enu, double costheta);


    /*
     * Return the number of targets = 2
     */
    double GetNumberOfTargets();


    /*
     * Return the cross-section for given event
     * Actually needed?
     * TODO: Finish
     */
    double GetRandomEvent(double Enu, double &costheta, double &E1);


    /*
     * Return the maximal cross-section for a given Enu
     * TODO: Finish
     */
    double GetMaximalCrossSection(double Enu);

};

#endif // IBD_H
