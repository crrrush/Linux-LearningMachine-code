SRC = $(wildcard *.c)
#OBJ = $(patsubst %.c, %.o, $(SRC))

ALL: test.out

test.out: $(SRC)
	gcc $^ -o $@

#$(OBJ): $(SRC)
#	gcc -c $^ -o $@

clean:
	rm -rf  test.out 

.PHONY: clean ALL
