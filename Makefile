CXX = g++
FLAGS = -std=c++20 -O3 -march=native -Wall -Wextra -pedantic
INCLUDE = -I /usr/include/python3.12 -I ./Examples/pybind11/include

default:
	$(CXX) $(FLAGS) -I . -c Examples/Binomial.cpp
	$(CXX) $(FLAGS) -I . -c main.cpp
	$(CXX) -Wno-unused-variable -Wno-attributes -Wno-unused-parameter $(FLAGS) -I . $(INCLUDE) -c Examples/PythonModel.cpp
	$(CXX) -Wno-unused-variable -Wno-attributes -Wno-unused-parameter $(FLAGS) -I . $(INCLUDE) -c main_python.cpp
	$(CXX) -o main main.o Binomial.o
	$(CXX) -o main_python main_python.o PythonModel.o -lpython3.12
	rm -f *.o
