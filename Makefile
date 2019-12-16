lib_objs = buffer_manager.o file_manager.o

app : binary_search insertion merge_sort
 
binary_search : $(lib_objs) binary_search.o
	     g++ -std=c++11 -o binary_search $(lib_objs) binary_search.o

insertion : $(lib_objs) insertion.o
	     g++ -std=c++11 -o insertion $(lib_objs) insertion.o

merge_sort : $(lib_objs) merge_sort.o
	     g++ -std=c++11 -o merge_sort $(lib_objs) merge_sort.o

binary_search.o : binary_search.cpp
	g++ -std=c++11 -c binary_search.cpp

insertion.o : insertion.cpp
	g++ -std=c++11 -c insertion.cpp

merge_sort.o : merge_sort.cpp
	g++ -std=c++11 -c merge_sort.cpp

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -std=c++11 -c file_manager.cpp

clean :
	rm -f *.o
	rm -f binary_search insertion merge_sort
