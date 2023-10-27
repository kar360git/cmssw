// -*- C++ -*-
//
// Package:    L1Trigger/DemonstratorTools
// Class:      GTTFileReader
//
/**\class GTTFileReader GTTFileReader.cc L1Trigger/DemonstratorTools/plugins/GTTFileReader.cc

 Description: Example EDProducer class, illustrating how BoardDataReader can be used to
   read I/O buffer files (that have been created in hardware/firmware tests), decode
   the contained data, and store this in EDM collections.

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Thomas Williams <thomas.williams@stfc.ac.uk>
//         Created:  Fri, 19 Feb 2021 01:10:55 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/L1Trigger/interface/Vertex.h"
#include "L1Trigger/DemonstratorTools/interface/BoardDataReader.h"
#include "L1Trigger/DemonstratorTools/interface/codecs/vertices.h"
#include "L1Trigger/DemonstratorTools/interface/utilities.h"

//
// class declaration
//

class GTTFileReader : public edm::stream::EDProducer<> {
public:
  explicit GTTFileReader(const edm::ParameterSet&);

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  // ----------constants, enums and typedefs ---------
  // NOTE: At least some of the info from these constants will eventually come from config files
  static constexpr size_t kFramesPerTMUXPeriod = 9;
  static constexpr size_t kGapLength = 44;
  static constexpr size_t kGapLengthInput = 6;
  static constexpr size_t kVertexTMUX = 6;
  static constexpr size_t kGTTBoardTMUX = 6;
  static constexpr size_t kTrackTMUX = 18;
  static constexpr size_t kVertexChanIndex = 0;
  static constexpr size_t kEmptyFrames = 0; // 10 does not match current file writing configuration

  const l1t::demo::BoardDataReader::ChannelMap_t kChannelSpecs = {
      /* logical channel within time slice -> {{link TMUX, inter-packet gap}, vector of channel indices} */
      {{"vertices", 0}, {{kVertexTMUX, kGapLength}, {kVertexChanIndex}}}};

  const l1t::demo::BoardDataReader::ChannelMap_t kChannelSpecsInput = {
      /* logical channel within time slice -> {{link TMUX, inter-packet gap}, vector of channel indices} */
      {{"tracks", 0}, {{kTrackTMUX, kGapLengthInput}, {0, 18, 36}}},
      {{"tracks", 1}, {{kTrackTMUX, kGapLengthInput}, {1, 19, 37}}},
      {{"tracks", 2}, {{kTrackTMUX, kGapLengthInput}, {2, 20, 38}}},
      {{"tracks", 3}, {{kTrackTMUX, kGapLengthInput}, {3, 21, 39}}},
      {{"tracks", 4}, {{kTrackTMUX, kGapLengthInput}, {4, 22, 40}}},
      {{"tracks", 5}, {{kTrackTMUX, kGapLengthInput}, {5, 23, 41}}},
      {{"tracks", 6}, {{kTrackTMUX, kGapLengthInput}, {6, 24, 42}}},
      {{"tracks", 7}, {{kTrackTMUX, kGapLengthInput}, {7, 25, 43}}},
      {{"tracks", 8}, {{kTrackTMUX, kGapLengthInput}, {8, 26, 44}}},
      {{"tracks", 9}, {{kTrackTMUX, kGapLengthInput}, {9, 27, 45}}},
      {{"tracks", 10}, {{kTrackTMUX, kGapLengthInput}, {10, 28, 46}}},
      {{"tracks", 11}, {{kTrackTMUX, kGapLengthInput}, {11, 29, 47}}},
      {{"tracks", 12}, {{kTrackTMUX, kGapLengthInput}, {12, 30, 48}}},
      {{"tracks", 13}, {{kTrackTMUX, kGapLengthInput}, {13, 31, 49}}},
      {{"tracks", 14}, {{kTrackTMUX, kGapLengthInput}, {14, 32, 50}}},
      {{"tracks", 15}, {{kTrackTMUX, kGapLengthInput}, {15, 33, 51}}},
      {{"tracks", 16}, {{kTrackTMUX, kGapLengthInput}, {16, 34, 52}}},
      {{"tracks", 17}, {{kTrackTMUX, kGapLengthInput}, {17, 35, 53}}}
  };
  // const std::map<std::string, l1t::demo::ChannelSpec> kChannelSpecsInput = {
  //     /* interface name -> {link TMUX, inter-packet gap} */
  //     {"tracks", {kTrackTMUX, kGapLengthInput}}};
  

  // ----------member functions ----------------------
  void produce(edm::Event&, const edm::EventSetup&) override;

  // ----------member data ---------------------------
  l1t::demo::BoardDataReader fileReader_;
  std::string l1VertexCollectionName_;
  l1t::demo::BoardDataReader fileReaderInputTracks_;
};

//
// class implementation
//

GTTFileReader::GTTFileReader(const edm::ParameterSet& iConfig)
    : fileReader_(l1t::demo::parseFileFormat(iConfig.getUntrackedParameter<std::string>("format")),
                  iConfig.getParameter<std::vector<std::string>>("files"),
                  kFramesPerTMUXPeriod,
                  kVertexTMUX,
                  kEmptyFrames,
                  kChannelSpecs),
      l1VertexCollectionName_(iConfig.getParameter<std::string>("l1VertexCollectionName")),
      fileReaderInputTracks_(l1t::demo::parseFileFormat(iConfig.getUntrackedParameter<std::string>("format")),
      			     iConfig.getParameter<std::vector<std::string>>("filesInputTracks"),
      			     kFramesPerTMUXPeriod,
      			     kGTTBoardTMUX,
      			     kEmptyFrames,
      			     kChannelSpecsInput) {
  produces<l1t::VertexWordCollection>(l1VertexCollectionName_);

}

// ------------ method called to produce the data  ------------
void GTTFileReader::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  using namespace l1t::demo::codecs;

  l1t::demo::EventData eventData(fileReader_.getNextEvent());

  l1t::VertexWordCollection vertices(decodeVertices(eventData.at({"vertices", 0})));

  edm::LogInfo("GTTFileReader") << vertices.size() << " vertices found";

  iEvent.put(std::make_unique<l1t::VertexWordCollection>(vertices), "L1VerticesFirmware");
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void GTTFileReader::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  // GTTFileReader
  edm::ParameterSetDescription desc;
  desc.add<std::vector<std::string>>("files",
                                     {
                                         "L1GTTOutputToCorrelator_0.txt",
                                     });
  desc.add<std::string>("l1VertexCollectionName", "L1VerticesFirmware");
  desc.add<std::vector<std::string>>("filesInputTracks",
                                     {
                                         "L1GTTInputFile_0.txt",
                                     });
  desc.addUntracked<std::string>("format", "APx");
  descriptions.add("GTTFileReader", desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(GTTFileReader);
