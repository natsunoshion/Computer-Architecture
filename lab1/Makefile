sim:
	gcc -g -O2 ./src/shell.c ./src/sim.c -o ./src/sim

run:
	src/sim inputs/$(arg1)

.PHONY: clean
clean:
	rm -rf ./src/*.o ./src/*~ ./src/sim
