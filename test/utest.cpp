/**
 * Copyright © 2017 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <gtest/gtest.h>
#include <stdlib.h>
#include "targeting.hpp"

using namespace openpower::targeting;

constexpr auto masterDir = "/tmp";

TEST(TargetingTest, CreateTargets)
{
    int num = random();

    char dir[100];
    sprintf(dir, "/tmp/fsi_%d", num);

    char cmd[100];
    sprintf(cmd, "rm -rf %s", dir);
    system(cmd);
    sprintf(cmd, "mkdir %s", dir);
    system(cmd);

    //Test that we always create the first Target
    {
        Targeting targets{masterDir, dir};
        ASSERT_EQ(targets.size(), 1);

        auto t = targets.begin();
        ASSERT_EQ((*t)->getPos(), 0);

        ASSERT_EQ((*t)->getPath(), masterDir);
    }


    //Test that we can create multiple Targets
    {
        //make some fake slave entries
        sprintf(cmd, "touch %s/slave@01:00", dir);
        system(cmd);
        sprintf(cmd, "touch %s/slave@02:00", dir);
        system(cmd);
        sprintf(cmd, "touch %s/slave@03:00", dir);
        system(cmd);

        Targeting targets{masterDir, dir};

        ASSERT_EQ(targets.size(), 4);

        int i = 0;
        char path[100];

        for (auto& t : targets)
        {
            ASSERT_EQ(t->getPos(), i);

            if (0 == i)
            {
                sprintf(path, "/tmp");
            }
            else
            {
                sprintf(path, "%s/slave@0%d:00/raw", dir, i);
            }

            ASSERT_EQ(t->getPath(), path);
            i++;
        }
    }
}
