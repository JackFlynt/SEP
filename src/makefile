BOOSTPATH:=.
all: app

app:
	g++ -I ${BOOSTPATH} -std=c++17 -O3 -o app.exe main.cpp structures.h parser.h parser.cpp hull.h hull.cpp merge.h merge.cpp simplify.h simplify.cpp

lib:
	g++ -I ${BOOSTPATH} -O3 -c api.cpp hull.cpp merge.cpp simplify.cpp parser.cpp
	g++ -fno-keep-inline-dllexport -shared -o hullsimp.dll api.o hull.o merge.o simplify.o parser.o

libtest:
	g++ -I ${BOOSTPATH} -o libtest.exe test_dll.cpp hullsimp.dll

clean:
	del *.exe *.o *.h.gch