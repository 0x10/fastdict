/*******************************************************************************
 * @file dea.h
 * @brief main library header for dea lib
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
#ifndef __DEA_H_
#define __DEA_H_

#include <string>
#include <vector>


namespace fastdict
{


typedef enum {
    CHAR,
    SPECIAL,
    INVALID
} dea_char_type_t;


/* these are reg ex specific chars not dea specific
 * dea specials group n transitions to a single
 */
typedef enum {
    ANY_SYMBOL        = '.',
    ANY_WHITESPACE    = 'w',
    ANY_DIGIT         = 'd'
} dea_special_types_t;


struct dea_input_symbol_t
{
    dea_input_symbol_t( char c, dea_char_type_t t ) :
        symbol( c ),
        type( t )
    {
    }

    char            symbol;
    dea_char_type_t type;
};


/*******************************************************************************
 *
 ******************************************************************************/
class DeaTransition
{
public:
    DeaTransition( size_t             trg,
                    dea_input_symbol_t input_symbol ) :
        m_input_symbol( input_symbol ),
        m_next_state( trg )
    {
    }
    ~DeaTransition()
    {
    }


public:
    /**************************************************************************
     *
     **************************************************************************/
    void print()
    {
        if ( CHAR == m_input_symbol.type )
            printf("              |--> \"%c\" ==> %zd\n", m_input_symbol.symbol, m_next_state );
        else
            printf("              |--> [%c] ==> %zd\n", m_input_symbol.symbol, m_next_state );
    }

    /**************************************************************************
     *
     **************************************************************************/
    bool process_symbol( char symbol, unsigned char verbose=0 )
    {
        bool matching = false;
        if ( 0 != verbose ) 
            printf("%c ?= %c ", m_input_symbol.symbol, symbol );

        switch ( m_input_symbol.type )
        {
            case SPECIAL:
                matching = process_special( symbol );
                if ( !matching )
                {
                    if ( 0 != verbose ) 
                        printf("\t FAIL");
                }
                break;
            case CHAR:
                if ( symbol == m_input_symbol.symbol )
                {
                    matching = true;
                }
                break;
            default: 
                break;
        }
        if ( 0 != verbose ) 
           printf("\n");

        return matching;
    }

    /**************************************************************************
     *
     **************************************************************************/
    size_t get_next_state()
    {
        return m_next_state;
    }
    
    /**************************************************************************
     *
     **************************************************************************/
    dea_input_symbol_t get_transition_symbol()
    {
        return m_input_symbol;
    }
private:
    /**************************************************************************
     *
     **************************************************************************/
    bool process_special( char s )
    {
        bool res = false;

        switch( m_input_symbol.symbol )
        {
            case ANY_SYMBOL:        
                res = true;
                break;
            case ANY_WHITESPACE:
                switch ( s )
                {
                    case 0x20:
                    case 0x09:
                    case 0x0A:
                    case 0x0D:
                        res = true;
                        break;
                    default: break;
                }
                break;
            case ANY_DIGIT:
                if ( ( s >= '0' ) && ( s <= '9' ) )
                {
                    res = true;
                }
                break;
            default: break;
        }

        return res;
    }

private:
    dea_input_symbol_t m_input_symbol;
    size_t             m_next_state;
};


/*******************************************************************************
 *
 ******************************************************************************/
class DeaStateImproved
{
public:
    DeaStateImproved( ssize_t accepting_index ) :
        m_accepting_index( accepting_index ),
        m_transitions()
    {
        new_transition( 0, dea_input_symbol_t( ANY_SYMBOL, SPECIAL ) );
    }

    ~DeaStateImproved()
    {
    }

public:
    /**************************************************************************
     *
     **************************************************************************/
    bool is_accepting()
    {
        return (m_accepting_index >= 0);
    }

    /**************************************************************************
     *
     **************************************************************************/
    ssize_t accepting_index()
    {
        return m_accepting_index;
    }

    /**************************************************************************
     *
     **************************************************************************/
    DeaTransition& transition( size_t at )
    {
        return m_transitions[at];
    }


    /**************************************************************************
     *
     **************************************************************************/
    size_t transition_count()
    {
        return m_transitions.size();
    }

    /**************************************************************************
     *
     **************************************************************************/
    std::vector<DeaTransition>& transitions()
    {
        return m_transitions;
    }

