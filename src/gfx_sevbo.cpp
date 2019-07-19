#include "engine/gfx_sevbo.h"

#include <stdio.h>
#include <memory>
#include <stdlib.h>

uint16_t loadSEVBO(const char * filepath, std::vector<SEVBO*>& vbos) {
	printf("Reading SEVBO %s\n", filepath);
		
	// Open the file
	FILE * file = fopen(filepath, "rb");
	if (!file) {
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", filepath);
		getchar();
		return 0;
	}

	char message[6];
	uint16_t num_vbo = 0;

	fread(message, 1, 5, file);
	message[5] = '\0';

	
	fread(&num_vbo, sizeof(uint16_t), 1, file);

	printf("num SEVBO: %d\n", num_vbo);
    /*
	for (int v = 0; v < num_vbo; ++v) {
		SEVBO* vbo = new SEVBO;
		size_t num_read = fread(&vbo->header, sizeof(SEVBO_header), 1, file);
		if (num_read != 1) {
			printf("Done reading file: %d\n", num_read);
			
			break;
		}

		vbo->header.filename[31] = '\0';

		//printf("VBO Header size vertices: %d, indices: %d\n", vbo.header.vert_size, vbo.header.index_size);

		vbo->data.num_vertices = vbo->header.vert_size / sizeof(SEVBO_2D_vertex);
		vbo->data.num_indices = vbo->header.index_size / sizeof(uint16_t);

		vbo->data.vertices = (SEVBO_2D_vertex*)malloc(vbo->header.vert_size);
		
		vbo->data.indices = (uint16_t*)malloc(vbo->header.index_size);

		fread(vbo->data.vertices, vbo->header.vert_size, 1, file);
		fread(vbo->data.indices, vbo->header.index_size, 1, file);
		
		//printf("VBO num vertices: %d, num indices: %d\n", vbo.data.num_vertices, vbo.data.num_indices);

		vbos.push_back(vbo);
	}*/

	fclose(file);


	return 0;
}