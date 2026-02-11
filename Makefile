##
# Project Title
#
# @file
# @version 0.1


CXX = c++
CXXFLAGS = -std=c++98 -Wall -Iinclude

# Source files
SRC_DIR = src
TEST_DIR = tests
MAIN_SRCS = main.cpp $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)

# Object files
MAIN_OBJS = $(MAIN_SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

# Binaries
MAIN_BIN = webserver
TEST_BIN = test_runner

all: $(MAIN_BIN) $(TEST_BIN)

$(MAIN_BIN): $(MAIN_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_BIN): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(MAIN_BIN)
	./$(MAIN_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f *.o 

fclean: clean
	rm -f $(MAIN_BIN) $(TEST_BIN)

re: fclean all

.PHONY: all clean fclean re run test


# end
