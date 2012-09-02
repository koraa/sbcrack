build:
	gcc crack.cpp cube/crypto.cpp cube/geom.cpp -I. -Icube -lstdc++ -o sbcrack
clean:
	rm sbcrack
	find . -name "*.o" | xargs rm
