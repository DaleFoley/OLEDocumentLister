#include <iostream>
#include <fstream>
#include <cstring>
#include <cerrno>
#include <gsf/gsf.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#include <fileapi.h>
#endif

bool is_file_exist(const std::string& file)
{
    struct stat buffer;
    return (stat (file.c_str(), &buffer) == 0);
}

int main(int argc, char *argv[])
{
    if(argv[1] == nullptr)
    {
        std::cout << "Require a valid source file to operate on" << std::endl;
        return 0;
    }

    if(argv[2] == nullptr)
    {
        std::cout << "Require a valid directory to write results to" << std::endl;
        return 0;
    }

    std::string sourceFile = argv[1];
    std::string destinationDir = argv[2];

    if(!is_file_exist(sourceFile))
    {
        std::cout << "File [" + sourceFile + "] does not exist";
        return 0;
    }

    GError *ge = nullptr;

    GsfInput * gsfInput = GSF_INPUT(gsf_input_stdio_new(sourceFile.c_str(), &ge));
    GsfInfile * oleInfile = GSF_INFILE(gsf_infile_msole_new(gsfInput, &ge));
    g_object_unref(G_OBJECT(gsfInput));

    if(ge != nullptr)
    {
        std::cout << "Error encountered [" + std::string(ge->message) + "]" << std::endl;
        return -1;
    }

    int childCount = gsf_infile_num_children(oleInfile);

    if(childCount == -1)
    {
        std::cout << "file [" + sourceFile + "] does not contain any children";
        return 0;
    }

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        _mkdir(destinationDir.c_str());
    #else
        if(!is_file_exist(destinationDir))
        {
            if(mkdir(destinationDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == -1)
            {
                std::cout << "Failed to create directory [" + destinationDir + "] errno is [" + std::strerror(errno) + "]";
                return -1;
            }
        }
    #endif

    for(int childIndex = 0; childIndex != childCount; childIndex++)
    {
        GsfInput * child = GSF_INPUT(gsf_infile_child_by_index(oleInfile, childIndex));

        std::string childName = child->name;
        std::string fileToWrite = destinationDir + "/" + childName + ".txt";

        std::ofstream fileHandle(fileToWrite, std::ios::trunc
                                                | std::ios::binary
                                                | std::ios::out);

        unsigned char buffer;
        while(!gsf_input_eof(child))
        {
            gsf_input_read(child, sizeof(buffer), static_cast<guint8*>(&buffer));
            fileHandle << buffer;
        }

        std::cout << "Wrote bytes [" + std::to_string(child->size) + "] to file [" + fileToWrite +"]" << std::endl;

        fileHandle.close();
    }

    return 0;
}