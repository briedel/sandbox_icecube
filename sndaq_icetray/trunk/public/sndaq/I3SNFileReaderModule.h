#ifndef I3SNFILEREADER_H_INCLUDED
#define I3SNFILEREADER_H_INCLUDED

#include <stdio.h>
#include <fstream>
#include <queue>

#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Logging.h>

#include <boost/shared_ptr.hpp>

#include <interfaces/I3OMKey2MBID.h>

#include <sndaq/I3SNDataclasses.h>

#include <dataclasses/I3Time.h>
#include <dataclasses/geometry/I3Geometry.h>


class I3SNFileReader : public I3ConditionalModule
{
public:

    I3SNFileReader(const I3Context&);

    virtual ~I3SNFileReader(){};

    void OpenFiles();

    void ReadRebinPayloads(I3FramePtr);

    I3SNPayloadPtr ReadPayload(std::ifstream&);

    void ReadPayloads(I3FramePtr);

    void RebinPayload(I3SNPayloadPtr);

    // I3SNDatagramMapPtr RebinPayload(I3SNPayloadPtr);

    void ResetQueueMap();

    void ResetDOMStatusMap();

    // uint32_t GetYear();

    bool CheckJitter();

    bool CheckPayload();

    void Configure();

    void Geometry(I3FramePtr);

    void DetectorStatus(I3FramePtr);

    void DAQ(I3FramePtr);

    bool IsPayloadUseable(I3SNPayloadPtr payload);

    // void Process();

    void SetupGeometryBadDOMs();

    void FillPushOutFrame(I3FramePtr);

private:

    uint32_t nPayloads_;

    uint32_t fileNumber_;

    uint32_t runNumber_;

    uint32_t nFrames_;

    uint32_t frameLength_;

    std::ifstream infile_;

    std::string inDir_;

    std::string infileName_;

    I3Time runStartTime_;

    I3Time runStartYear_;

    std::vector<std::string> filesProcessed;
    // std::string geofileName_;

    I3SNDatagramMapPtr rebinDataMap_;

    I3SNDOMStatusMapPtr DOMStatMap_;

    I3SNConfigPtr config;

    boost::shared_ptr<const I3OMKey2MBID> omkey2mbid;

    std::vector<OMKey> badDOMs_;

    std::string GDFileName_;

    std::string I3GeometryName_;

    std::string badDOMsListName_;

    // I3SNDatagramMapPtr tempDatagramMap(new I3SNDatagramMap());

    std::map<OMKey, boost::shared_ptr<std::deque<uint16_t> > > tempQueueMap_;

    std::map<OMKey, I3SNPayloadPtr> previousPayload;

    I3SNDatagramMapPtr outputDatagramMap;

    I3VectorOMKeyConstPtr badDOMList;

    I3GeometryConstPtr geometry;

    std::set<OMKey> DOMsReadIn_;

};

#endif //I3SNFILEREADER_H_INCLUDED