    /**************************************************************************
     *
     **************************************************************************/
    void print( size_t idx, bool is_current )
    {
        printf("     |--> [%zd @ %p] is_accepting == %ld", idx, static_cast<void*>(this), m_accepting_index );
        if ( false != is_current ) printf(" {*}\n");
        else printf("\n");

        for( size_t t_idx=0; t_idx < m_transitions.size(); t_idx++ )
        {
            m_transitions[t_idx].print();
        }
    }

    /**************************************************************************
     *
     **************************************************************************/
    void new_transition( size_t             trg,
                         dea_input_symbol_t input_symbol )
    {
        std::vector<DeaTransition>::iterator it;

        it = m_transitions.begin();
        m_transitions.insert( it, DeaTransition( trg, input_symbol ) );
    }

    /**************************************************************************
     *
     **************************************************************************/
    size_t process_symbol( size_t self_idx, char symbol, unsigned char verbose=0 )
    {
        size_t result = self_idx;

        for (size_t t_idx = 0; t_idx < m_transitions.size(); t_idx++ )
        {
            bool found = m_transitions[t_idx].process_symbol( symbol, verbose );
            if ( found )
            {
                result = m_transitions[t_idx].get_next_state();
                break;
            }

            if ( 0 != verbose ) 
                printf("\n");
        }

        return result;
    }

private:
    ssize_t                     m_accepting_index;
    std::vector<DeaTransition> m_transitions;
};


/*******************************************************************************
 *
 ******************************************************************************/
class DeaImproved
{
public:
    DeaImproved() :
        m_states(),
        m_current_state(0)
    {
        m_states.push_back( DeaStateImproved( -1 ) );
    }
    ~DeaImproved()
    {
    }

public:
    /**************************************************************************
     *
     **************************************************************************/
    void init()
    {
        if ( m_states.size() > 0 )
            m_current_state = 0;
    }

    /**************************************************************************
     *
     **************************************************************************/
    void print()
    {
        printf("dea %p\n", static_cast<void*>(this));
        printf(" |--> states => %zd @ %p\n", m_states.size(), static_cast<void*>(&m_states) );
        for ( size_t s_idx=0; s_idx < m_states.size() ; s_idx++ )
        {
            m_states[s_idx].print( s_idx, ( s_idx == m_current_state ) );
        }
    }

    /**************************************************************************
     *
     **************************************************************************/
    void process_symbol( char symbol, unsigned char verbose=0 )
    {
       m_current_state = m_states[m_current_state].process_symbol( m_current_state, symbol, verbose );
    }

    /**************************************************************************
     *
     **************************************************************************/
    std::vector<ssize_t> find_in_string( std::string input )
    {
        std::vector<ssize_t> result;

        size_t input_len = input.length();
        //printf("input(%zd) = %s\n", input_len, input.c_str() );
        
        init();

        for ( size_t input_idx = 0; input_idx < input_len; input_idx++ )
        {
            m_current_state = m_states[m_current_state].process_symbol( m_current_state, input[input_idx], 0 );
            if ( m_states[m_current_state].accepting_index() >= 0 )
            {
                result.push_back(m_states[m_current_state].accepting_index());
            }
        }
        //printf("\n");

        return result;
    }

    /**************************************************************************
     *
     **************************************************************************/
    std::vector<ssize_t> find_in_string_multipass( std::string input )
    {
        std::vector<ssize_t> result;

        std::vector<ssize_t> working = find_in_string(input);
        result.insert(result.end(), working.begin(), working.end());

        for ( size_t input_idx = 1; input_idx < input.length(); input_idx++ )
        {
            working = find_in_string( input.substr(input_idx) );
            result.insert(result.end(), working.begin(), working.end());
        }        

        return result;
    }


    /**************************************************************************
     *
     **************************************************************************/
    bool is_current_state_accepting()
    {
        return m_states[m_current_state].is_accepting();
    }


    /**************************************************************************
     *
     **************************************************************************/
    ssize_t accepting_index()
    {
        return m_states[m_current_state].accepting_index();
    }

    /**************************************************************************
     *
     **************************************************************************/
    void new_contains( std::string w, size_t index )
    {
        if ( w.length() > 0 )
        {
            m_states.clear();
            m_states.resize( w.length(), DeaStateImproved( -1 ) );
            m_states.push_back( DeaStateImproved( static_cast<ssize_t>(index) ) );

            m_states[0].new_transition( 1, dea_input_symbol_t( w[0], CHAR ) );

            for (size_t i=1; i<w.length(); i++ )
            {
                m_states[i].new_transition( i+1, dea_input_symbol_t( w[i], CHAR ) );
                m_states[i].new_transition( 1,   dea_input_symbol_t( w[0], CHAR ) );
                m_states[i].new_transition( 0,   dea_input_symbol_t( ANY_SYMBOL, SPECIAL ) );
            }
        }
    }


