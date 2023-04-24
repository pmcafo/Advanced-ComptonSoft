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


#ifndef COMPTONSOFT_PushXrayEventToQuickLookDB_H
#define COMPTONSOFT_PushXrayEventToQuickLookDB_H 1

#include <anlnext/BasicModule.hh>
#include "VCSModule.hh"
#include "XrayEventCollection.hh"
#include "LoadMetaDataFile.hh"

namespace hsquicklook {
class MongoDBClient;
}

namespace comptonsoft {

class PushXrayEventToQuickLookDB : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(PushXrayEventToQuickLookDB, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  PushXrayEventToQuickLookDB();
  
protected:
  PushXrayEventToQuickLookDB(const PushXrayEventToQuickLookDB&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

protected:
  void pushXrayEventsToDB();

private:
  std::string event_collection_module_name_;
  std::string collection_;
  int period_ = 1;
  int phase_ = 0;
  std::string analysis_id_ = "";

  const XrayEventCollection* event_collection_module_ = nullptr;
  const LoadMetaDataFile* metadata_file_module_ = nullptr;
  hsquicklook::MongoDBClient* mongodb_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_PushXrayEventToQuickLookDB_H */
