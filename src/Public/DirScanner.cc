#include "DirScanner.h"
#include "LogMgr.h"

#include <sys/types.h>
#include <dirent.h>

void DirScanner::operator()(const std::string &dirname)
{
    if (!_files.empty())
    {
        _files.clear();
    }
    traverse(dirname);
}

const std::vector<std::string> &DirScanner::files() const
{
    return _files;
}

void DirScanner::traverse(const std::string &dirname)
{
    DIR *dir;
    struct dirent *ent;
    if (dir = opendir(dirname.c_str()))
    {
        std::string dirpath = dirname;
        if (dirpath[dirpath.size() - 1] != '/')
        {
            dirpath += '/';
        }
        while ((ent = readdir(dir)) != nullptr)
        {
            std::string filename = ent->d_name;
            std::string concpath = dirpath + filename;

            if (ent->d_type == DT_DIR)
            {
                if (filename != "." && filename != "..")
                {
                    traverse(concpath);
                }
            }
            else
            {
                _files.push_back(concpath);
            }
        }
    }
    else
    {
        LogError("open dir: %s error!", dirname.c_str());
    }
    closedir(dir);
}
