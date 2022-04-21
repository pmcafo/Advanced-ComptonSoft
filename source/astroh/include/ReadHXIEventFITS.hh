
/*************************************************************************
 *                                                                       *
 * Copyright (c) 2013 Hirokazu Odaka                                     *
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

#ifndef COMPTONSOFT_ReadHXIEventFITS_H
#define COMPTONSOFT_ReadHXIEventFITS_H 1

#include "VCSModule.hh"
#include "InitialInformation.hh"
#include <memory>
#include "HXIEventFITS.hh"

namespace comptonsoft {

/**
 * ANL module to read HXI event ROOT files.
 *
 * @author Hirokazu Odaka
 * @date 2016-11-10
 * @date 2017-10-11 | access to event time */
class ReadHXIEventFITS : public VCSModule, public anlgeant4::InitialInformation
{
  DEFINE_ANL_MODULE(ReadHXIEventFITS, 1.0);
public:
  ReadHXIEventFITS();
  ~ReadHXIEventFITS();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  double EventTime() const { return m_EventTime; }

private:
  std::string m_Filename;
  bool m_VetoEnable;

  std::unique_ptr<astroh::hxi::EventFITSReader> m_EventReader;
  Long64_t m_NumEvents;
  Long64_t m_Index;

  double m_EventTime;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadHXIEventFITS_H */