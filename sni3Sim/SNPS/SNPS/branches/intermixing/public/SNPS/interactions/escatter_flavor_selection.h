#ifndef ESCATTER_FLAVOR_SELECTION_H
#define ESCATTER_FLAVOR_SELECTION_H

#include <SNPS/interactions/escatter.h>
#include <SNPS/interactions/interaction.h>

/**
 * Classes that provide the proper coefficients give the interaction
 */


class EScatter_neutrino_e : public EScatter<EScatter_neutrino_e>
{

private:


public:

    EScatter_neutrino_e() {};

    virtual ~EScatter_neutrino_e();

    static const double epsilon1;
    static const double epsilon2;
    static const double coefficient1;
    static const double coefficient2;
    static const double coefficient3;

};

class EScatter_antineutrino_e : public EScatter<EScatter_antineutrino_e>
{

private:


public:

    EScatter_antineutrino_e() {};

    virtual ~EScatter_antineutrino_e();

    static const double epsilon1;
    static const double epsilon2;
    static const double coefficient1;
    static const double coefficient2;
    static const double coefficient3;

};

class EScatter_neutrino_x : public EScatter<EScatter_neutrino_x>
{

private:


public:

    EScatter_neutrino_x() {};

    virtual ~EScatter_neutrino_x();

    static const double epsilon1;
    static const double epsilon2;
    static const double coefficient1;
    static const double coefficient2;
    static const double coefficient3;

};

class EScatter_antineutrino_x : public EScatter<EScatter_antineutrino_x>
{

private:


public:

    EScatter_antineutrino_x() {};

    virtual ~EScatter_antineutrino_x();

    static const double epsilon1;
    static const double epsilon2;
    static const double coefficient1;
    static const double coefficient2;
    static const double coefficient3;

};

#endif // ESCATTER_FLAVOR_SELECTION_H