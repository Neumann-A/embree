// ======================================================================== //
// Copyright 2009-2016 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "../common/tutorial/tutorial.h"

namespace embree
{
  extern "C" { int g_instancing_mode = 0; }
  extern "C" { bool g_anim = false; }

  extern "C" void dumpBuildAndRenderTimes();

  struct Tutorial : public SceneLoadingTutorialApplication
  {
    Tutorial()
      : SceneLoadingTutorialApplication("viewer_anim",FEATURE_RTCORE) {}
    
    void postParseCommandLine() 
    {
      /* enable/disable animation sequence */
      g_anim = anim;

      /* load default scene if none specified */
      if (!g_anim && sceneFilename.ext() == "") {
        FileName file = FileName::executableFolder() + FileName("models/cornell_box.ecs");
        parseCommandLine(new ParseStream(new LineCommentFilter(file, "#")), file.path());
      }
      
      g_instancing_mode = instancing_mode;
    }
  };

}

int main(int argc, char** argv) {
  int t = embree::Tutorial().main(argc,argv);
  return t;
}