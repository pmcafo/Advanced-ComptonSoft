/*************************************************************************
 *                                                                       *
 * Copyright (c) 2014 Hirokazu Odaka                                     *
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

#include "ReadHXIEventFITS.hh"
#include "AstroUnits.hh"
#include "TChain.h"
#include "ChannelID.hh"
#include "HXIEvent.hh"
#include "HXIEventFITS.hh"
#include "ReadoutModule.hh"
#include "MultiChannelData.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace
{

comptonsoft::ReadoutBasedChannelID getReadoutID(uint8_t ASIC_ID)
{
  const uint8_t ASICNo = ASIC_ID >> 4;
  const uint8_t TrayNo = ASIC_ID & 0xf;
  return comptonsoft::ReadoutBasedChannelID(TrayNo, ASICNo);
}

bool is_pseudo_triggered(astroh::hxi::EventFlags f)
{
  return (f.getTriggerPattern() & (0x1u<<6));
}

bool is_pseudo_effective(astroh::hxi::EventFlags f)
{
  if (f.getFastBGO()!=0) { return false; }
  if (f.getHitPattern()!=0) { return false; }
  return is_pseudo_triggered(f);
}

} /* anonymous namespace */


namespace comptonsoft
{

ReadHXIEventFITS::ReadHXIEventFITS()
  : anlgeant4::InitialInformation(false),
    m_VetoEnable(true), m_NumEvents(0), m_Index(0),
    m_EventTime(0.0)
{
  add_alias("InitialInformation");
}

ReadHXIEventFITS::~ReadHXIEventFITS() = default;

ANLStatus ReadHXIEventFITS::mod_define()
{
  register_parameter(&m_Filename, "filename");

  return AS_OK;
}

ANLStatus ReadHXIEventFITS::mod_initialize()
{
  VCSModule::mod_initialize();

  m_EventReader.reset(new astroh::hxi::EventFITSReader);
  m_EventReader->open(m_Filename);
  m_NumEvents = m_EventReader->NumberOfRows();
  std::cout << "Total events: " << m_NumEvents << std::endl;

  define_evs("ReadHXIEventFITS:PseudoTrigger");
  define_evs("ReadHXIEventFITS:PseudoEffective");
  define_evs("ReadHXIEventFITS:ShieldTrigger");
  
  return AS_OK;
}

ANLStatus ReadHXIEventFITS::mod_analyze()
{
  if (m_Index==m_NumEvents) {
    return AS_QUIT;
  }

  astroh::hxi::Event event;
  const long int row = m_Index + 1;
  m_EventReader->restoreEvent(row, event);
  setEventID(event.getOccurrenceID());

  const double eventTime = event.getTime() * unit::second;
  m_EventTime = eventTime;

  const astroh::hxi::EventFlags eventFlags = event.getFlags();

  if (is_pseudo_triggered(eventFlags)) {
    set_evs("ReadHXIEventFITS:PseudoTrigger");
  }
  if (is_pseudo_effective(eventFlags)) {
    set_evs("ReadHXIEventFITS:PseudoEffective");
  }

  if (eventFlags.getHitPattern() || ev