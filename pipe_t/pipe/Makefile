SRC = $(wildcard *.cc)
OBJ = $(patsubst %.cc, %.o, $(SRC))

ALL: test

#test: $(OBJ)
test: $(SRC)
	g++ $^ -o $@ -std=c++11

#$(OBJ): $(SRC)
#	g++ -c $^ -o $@

clean:
	rm -f  test
#	rm -rf  test $(OBJ)

.PHONY: clean ALL
