CC = g++
CFLAGS = -Wall -g -O3 -I.
DEPS = helper.h structs.h vec3.h color.h ray.h face.h pointLight.h faceList.h
OBJ = main.o helper.o structs.o
 
%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

raytrace: $(OBJ)
	$(CC)  -o $@ $^ $(CFLAGS)
