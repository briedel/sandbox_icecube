// insert standard header

#ifndef INTERACTION_H
#define INTERACTION_H

/*
 * Interaction base class that provides a template of the
 * public classes needed for every interaction
 */

// template<typename interaction>
class Interaction
{

private:



public:

    virtual ~Interaction() {};

    /*
     * Function to return the energy of the first product,
     * given the neutrino energy and the angle of the product
     * w.r.t. to the incoming neutrino in the target rest frame
     * First product is assumed to be the charged lepton
     * throughout
     */
    virtual double GetE1(double Enu, double costheta) = 0;

    /*
     * Function to return the energy of the second product
     * given the neutrino energy, and energy of the first product.
     * Conservation of energy is assumed.
     * Second product is assumed to be the second particle generated
     * in the interaction, for example neutron in case of inverse
     * beta decay, or the product of a subsequent nuclear decay,
     * for example proton from the proton emission of fluorine-16
     */
    virtual double GetE2(double Enu, double costheta) = 0;

    /*
     * Function to return the kinetic energy of the first product
     * as expected by CLSim
     */
    virtual double GetKE1(double E1) = 0;


    /*
     * Function to return the kinetic energy of the second product
     * as expected by CLSim
     */
    virtual double GetKE2(double E2) = 0;

    /*
     * Function to return the angle of the second product given
     * energies of the particles
     */
    virtual double GetPsi(double E1, double E2, double costheta) = 0;

    /*
     * Function to return the total cross-section of the given
     * interaction for a given neutrino energy
     */
    virtual double GetTotalCrossSection(double Enu) = 0;

    /*
     * Function to return the differential cross-section
     * \frac{d \sigma}{dE} for a given neutrino energy,
     * the angle of the outgoing lepton, and the lepton's
     * energy
     */
    virtual double GetDifferentialCrossSection(double Enu, double costheta) = 0;


    /*
     * Function to return the number of targets for a given
     * interaction
     */
    virtual double GetNumberOfTargets() = 0;

    /*
     * Get the maximum possible cross-section for a given neutrino energy.
     * Needed to check whether or not
     */
    virtual double GetMaximalCrossSection(double Enu) = 0;

    /*
     * Function to return the differential cross-section
     * for a given event
     */
    virtual double GetRandomEvent(double Enu, double &costheta, double &E1) = 0;

};

#endif // INTERACTION_H
