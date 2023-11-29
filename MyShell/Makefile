SRC = $(wildcard *.c)
#OBJ = $(patsubst %.c, %.o, $(SRC))

ALL: myshell

myshell: $(SRC)
	gcc $^ -o $@ -std=c99

#$(OBJ): $(SRC)
#	gcc -c $^ -o $@

clean:
	rm -rf  myshell 

.PHONY: clean ALL
