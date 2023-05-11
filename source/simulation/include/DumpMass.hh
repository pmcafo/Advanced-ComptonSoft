
/*************************************************************************
 *                                                                       *
 * Copyright (c) 2012 Hirokazu Odaka                                     *
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

#ifndef COMPTONSOFT_DumpMass_H
#define COMPTONSOFT_DumpMass_H 1

#include <anlnext/BasicModule.hh>

namespace comptonsoft {


/**
 * Dump mass of all logical volumes.
 *
 * @author Hirokazu Odaka
 * @date 2013-06-17
 */
class DumpMass : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(DumpMass, 1.0);
public:
  DumpMass();
  ~DumpMass();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_begin_run() override;

private:
  std::string m_FileName;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_DumpMass_H */