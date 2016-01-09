#include <SNPS/constants/SNPSConstants.h>
#include <SNPS/interactions/escatter.h>
#include <SNPS/interactions/escatter_flavor_selection.h>
#include <SNPS/interactions/interaction.h>

using namespace SNPSConstants;

EScatter_neutrino_e::~EScatter_neutrino_e() {};
EScatter_antineutrino_e::~EScatter_antineutrino_e() {};
EScatter_neutrino_x::~EScatter_neutrino_x() {};
EScatter_antineutrino_x::~EScatter_antineutrino_x() {};

const double EScatter_neutrino_e::epsilon1 = -(1. / 2.) - sinsqthetaW;
const double EScatter_neutrino_e::epsilon2 = -sinsqthetaW;
const double EScatter_neutrino_e::coefficient1 = 1.;
const double EScatter_neutrino_e::coefficient2 = 4. * sinsqthetaW;
const double EScatter_neutrino_e::coefficient3 = (16. / 3.) * sinsqthetaW_sq;

const double EScatter_antineutrino_e::epsilon1 = -sinsqthetaW;
const double EScatter_antineutrino_e::epsilon2 = -(1. / 2.) - sinsqthetaW;
const double EScatter_antineutrino_e::coefficient1 = 1. / 3.;
const double EScatter_antineutrino_e::coefficient2 = (4. / 3.) * sinsqthetaW;
const double EScatter_antineutrino_e::coefficient3 = (16. / 3.) * sinsqthetaW_sq;

const double EScatter_neutrino_x::epsilon1 = (1. / 2.) - sinsqthetaW;
const double EScatter_neutrino_x::epsilon2 = -sinsqthetaW;
const double EScatter_neutrino_x::coefficient1 = 1.;
const double EScatter_neutrino_x::coefficient2 = -4. * sinsqthetaW;
const double EScatter_neutrino_x::coefficient3 = (16. / 3.) * sinsqthetaW_sq;

const double EScatter_antineutrino_x::epsilon1 = -sinsqthetaW;
const double EScatter_antineutrino_x::epsilon2 = (1. / 2.) - sinsqthetaW;
const double EScatter_antineutrino_x::coefficient1 = 1. / 3.;
const double EScatter_antineutrino_x::coefficient2 = -(4. / 3.) * sinsqthetaW;
const double EScatter_antineutrino_x::coefficient3 = (16. / 3.) * sinsqthetaW_sq;