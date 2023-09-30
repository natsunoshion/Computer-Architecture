sim:
	gcc -g -O2 ./src/shell.c ./src/sim.c -o ./src/sim

as:
	java -cp ./inputs/Mars4_5.jar MarsCompiler ./inputs/$(arg1).s

run:
	src/sim inputs/$(arg1).x

.PHONY: clean
clean:
	rm -rf ./src/*.o ./src/*~ ./src/sim
