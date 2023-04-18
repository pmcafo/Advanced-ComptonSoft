add_definitions(-DANLNEXT_USE_TVECTOR -DANLNEXT_USE_HEPVECTOR)

include_directories(
  include
  ../core/include
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${G4_INC_DIR}
  ${CLHEP_INC_DIR}
  ${CFITSIO_INC_DIR}
  ${HSQUICKLOOK_INC_DIR}
  ${SIMX_INC_DIR}
  ${ADD_INC_DIR}
  ${QT_INC_DIR}
  ${BOOST_INC_DIR}
  )

link_directories(
  /usr/local/lib
  ${ANLNEXT_LIB_DIR}
  ${ROOT_LIB_DIR}
  ${G4_LIB_DIR}
  ${CLHEP_LIB_DIR}
  ${SIMX_LIB_DIR}
  )

set(CS_MODULES
  ### Basic modules
  src/ConstructDetector.cc
  src/ConstructDetectorForSimulation.cc
  src/VCSModule.cc
  src/CSHitCollection.cc
  src/ConstructChannelMap.cc
  src/SetNoiseLevels.cc
  src/SetBadChannels.cc
  src/SetChannelProperties.cc
  src/SelectHits.cc
  src/AnalyzeHits.cc
  src/MakeDetectorHits.cc
  src/MakeRawHits.cc
  src/MakeDetectorHitsWithTimingProcess.cc
  src/ApplyEPICompensation.cc
  src/EventSelection.cc
  src/EventReconstruction