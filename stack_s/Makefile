SRC = $(wildcard *.cpp)
#OBJ = $(patsubst %.c, %.o, $(SRC))

ALL: test

test: $(SRC)
	g++ $^ -o $@

#$(OBJ): $(SRC)
#	gcc -c $^ -o $@

clean:
	rm -rf  test 

.PHONY: clean ALL
