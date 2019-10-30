libFastDict.so:
	$(CXX) -shared -fPIC -Iinc -Isrc src/fastdict.cpp -o $@ 

