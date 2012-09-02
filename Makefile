OPTS = -Wall -Ofast -fmodulo-sched -funsafe-loop-optimizations 

# Bad Args
#-fipa-pta                                  \
#-freschedule-modulo-scheduled-loops        \
#-fdelete-null-pointer-checks               \
#-fmodulo-sched-allow-regmoves              \
#-fmerge-all-constants                       \
#-fomit-frame-pointer                       


build:
	gcc crack.cpp cube/geom.cpp -I. -Icube -lstdc++ -lrt -o sbcrack $(OPTS)
clean:
	rm sbcrack
	find . -name "*.o" | xargs rm
