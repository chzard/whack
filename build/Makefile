default:
	g++ ../src/main.cpp -o game.exe -O2 -Wall -Wno-missing-braces -Wnarrowing -I ../include/ -L ../lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
hello:
	echo hi
webtest:
	emcc -o game.html ../src/main.cpp -Os -Wall -Wno-missing-braces -Wnarrowing -I ../include/ -L ../lib/ -lraylib -s USE_GLFW=3 -DPLATFORM_WEB --preload-file ../assets/