
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

// ConstructDetectorForSimulation.hh
// 2008-08-27 Hirokazu Odaka

#ifndef COMPTONSOFT_ConstructDetectorForSimulation_H
#define COMPTONSOFT_ConstructDetectorForSimulation_H 1

#include "ConstructDetector.hh"

namespace comptonsoft {

class ConstructDetectorForSimulation : public ConstructDetector
{
  DEFINE_ANL_MODULE(ConstructDetectorForSimulation, 3.3);
public:
  ConstructDetectorForSimulation();
  ~ConstructDetectorForSimulation();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ConstructDetectorForSimulation_H */