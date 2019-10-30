# fastdict
a c++ library which provides a fast specialized dictionary.

Currently the only API it offers is to load a list of ascii words and after that you can use the library to search within any ascii sequence for words from the dictionary.

It uses a deterministic finite automaton under the hood.

# Usage
```cpp
#include <fastdict.h>
...
fastdict::FastDict dict;
dict.load_from_list( "your_word_list.txt" );
...
std::vector<std::string> results = dict.get_contained_words( "some sequence" );
for( std::string w : results )
{
    std::cout << w << std::endl;
}
...
```

# Performance

_TODO: provide comparable measurements_

Measurements made during the course of the development on a single core (i7 4770) are:

Loading a word list with around 180K words took about 750ms.

Search time with input sequences between 5 and 20 characters: between 5 and 30 microseconds.



# TODO

  * improve performance by reducing the multipass call for substrings (TODO and concept are already available but won't work yet)
  * provide better test cases and examples
  * extend the api for more types of dictionary queries
