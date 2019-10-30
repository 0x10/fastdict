/*!*****************************************************************************
 * @file fastdict.cpp
 * @brief main lib src for libfastdict
 *
 * @author Christian Kranz
 * 
 * This file is part of the Fastdict Library.
 *
 * The Fastdict Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The Fastdict Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar. If not, see <https://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "dea.h"
#include "fastdict.h"

#include <iostream>
#include <fstream>
#include <unistd.h>

namespace fastdict
{
FastDict::FastDict( const std::string input_list_name, EConvertChars conv ) :
    m_words(),
    m_list_fname(input_list_name),
    m_conv( conv ),
    m_contains_dea()
{
}

/**************************************
 *
 *************************************/
FastDict::operator const std::vector<std::string>&()
{
    return m_words;
}

/**************************************
 *
 *************************************/
size_t FastDict::size() const
{
    return m_words.size();
}


/**************************************
 *
 *************************************/
void FastDict::print_dea()
{
    m_contains_dea.print();
}

/**************************************
 *
 *************************************/
void FastDict::load_from_list( const std::string input_list_name, EConvertChars conv )
{
    if ( ! input_list_name.empty() )
    {
        m_words.clear();
        std::vector<std::string>& input_list = m_words;
        m_list_fname = input_list_name;
        m_conv = conv;

        load_list_from_file( input_list_name, input_list, conv );
    }
    else
    {
        m_list_fname = "";
        m_conv = eNone;
        m_words.clear();
    }
}


/**************************************
 *
 *************************************/
std::vector<std::string> FastDict::get_contained_words( const std::string sequence )
{
    std::vector<std::string> result_words;
    std::vector<ssize_t> result = m_contains_dea.find_in_string_multipass( sequence );
    std::vector<ssize_t>::iterator ip; 
    // Sorting the array 
    std::sort(result.begin(), result.end()); 
    // Now v becomes 1 1 2 2 3 3 3 3 7 7 8 10 

    // Using std::unique 
    ip = std::unique(result.begin(), result.end());
    // Now v becomes {1 2 3 7 8 10 * * * * * *} 
    // * means undefined 

    // Resizing the vector so as to remove the undefined terms 
    result.resize(std::distance(result.begin(), ip)); 

    for( ssize_t index : result )
    {
        if ( index >= 0 )
        {
            result_words.push_back( m_words[index] );
        }
    }

    return result_words;
}


/**************************************
 *
 *************************************/
void FastDict::load_list_from_file( const std::string list_filename,
                                    std::vector<std::string>& list,
                                    EConvertChars conv )
{
    list.clear();
    std::vector<uint8_t> file_content = get_file_buf( list_filename );
    // process....
    std::string str(file_content.begin(), file_content.end());
    std::stringstream ss(str);
    for (std::string each; std::getline(ss, each, '\n'); )
    {
        if ( !each.empty() )
        {
            switch( conv )
            {
                case eToLower: std::transform(each.begin(), each.end(), each.begin(), ::tolower); break;
                case eToUpper: std::transform(each.begin(), each.end(), each.begin(), ::toupper); break;
                default: break;
            }

            list.push_back(each);
            m_contains_dea.else_contains( each, list.size()-1 );
        }
    }
}

/**************************************
 *
 *************************************/
std::vector<uint8_t> FastDict::get_file_buf( const std::string filename )
{
    std::vector<uint8_t> result_buffer;

    std::ifstream file( filename, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    result_buffer.resize(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(result_buffer.data()), size))
    {
        std::cout << "error read file " << filename << std::endl;
    }
    return result_buffer;
}

}
