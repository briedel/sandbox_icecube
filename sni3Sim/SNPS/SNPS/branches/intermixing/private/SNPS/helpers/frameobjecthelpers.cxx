#include <SNPS/helpers/frameobjecthelpers.h>


namespace frameobjecthelpers
{

I3Particle GetParticle(const I3MCTree &t, const I3MCPE &mcpe)
{
    I3MCTree::iterator iter;
    for (iter = t.begin(); iter != t.end(); ++iter)
    {
        log_trace("MajorID Tree %llu", iter->GetMajorID());
        log_trace("MinorID Tree %d", iter->GetMinorID());
        log_trace("MajorID MCPE %llu", mcpe.major_ID);
        log_trace("MinorID MCPE %d", mcpe.minor_ID);
        if (iter->GetMajorID() == mcpe.major_ID && iter->GetMinorID() == mcpe.minor_ID)
        {
            log_trace("MajorID %llu", mcpe.major_ID);
            log_trace("MinorID %d", mcpe.minor_ID);
            return *iter;
        }
    }
    log_error("Could not find I3Particle associated with I3MCPE");
    return I3Particle();
}

// bool FindHit(const int partminid, const uint64_t partmajid, const OMKey omkey, const I3MCPESeriesMap &MPESM)
// {
//     //Determining if an I3Photon generated an MCHit, to avoid double counting
//     I3MCPESeriesMap::const_iterator i = MPESM.find(omkey);
//     if (i == MPESM.end())
//     {
//         return true;
//     }
//     const I3MCPESeries &MPES = i->second;

//     for (I3MCPESeries::const_iterator p = MPES.begin(), endp = MPES.end(); p != endp; p++)
//     {
//         if (p->GetHitID() == hitid &&
//                 p->GetParticleMinorID() == partminid &&
//                 p->GetParticleMajorID() == partmajid)
//         {
//             return false;
//         }
//     }
//     return true;

// }

I3Photon FindPhoton(const int partminid, const uint64_t partmajid, const ModuleKey modulekey, const I3PhotonSeriesMap &PSM)
{
    //Finding the I3Photon that generated the "hit" on a 4pi-acceptance DOM
    const I3PhotonSeries &PS = PSM.find(modulekey)->second;
    for (I3PhotonSeries::const_iterator p = PS.begin(), endp = PS.end(); p != endp; p++)
    {
        if (p->GetParticleMajorID() == partmajid &&
                p->GetParticleMinorID() == partminid
           )
        {
            return *p;
        }
    }
    return I3Photon();

}

I3Particle FindPart(const int partminid, const uint64_t partmajid, const I3MCTree &tree)
{
    //  From ckopper's clsim/I3PhotontoMCHitConverter.cxx -
    //  Trying to find the particle that is associated with the MCHit
    std::map<std::pair<uint64_t, int>, const I3Particle *> mcTreeIndex;
    for (I3MCTree::iterator it = tree.begin(); it != tree.end(); ++it)
    {

        const I3Particle &particle = *it;
        mcTreeIndex.insert(std::make_pair(std::make_pair(particle.GetMajorID(),
                                          particle.GetMinorID()), &particle));


    }
    std::map<std::pair<uint64_t, int>, const I3Particle *>::const_iterator it = mcTreeIndex.find(std::make_pair(partmajid, partminid));
    const I3Particle &particle = *(it->second);
    return particle;
}


bool earlierHit(const I3MCPE &h1, const I3MCPE &h2)
{
    return (h1.time < h2.time);
}

bool earlierHitPhoton(const I3Photon &p1, const I3Photon &p2)
{
    return (p1.GetTime() < p2.GetTime());
}

void SortHits(I3MCPESeriesMapPtr newhit, I3PhotonSeriesMapPtr newphoton)
{
    SortHits(newhit);

    I3PhotonSeriesMap::iterator modulekey;
    for ( modulekey = newphoton->begin(); modulekey != newphoton->end(); modulekey++)
    {
        std::sort(modulekey->second.begin(), modulekey->second.end(), &earlierHitPhoton);
    }

}

void SortHits(I3MCPESeriesMapPtr newhit)
{
    I3MCPESeriesMap::iterator omkey;
    for ( omkey = newhit->begin(); omkey != newhit->end(); omkey++)
    {
        std::sort(omkey->second.begin(), omkey->second.end(), &earlierHit);
    }

}

float GetFirstHitTime(I3MCPESeriesMapPtr newhit)
{
    float t = std::numeric_limits<float>::max();
    I3MCPESeriesMap::const_iterator omkey;
    I3MCPESeries::const_iterator mcpe;
    for ( omkey = newhit->begin(); omkey != newhit->end(); omkey++)
    {
        for ( mcpe = omkey->second.begin(); mcpe != omkey->second.end(); mcpe++)
        {
            if ( mcpe->time < t ) t = (*mcpe).time;
        }
    }
    return t;
}



}





