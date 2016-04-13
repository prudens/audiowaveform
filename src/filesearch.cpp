#include "filesearch.h"
#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <set>
#include "unittest.h"
using namespace std;
using namespace std::tr2::sys;
std::vector<path> file_list;
std::set<std::string> header_list;
void search( std::string basedir )
{
    path dir = basedir;
   // std::cout << dir << endl;
    directory_iterator end;
    for (directory_iterator file(dir); file != end; file++)
    {
        if (is_directory(*file))
        {
            search( file->path().string() );
        }
        else
        {
            auto ext = file->path().extension();
            if (ext == ".h" || ext == ".cc" || ext == ".cpp" || ext == ".c")
            {
                file_list.push_back( file->path() );
            }
            else
            {
               // std::cout << *file << endl;
            }
            
        }
    }
}

void search_header(path file)
{
    ifstream ifs( file );
    string line;
    regex re("[\\s]*#include[\\s]*(<(.*)>|\\\"(.+)\\\").*" );
    std::smatch base_match;
    string strBase( "E:/webrtc-9540-d92f267-windows/src/" );
    while ( getline( ifs, line ) )
    {
        if (line.find("webrtc") == -1)
        {
            continue;
        }
        if ( std::regex_match( line, base_match, re ) )
        {
            for ( size_t i = 2; i < base_match.size(); i++)
            {
                if (base_match[i].str().empty())
                {
                    continue;
                }
                if (base_match[i].str().find("webrtc") != -1)
                {
                    header_list.insert( strBase + base_match[i].str() );
                    break;
                }
            }
        }
    }

}


CASE_TEST( NullType, search )
{
    EXPECT_EQ( true, true );
    search( "E:/webrtc-9540-d92f267-windows/src/webrtc/modules/audio_processing" );
    for (auto &file :file_list)
    {
        search_header( file );
    }

    std::cout << "Total header:" << header_list.size() << endl;


}