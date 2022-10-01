
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

#ifndef COMPTONSOFT_ReadoutModule_H
#define COMPTONSOFT_ReadoutModule_H 1

#include <vector>
#include "ChannelID.hh"

namespace comptonsoft {

/**
 * A class of a readout module.
 * @author Hirokazu Odaka
 * @date 2014-11-11
 * @date 2016-08-19 | change class name
 */
class ReadoutModule
{
public:
  ReadoutModule();
  ~ReadoutModule();

  ReadoutModule(const ReadoutModule&) = default;
  ReadoutModule(ReadoutModule&&) = default;
  ReadoutModule& operator=(const ReadoutModule&) = default;
  ReadoutModule& operator=(ReadoutModule&&) = default;

  void setID(int v) { ID_ = v; }
  int ID() const { return ID_; }

  void push(int detectorID, int detectorSection);
  void clear();

  const std::vector<DetectorBasedChannelID>& Sections() const
  { return sections_; }
  int NumberOfSections() const { return sections_.size(); }
  DetectorBasedChannelID getSection(int index) const;

private:
  int ID_ = 0;
  std::vector<DetectorBasedChannelID> sections_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_DetectorReadoutModule_H */