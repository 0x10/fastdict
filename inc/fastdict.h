/*!*****************************************************************************
 * @file fastdict.h
 * @brief main lib header for libfastdict
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
#ifndef _FASTDICT_H_
#define _FASTDICT_H_

#include "dea.h"

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

namespace fastdict
{


/**************************************
 *
 *************************************/
class FastDict
{
public:
    typedef enum {
        eToLower,
        eToUpper,
        eNone,
    } EConvertChars;


    FastDict( const std::string input_list_name="", EConvertChars conv=eNone );
    virtual ~FastDict() {}

    /**************************************
     *
     *************************************/
    operator const std::vector<std::string>&();

    /**************************************
     *
     *************************************/
    size_t size() const;


    /**************************************
     *
     *************************************/
    void print_dea();

    /**************************************
     *
     *************************************/
    void load_from_list( const std::string input_list_name, EConvertChars conv=eNone );

    /**************************************
     *
     *************************************/
    std::vector<std::string> get_contained_words( const std::string sequence );

private:
    /**************************************
     *
     *************************************/
    void load_list_from_file( const std::string list_name, 
                              std::vector<std::string>& list,
                              EConvertChars conv=eNone  );

    /**************************************
     *
     *************************************/
    std::vector<uint8_t> get_file_buf( const std::string filename );
private:
    std::vector<std::string> m_words;
    std::string              m_list_fname;
    EConvertChars            m_conv;

    DeaImproved              m_contains_dea;
};


}



#endif /* _FASTDICT_H_ */
