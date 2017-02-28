#pragma once

#include <dirent.h>
#include <string>

/**
 * Class used for traversing contents of a directory
 */
class Directory
{
    public:

        /**
         * Constructor
         * @param[in] - the directory path
         */
        explicit Directory(const std::string& path);

        /**
         * Destructor
         */
        ~Directory();

        /**
         * Fills in name with a directory entry.  If true is returned,
         * there are more entries in the directory.
         *
         * @param[out] name - filled in with name of the file/subdir.
         * @return - true if more files/subdirs, false else
         */
        bool next(std::string& name);

    private:

        /**
         * Current directory entry while traversing using next()
         */
        dirent* entry;

        /**
         * DIR structure returned from opendir()
         */
        DIR* dirp;
};
