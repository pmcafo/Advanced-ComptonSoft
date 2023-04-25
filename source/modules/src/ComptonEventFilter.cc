
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

#include "ComptonEventFilter.hh"
#include <algorithm>
#include "AstroUnits.hh"
#include "BasicComptonEvent.hh"
#include "EventReconstruction.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

ComptonEventFilter::ComptonEventFilter()
  : m_EventReconstruction(nullptr)
{
}

ANLStatus ComptonEventFilter::mod_define()
{
  return AS_OK;
}

ANLStatus ComptonEventFilter::mod_initialize()
{
  VCSModule::mod_initialize();
  
  define_evs("ComptonEventFilter:Selected");
  define_evs("ComptonEventFilter:Exception");
  
  get_module_NC("EventReconstruction", &m_EventReconstruction);

  auto& hitPatterns = getDetectorManager()->getHitPatterns();
  for (auto& eventSelection: m_ConditionsVector) {
    const std::vector<std::string>& hitpatNames = eventSelection.get_hit_pattern_names();
    for (const std::string& name: hitpatNames) {
      auto it = std::find_if(std::begin(hitPatterns), std::end(hitPatterns),
                             [&](const HitPattern hitpat) {
                               return (hitpat.Name()==name);
                             });
      if (it != std::end(hitPatterns)) {
        std::string evsKey = "HitPattern:";
        evsKey += it->ShortName();
        eventSelection.add_hit_pattern_key(evsKey);
      }
      else {
        std::cout << "Hit pattern \"" << name << "\" is not found." << std::endl;
      }
    }
  }
  
  return AS_OK;
}

ANLStatus ComptonEventFilter::mod_analyze()
{
  if (m_EventReconstruction->NumberOfReconstructedEvents()!=1) {
    set_evs("ComptonEventFilter:Exception");
    return AS_SKIP;
  }

  const_BasicComptonEvent_sptr comptonEvent = m_EventReconstruction->getReconstructedEvents()[0];

  if (m_ConditionsVector.empty()) {
    set_evs("ComptonEventFilter:Selected");
    return AS_OK;
  }
  
  bool selected = false;
  for (auto& eventSelection: m_ConditionsVector) {
    bool goodHitPattern = false;
    const std::vector<std::string>& hpKeys = eventSelection.get_hit_pattern_keys();
    if (hpKeys.empty()) {
      goodHitPattern = true;
    }
    for (const std::string& key: hpKeys) {
      if (evs(key)) {
        goodHitPattern = true;
        break;
      }
    }
    if (goodHitPattern == false) { continue; }

    bool goodEvs = true;
    const std::vector<std::string>& evsKeys = eventSelection.get_evs_keys();
    for (const std::string& key: evsKeys) {
      if (!evs(key)) {
        goodEvs = false;
        break;
      }
    }
    if (goodEvs == false) { continue; }

    bool goodCondition = true;
    const std::vector<std::function<bool (const BasicComptonEvent&)>>& conditions = eventSelection.get_conditions();
    for (auto& condition: conditions) {
      if ( !condition(*comptonEvent) ) {
        goodCondition = false;
        break;
      }
    }

    if (goodCondition) {
      selected = true;
      break;
    }
  }
  
  if (!selected) {
    return AS_SKIP;
  }

  set_evs("ComptonEventFilter:Selected");
  return AS_OK;
}

void ComptonEventFilter::define_condition()
{
  m_ConditionsVector.resize(m_ConditionsVector.size()+1);
}

void ComptonEventFilter::add_evs_key(const std::string& key)
{
  if (m_ConditionsVector.size()==0) {
    define_condition();
  }

  m_ConditionsVector.back().add_evs_key(key);
}

void ComptonEventFilter::add_hit_pattern_key(const std::string& key)
{
  if (m_ConditionsVector.size()==0) {
    define_condition();
  }

  m_ConditionsVector.back().add_hit_pattern_key(key);
}

void ComptonEventFilter::add_hit_pattern(const std::string& name)
{
  if (m_ConditionsVector.size()==0) {
    define_condition();
  }

  m_ConditionsVector.back().add_hit_pattern_name(name);
}

void ComptonEventFilter::add_condition(const std::string& type,
                                       double min_value,
                                       double max_value)
{
  if (m_ConditionsVector.size()==0) {
    define_condition();
  }

  std::function<bool (const BasicComptonEvent&)> condition;

  namespace arg = std::placeholders;
  if (type == "event ID") {
    condition = std::bind(filter_compton<int64_t>,
                          &BasicComptonEvent::EventID,
                          arg::_1, min_value, max_value);
  }
  else if (type == "number of hits") {
    condition = std::bind(filter_compton<int>,
                          &BasicComptonEvent::NumberOfHits,
                          arg::_1, min_value, max_value);
  }
  else if (type == "E1") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit1Energy,
                          arg::_1, min_value*unit::keV, max_value*unit::keV);
  }
  else if (type == "E2") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit2Energy,
                          arg::_1, min_value*unit::keV, max_value*unit::keV);
  }
  else if (type == "E1+E2") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::IncidentEnergy,
                          arg::_1, min_value*unit::keV, max_value*unit::keV);
  }
  else if (type == "energy") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::IncidentEnergy,
                          arg::_1, min_value*unit::keV, max_value*unit::keV);
  }
  else if (type == "theta K") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::ThetaE,
                          arg::_1, min_value, max_value);
  }
  else if (type == "theta G") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::ThetaG,
                          arg::_1, min_value, max_value);
  }
  else if (type == "delta theta") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::DeltaTheta,
                          arg::_1, min_value, max_value);
  }
  else if (type == "time") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit1Time,
                          arg::_1, min_value*unit::s, max_value*unit::s);
  }
  else if (type == "hit 1 position x") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit1PositionX,
                          arg::_1, min_value*unit::cm, max_value*unit::cm);
  }
  else if (type == "hit 1 position y") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit1PositionY,
                          arg::_1, min_value*unit::cm, max_value*unit::cm);
  }
  else if (type == "hit 1 position z") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit1PositionZ,
                          arg::_1, min_value*unit::cm, max_value*unit::cm);
  }
  else if (type == "hit 2 position x") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit2PositionX,
                          arg::_1, min_value*unit::cm, max_value*unit::cm);
  }
  else if (type == "hit 2 position y") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit2PositionY,
                          arg::_1, min_value*unit::cm, max_value*unit::cm);
  }
  else if (type == "hit 2 position z") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Hit2PositionZ,
                          arg::_1, min_value*unit::cm, max_value*unit::cm);
  }
  else if (type == "first interaction distance") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::DistanceBetweenTheHits,
                          arg::_1, min_value*unit::cm, max_value*unit::cm);
  }
  else if (type == "distance between the hits") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::DistanceBetweenTheHits,
                          arg::_1, min_value*unit::cm, max_value*unit::cm);
  }
  else if (type == "max delta theta") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::MaxDeltaTheta,
                          arg::_1, min_value, max_value);
  }
  else if (type == "grade") {
    condition = std::bind(filter_compton<int32_t>,
                          &BasicComptonEvent::Grade,
                          arg::_1, min_value, max_value);
  }
  else if (type == "likelihood") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::Likelihood,
                          arg::_1, min_value, max_value);
  }
  else if (type == "reconstruction fraction") {
    condition = std::bind(filter_compton<double>,
                          &BasicComptonEvent::ReconstructionFraction,
                          arg::_1, min_value, max_value);
  }
  else {
    std::cout << "invalid condition type: " << type << std::endl;
    return;
  }
  
  m_ConditionsVector.back().add_condition(condition);
}

} /* namespace comptonsoft */