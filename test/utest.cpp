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
#include <experimental/filesystem>
#include <fstream>
#include <stdlib.h>
#include "registration.hpp"
#include "targeting.hpp"

using namespace openpower::util;
using namespace openpower::targeting;
namespace fs = std::experimental::filesystem;

constexpr auto masterDir = "/tmp";

class TargetingTest : public ::testing::Test
{
    protected:

        virtual void SetUp()
        {
#if 0
            char dir[50];
            strcpy(dir, masterDir);
            strcat(dir, "/targetingXXXXXX");

            auto path = mkdtemp(dir);
            assert(path != nullptr);

            _slaveBaseDir = path;

            _slaveDir = _slaveBaseDir / "hub@00";
            fs::create_directory(_slaveDir);
#endif
        }

        virtual void TearDown()
        {
#if 0
            fs::remove_all(_slaveDir);
            fs::remove_all(_slaveBaseDir);
#endif
        }

        fs::path _slaveBaseDir;
        fs::path _slaveDir;
};


TEST_F(TargetingTest, CreateTargets)
{

    //Test that we always create the first Target
    {
#if 0
        Targeting targets{masterDir, _slaveDir};
        ASSERT_EQ(targets.size(), 1);

        auto t = targets.begin();
        ASSERT_EQ((*t)->getPos(), 0);

        ASSERT_EQ((*t)->getCFAMPath(), masterDir);
#endif
    }


    //Test that we can create multiple Targets
    {
#if 0
        //make some fake slave entries
        std::ofstream(_slaveDir / "slave@01:00");
        std::ofstream(_slaveDir / "slave@02:00");
        std::ofstream(_slaveDir / "slave@03:00");
        std::ofstream(_slaveDir / "slave@04:00");

        Targeting targets{masterDir, _slaveDir};

        ASSERT_EQ(targets.size(), 5);

        int i = 0;

        for (const auto& t : targets)
        {
            fs::path path;

            ASSERT_EQ(t->getPos(), i);

            if (0 == i)
            {
                path = masterDir;
            }
            else
            {
                std::ostringstream subdir;
                subdir << "slave@0" << i << ":00/raw";

                path = _slaveDir;
                path /= subdir.str();
            }

            
            std::cout << "t path = " << t->getCFAMPath() << std::endl;
            std::cout << "path = " << path << std::endl;

            ASSERT_EQ(t->getCFAMPath(), path);
            i++;
        }
#endif
    }
}


void func1()
{
    std::cout << "Hello\n";
}

void func2()
{
    std::cout << "World\n";
}

REGISTER_PROCEDURE("hello", func1);
REGISTER_PROCEDURE("world", func2);


TEST(RegistrationTest, TestReg)
{
#if 0
    int count = 0;
    for (const auto& p : Registration::getProcedures())
    {
        std::cout << p.first << std::endl;
        p.second();
        count++;
    }

    ASSERT_EQ(count, 2);
#endif
}
