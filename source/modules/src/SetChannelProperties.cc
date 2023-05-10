
/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/

#include "SetChannelProperties.hh"

#include <iostream>
#include <functional>
#include <boost/format.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "AstroUnits.hh"

#include "VRealDetectorUnit.hh"
#include "MultiChannelData.hh"
#include "FrameData.hh"
#include "GainFunctionCubic.hh"
#include "VChannelMap.hh"
#include "SimDetectorUnit2DPixel.hh"
#include "SimDetectorUnit2DStrip.hh"
#include "SimDetectorUnitScintillator.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

SetChannelProperties::SetChannelProperties()
  : m_Filename("channel_properties.xml")
{
}

SetChannelProperties::~SetChannelProperties() = default;

ANLStatus SetChannelProperties::mod_define()
{
  register_parameter(&m_Filename, "filename");
  return AS_OK;
}

ANLStatus SetChannelProperties::mod_initialize()
{
  VCSModule::mod_initialize();
  readFile();
  return AS_OK;
}

void SetChannelProperties::readFile()
{
  using boost::property_tree::ptree;
  using boost::optional;

  ptree pt;
  try {
    read_xml(m_Filename, pt);
  }
  catch (boost::property_tree::xml_parser_error&) {
    const std::string message
      = (boost::format("cannot parse: %s") % m_Filename).str();
    BOOST_THROW_EXCEPTION(ANLException(message));
  }

  loadRootNode(pt);
}

void SetChannelProperties::loadRootNode(const boost::property_tree::ptree& rootNode)
{
  using boost::property_tree::ptree;
  
  for (const ptree::value_type& v: rootNode.get_child("channel_properties.data")) {
    if (v.first == "detector") {
      const ptree& detectorNode = v.second;
      const int detectorID = detectorNode.get<int>("<xmlattr>.id");
      for (const ptree::value_type& vv: detectorNode.get_child("")) {
        if (vv.first == "section") {
          const ptree& sectionNode = vv.second;
          const int section = sectionNode.get<int>("<xmlattr>.id");
          DetectorBasedChannelID detectorChannelID(detectorID, section);
          loadSectionNode(sectionNode, detectorChannelID);
        }
        if (vv.first == "frame") {
          const ptree& frameNode = vv.second;
          DetectorBasedChannelID detectorChannelID(detectorID);
          loadFrameNode(frameNode, detectorChannelID);
        }
      }
    }
    else if (v.first == "readout_module") {
      const ptree& moduleNode = v.second;
      const int moduleID = moduleNode.get<int>("<xmlattr>.id");
      for (const ptree::value_type& vv: moduleNode.get_child("")) {
        if (vv.first == "section") {
          const ptree& sectionNode = vv.second;
          const int section = sectionNode.get<int>("<xmlattr>.id");
          ReadoutBasedChannelID readoutChannelID(moduleID, section);
          DetectorBasedChannelID detectorChannelID
            = getDetectorManager()->convertToDetectorBasedChannelID(readoutChannelID);
          loadSectionNode(sectionNode, detectorChannelID);
        }
      }
    }
  }
}  

void SetChannelProperties::
loadSectionNode(const boost::property_tree::ptree& sectionNode,
                DetectorBasedChannelID detectorChannelID)
{