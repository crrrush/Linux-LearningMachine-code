SRC = $(wildcard *.c)
#OBJ = $(patsubst %.c, %.o, $(SRC))

ALL: test

test.out: $(SRC)
	gcc $^ -o $@

#$(OBJ): $(SRC)
#	gcc -c $^ -o $@

clean:
	rm -rf  test 

.PHONY: clean ALL
