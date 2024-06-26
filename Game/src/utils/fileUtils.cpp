//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

//game
#include "fileUtils.hpp"

using std::cout;
using std::exception;
using std::runtime_error;
using std::filesystem::exists;
using std::filesystem::copy_file;
using std::filesystem::copy;
using std::filesystem::remove;
using std::filesystem::remove_all;
using std::filesystem::copy_options;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::filesystem::create_directory;
using std::filesystem::create_directories;
using std::filesystem::rename;
using std::filesystem::recursive_directory_iterator;

namespace Utils
{
	string File::GetOutputFromBatFile(const char* file)
	{
        char buffer[128];
        string result = "";
        string command = "\"" + string(file) + "\"";
        FILE* pipe = _popen(command.c_str(), "r");

        if (!pipe) throw runtime_error("popen() failed!");

        try
        {
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            {
                result += buffer;
            }
        }
        catch (...)
        {
            _pclose(pipe);
            throw;
        }

        _pclose(pipe);

        return result;
	}

    void File::MoveOrRenameFileOrFolder(const path& sourcePath, const path& destinationPath, const bool isRenaming)
    {
        string output;

        if (!exists(sourcePath))
        {
            cout << "Source path " << sourcePath.string() << " does not exist!\n\n";
            return;
        }
        if (exists(destinationPath))
        {
            string outputType = isRenaming ? "Cannot rename source " : "Cannot move source ";
            cout << outputType << sourcePath.string() << " to destination " 
                << destinationPath.string() << " because destination already exists!\n\n";
            return;
        }

        try
        {
            rename(sourcePath, destinationPath);

            string outputType = isRenaming ? "Renamed " : "Moved ";
            cout << outputType << sourcePath.string() << " to " << destinationPath.string() << ".\n\n";
        }
        catch (const exception& e)
        {
            cout << string(e.what()) << ".\n\n";
        }
    }

    void File::CopyFileOrFolder(const path& sourcePath, const path& destinationPath)
    {
        if (!exists(sourcePath))
        {
            cout << "Source path " << sourcePath.string() << " does not exist!\n\n";
            return;
        }

        try
        {
            if (is_directory(sourcePath))
            {
                copy(sourcePath, destinationPath, copy_options::recursive | copy_options::overwrite_existing);

                cout << "Copied folder " << sourcePath.string() << " to " << destinationPath.string() << ".\n\n";
            }
            else if (is_regular_file(sourcePath))
            {
                copy_file(sourcePath, destinationPath, copy_options::overwrite_existing);

                cout << "Copied file " << sourcePath.string() << " to " << destinationPath.string() << ".\n\n";
            }
        }
        catch (const exception& e)
        {
            cout << string(e.what()) << ".\n\n";
        }
    }

    void File::DeleteFileOrfolder(const path& sourcePath)
    {
        string output;
        if (!exists(sourcePath))
        {
            cout << "Error: " << sourcePath.string() << " does not exist!\n\n";
            return;
        }

        try
        {
            if (is_regular_file(sourcePath)) remove(sourcePath);
            else if (is_directory(sourcePath)) remove_all(sourcePath);

            cout << "Deleted " << sourcePath.string() << ".\n\n";
        }
        catch (const exception& e)
        {
            cout << string(e.what()) << ".\n\n";
        }
    }

    void File::CreateNewFolder(const path& folderPath)
    {
        string output;
        if (exists(folderPath))
        {
            cout << "Error " << folderPath.string() << " already exists!\n\n";

            return;
        }

        if (is_regular_file(folderPath))
        {
            cout << "Error " << folderPath.string() << " must be a folder!\n\n";

            return;
        }

        try
        {
            create_directory(folderPath);

            cout << "Created new folder at " << folderPath.string() << ".\n\n";
        }
        catch (const exception& e)
        {
            cout << string(e.what()) << ".\n\n";
        }
    }
}