    /**************************************************************************
     *
     **************************************************************************/
    void else_contains( std::string w, size_t index )
    {
        if ( w.length() > 0 )
        {
            if ( m_states[0].transition_count() > 0 )
            {
                size_t current_state = 0;
                size_t word_starting_state = 0;

                ssize_t pending_accepting_index = -1;
                for (size_t i=0; i<w.length(); i++ )
                {
                    size_t found_idx = 0;
                    while(    ( found_idx < m_states[current_state].transition_count() )
                           && ( ( m_states[current_state].transition(found_idx).get_transition_symbol().symbol != w[i] )
                                || (m_states[current_state].transition(found_idx).get_transition_symbol().type == SPECIAL) ) )
                    {
                        found_idx++;
                    }

                    if ( found_idx == m_states[current_state].transition_count() )
                    {
                        ssize_t accepting_index = -1;
                        // not found -> add new
                        if ( i == (w.length() - 1) )
                        {
                            accepting_index = static_cast<ssize_t>(index);
                        }
                        else
                        {
                            accepting_index = -1;
                        }
                        m_states.push_back( DeaStateImproved( accepting_index ) );
                        size_t dst_state = m_states.size() - 1;
                        
                        if ( i == 0 )
                        {
                            word_starting_state = dst_state;
                        }
                        else
                        {
                            if ( ! m_states[current_state].is_accepting() )
                            {
                                size_t end_found_idx=0;
                                while (    ( end_found_idx < m_states[current_state].transition_count() )
                                        && ( m_states[current_state].transition(end_found_idx).get_transition_symbol().symbol != w[0] )
                                        && ( m_states[current_state].transition(end_found_idx).get_next_state() == word_starting_state )
                                      ) 
                                {
                                    end_found_idx++;
                                }
                                if ( end_found_idx == m_states[current_state].transition_count() )
                                    m_states[current_state].new_transition( word_starting_state, dea_input_symbol_t( w[0], CHAR ) );
                            }
                        }

                        m_states[current_state].new_transition( dst_state, dea_input_symbol_t( w[i], CHAR ) );

                        current_state = dst_state;
                    }
                    else
                    {
                        // found
                        current_state = m_states[current_state].transition(found_idx).get_next_state();
                        pending_accepting_index = m_states[current_state].accepting_index();
                        word_starting_state = current_state;
                        //std::cout << "found prefix " << std::string(w[i], 1) << " ..skip\n";
                    }

                }

                // basic idea is:
                // for each state
                //    if there is a transition on symbol w[0]
                //        check all transitions from target state if there is a transition on w[1]
                //        if false
                //          add it
                //
                // should replace find_in_string_multipass with a single find_in_string call...
                //
                // for ( DeaStateImproved& state : m_states )
                // {
                //     for ( DeaTransition transition : state.transitions() )
                //     {
                //         if ( ( transition.get_transition_symbol().symbol == w[0] ) 
                //           && ( transition.get_transition_symbol().type == CHAR) )
                //         {
                //             bool found_path = false;
                //             for ( DeaTransition next_transition : m_states[transition.get_next_state()].transitions() )
                //             {
                //                 if ( ( next_transition.get_transition_symbol().symbol == w[1] ) 
                //                   && ( next_transition.get_transition_symbol().type == CHAR) )
                //                 {
                //                     found_path = ( next_transition.get_next_state() == word_2nd_state );
                //                 }
                //             }
                //             if ( !found_path )
                //             {
                //                 // size_t end_found_idx=0;
                //                 // while (    ( end_found_idx < state.transition_count() )
                //                 //         && ( state.transition(end_found_idx).get_transition_symbol().symbol != w[1] )
                //                 //         && ( state.transition(end_found_idx).get_next_state() == word_2nd_state )
                //                 //       ) 
                //                 // {
                //                 //     end_found_idx++;
                //                 // }
                //                 // if ( end_found_idx == state.transition_count() )
                //                     state.new_transition( word_2nd_state, dea_input_symbol_t( w[1], CHAR ) );
                //             }
                //         }
                //     }
                // }

            }
            else
            {
                new_contains( w, index );
            }

            init();
        }
    }



private:
    std::vector<DeaStateImproved> m_states;
    size_t                        m_current_state;
};

}

#endif /* __DEA_H_ */
