#ifndef FRAMEOBJECTHELPERS_H_INCLUDED
#define FRAMEOBJECTHELPERS_H_INCLUDED

#define GRANULAR_GEOMETRY_SUPPORT

#include <SNPS/helpers/frameobjects.h>

#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/physics/I3MCHit.h>

#include <clsim/I3Photon.h>

#include <simclasses/I3MCPE.h>

namespace frameobjecthelpers
{


/*
 * Find particle in MCTree from MCPE
 *
 * @param Reference to an MCTree
 * @param I3MCPE
 * @return I3Particle
 */
I3Particle GetParticle(const I3MCTree &, const I3MCPE &);

// TODO: QUESTION OF HITID
// bool FindHit(const int, const uint64_t, const OMKey, const I3MCPESeriesMap &);

// I3Photon FindPhoton(const int, const uint64_t, const ModuleKey, const I3PhotonSeriesMap &);

/*
 * Find particle in MCTree by minor and major ID
 *
 * @param Minor ID
 * @param Major ID
 * @param Reference to an MCTree
 * @return I3Particle
 */
I3Particle FindPart(const int, const uint64_t, const I3MCTree &);


/**
 * Sort hits in MCPESeriesMap and PhotonSeriesMap
 */
void SortHits(I3MCPESeriesMapPtr, I3PhotonSeriesMapPtr );

/**
 * Sort hits in MCPESeriesMap
 */
void SortHits(I3MCPESeriesMapPtr);

/**
 * Get time of first hit in MCPESeriesMap
 */
float GetFirstHitTime(I3MCPESeriesMapPtr);

};

#endif //FRAMEOBJECTHELPERS_H_INCLUDED
