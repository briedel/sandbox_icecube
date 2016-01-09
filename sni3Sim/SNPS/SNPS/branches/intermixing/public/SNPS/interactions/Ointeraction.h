#ifndef OINTERACTION_H
#define OINTERACTION_H

#include <SNPS/interactions/interaction.h>


/*
 * Class to calculate the cross-sections and energies
 * for neutrino-Oxygen interactions. Taken from
 * , arXiv:
 * http://arxiv.org/abs/
 */

class OInteraction
{

private:



public:

    double GetE1(double Enu, double costheta);

    double GetE2(double Enu, double E1);

    double GetCosPsi(double Enu, double E1, double E2);

    double GetTotalCrossSection(double Enu);

    double GetDifferentialCrossSection(double Enu, double costheta, double &Ee);

    double GetRandomEvent(double Enu, double &cospsi, double &Ee);

};



#endif // OINTERACTION_H
