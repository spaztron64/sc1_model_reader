//sc1_model_reader.cpp
//Application that reads SoulCalibur 1 Arcade model data
//Written by spaztron64, 2021

#include <stdio.h>
#include <math.h>



typedef struct root_mesh{
	unsigned long misc_model_data_address; //not fully understood. First byte is generally "A4" and changing it breaks all rendering or makes the entire mesh invisible.
	unsigned long unknown1;
	unsigned long unknown2;
	unsigned long vertex_block_address1;
	unsigned long weirdo_address;		//In the root mesh, always points to start of model.
	unsigned long unknown3;
	unsigned long unknown4;
	unsigned long unknown5;		
}root_mesh;

typedef struct submesh{
	short Y_base;			//These tell the submesh where to derive it's coordinates
	short X_base;			//from, since they're normally expressed relative to 0,0,0 in
	short Z_base;			//the vertex blocks.					
	short padding;	
	unsigned long unknown6;		//Highly volatile. Changing these values screws up rendering and usually crashes the game.
	unsigned long vertex_block_address;
	unsigned long weirdo_address;		//sometimes this points to the very start of the model, sometimes it points to a table of strings.		
	unsigned long unknown7;		
	unsigned long unknown8;
	unsigned long unknown9;
}submesh;

typedef struct NORMAL_BLOCK{
	short X;	
	short Y;	
	short Z;
	short index;			//The high byte is generally 00, 01, or 02.
}NORMAL_BLOCK;

typedef struct VERTEX_BLOCK{
	short X;	
	short Y;	
	short Z;
	short index;			//The high byte is generally something higher than A0. 
					//With Xianghua, it's most commonly A0, B0, C0, C1, or C2.
}VERTEX_BLOCK;

typedef struct MODEL{
	char model_name[23];		//24 characters long
	root_mesh Root;
	submesh Sub;			//Repeated multiple times.
}MODEL;

MODEL sc1model = {0};
VERTEX_BLOCK vblok = {0x0000, 0x0000, 0x0000, 0x0000};
FILE *fptr;
FILE *wptr;
FILE *wptr2;
FILE *wptr3;
int vblok_counter = 0;
short separator = 0;

void read_vertex_block(){
	vblok.index = 0;
	separator += 0x0400;
	while (vblok.index != 0x03FF){
		//Read vertex blocks
		vblok_counter++;
		fread(&vblok.X,sizeof(vblok.X),1,fptr);									//Unusual order to match with the one stored in the game's RAM.
		fread(&vblok.Y,sizeof(vblok.Y),1,fptr);
		fread(&vblok.Z,sizeof(vblok.Z),1,fptr);
		fread(&vblok.index,sizeof(vblok.index),1,fptr);	
		
		if ((unsigned short)vblok.index <= 0x03FF) break;										//look for 0x000000000000FF03. This marks the end of a vertex block list.
		
		
		fwrite(&vblok.X,sizeof(vblok.X),1,wptr2);									
		fwrite(&vblok.Y,sizeof(vblok.Y),1,wptr2);
		fwrite(&vblok.Z,sizeof(vblok.Z),1,wptr2);
		fwrite(&vblok.index,sizeof(vblok.index),1,wptr2);	
		
		
		printf("\n\nVertex Block %d: \n", vblok_counter);
		printf("\nvblok.X                  = 0x%04x", (unsigned short) vblok.X);
		printf("\nvblok.Y                  = 0x%04x", (unsigned short) vblok.Y);
		printf("\nvblok.Z                  = 0x%04x", (unsigned short) vblok.Z);
		printf("\nvblok.index              = 0x%04x", (unsigned short) vblok.index);

		//Process base coords and submesh vertex block coords, and write to file.
		

		vblok.X +=(unsigned int) sc1model.Sub.X_base;
		vblok.Y +=(unsigned int) sc1model.Sub.Y_base;
		vblok.Z +=(unsigned int) sc1model.Sub.Z_base;
		vblok.Z += separator;

		fwrite(&vblok.X,sizeof(vblok.X),1,wptr3);									
		fwrite(&vblok.Y,sizeof(vblok.Y),1,wptr3);
		fwrite(&vblok.Z,sizeof(vblok.Z),1,wptr3);
		fwrite(&vblok.index,sizeof(vblok.index),1,wptr3);	

	}
}


int main(int argc, char* argv[])
{


	char ch;
	int i=0;
	unsigned long long endblock = 0; 
	long track = 0;
	sc1model.Sub.vertex_block_address = 1;
	int submesh_counter = 0;
	const char* a="0";

	//fptr = fopen("xianghua1P.bin","rb");
	fptr = fopen("xianghua1P.bin","rb");
	wptr = fopen("xianghua_submesh_positions.bin","wb");
	wptr2 = fopen("xianghua_submesh_vertices_unprocessed.bin","wb");
	wptr3 = fopen("xianghua_submesh_vertices_processed.bin","wb");

	if(fptr==NULL){
		printf("Failed to open file. Exiting...");
		return(0);
	}


	fseek(fptr,0,SEEK_SET);
	fseek(wptr,0,SEEK_SET);
	fseek(wptr2,0,SEEK_SET);
	printf("Model name              =");
	for(i=0;i<24;i++){
		sc1model.model_name[i] = fgetc(fptr);
		printf("%c",sc1model.model_name[i]);
	}
	printf("\n\nRoot mesh:\n");

	//Read model name and root_mesh struct
	fread(&sc1model.Root.misc_model_data_address,sizeof(sc1model.Root.misc_model_data_address),1,fptr);
	printf("\nmisc_model_data_address = 0x%08x", sc1model.Root.misc_model_data_address);
	fread(&sc1model.Root.unknown1,sizeof(sc1model.Root.unknown1),1,fptr);
	fread(&sc1model.Root.unknown2,sizeof(sc1model.Root.unknown2),1,fptr);
	fread(&sc1model.Root.vertex_block_address1,sizeof(sc1model.Root.vertex_block_address1),1,fptr);
	printf("\nvertex_block_address1   = 0x%08x", sc1model.Root.vertex_block_address1);
	fread(&sc1model.Root.weirdo_address,sizeof(sc1model.Root.weirdo_address),1,fptr);
	printf("\nweirdo_address          = 0x%08x", sc1model.Root.weirdo_address);
	fread(&sc1model.Root.unknown3,sizeof(sc1model.Root.unknown3),1,fptr);
	printf("\nunknown3                = 0x%08x", sc1model.Root.unknown3);
	fread(&sc1model.Root.unknown4,sizeof(sc1model.Root.unknown4),1,fptr);
	printf("\nunknown4                = 0x%08x", sc1model.Root.unknown4);
	fread(&sc1model.Root.unknown5,sizeof(sc1model.Root.unknown5),1,fptr);
	printf("\nunknown5                = 0x%08x", sc1model.Root.unknown5);

	track=ftell(fptr);
	//fseek(fptr,(sc1model.Root.vertex_block_address1 - 0x80223C58),SEEK_SET);		// 0x80223C58 is the address where Player 1's model always loads.
	fseek(fptr,(sc1model.Root.vertex_block_address1),SEEK_SET);
	read_vertex_block();
	vblok_counter=0;
	fseek(fptr,track,SEEK_SET);


	//Read submesh structs
	while (sc1model.Sub.vertex_block_address != 0x00000000){
		submesh_counter++;
		fread(&sc1model.Sub.X_base,sizeof(sc1model.Sub.X_base),1,fptr);
		fread(&sc1model.Sub.Y_base,sizeof(sc1model.Sub.Y_base),1,fptr);
		fread(&sc1model.Sub.Z_base,sizeof(sc1model.Sub.padding),1,fptr);
		fread(&sc1model.Sub.padding,sizeof(sc1model.Sub.Z_base),1,fptr);							//Unusual order to match with the one stored in the game's RAM.
		fread(&sc1model.Sub.unknown6,sizeof(sc1model.Sub.unknown6),1,fptr);
		fread(&sc1model.Sub.vertex_block_address,sizeof(sc1model.Sub.vertex_block_address),1,fptr);
		fread(&sc1model.Sub.weirdo_address,sizeof(sc1model.Sub.weirdo_address),1,fptr);
		fread(&sc1model.Sub.unknown7,sizeof(sc1model.Sub.unknown7),1,fptr);
		fread(&sc1model.Sub.unknown8,sizeof(sc1model.Sub.unknown8),1,fptr);
		fread(&sc1model.Sub.unknown9,sizeof(sc1model.Sub.unknown9),1,fptr);




		if (sc1model.Sub.vertex_block_address == 0x00000000) break;

		fwrite(&sc1model.Sub.X_base,sizeof(sc1model.Sub.X_base),1,wptr);
		fwrite(&sc1model.Sub.Y_base,sizeof(sc1model.Sub.Y_base),1,wptr);
		fwrite(&sc1model.Sub.Z_base,sizeof(sc1model.Sub.padding),1,wptr);
		fwrite(&sc1model.Sub.padding,sizeof(sc1model.Sub.Z_base),1,wptr);

		printf("\n\nSubmesh %d:\n", submesh_counter);
		printf("\nY_base                  = 0x%04x", (unsigned short) sc1model.Sub.Y_base);
		printf("\nX_base                  = 0x%04x", (unsigned short) sc1model.Sub.X_base);
		printf("\npadding                 = 0x%04x", (unsigned short) sc1model.Sub.padding);
		printf("\nZ_base                  = 0x%04x", (unsigned short) sc1model.Sub.Z_base);
		printf("\nvertex_block_address    = 0x%08x", sc1model.Sub.vertex_block_address);
		printf("\nweirdo_address          = 0x%08x", sc1model.Sub.weirdo_address);

		track=ftell(fptr);
		//fseek(fptr,(sc1model.Sub.vertex_block_address - 0x80223C58),SEEK_SET);
		fseek(fptr,(sc1model.Sub.vertex_block_address),SEEK_SET);
		read_vertex_block();
		vblok_counter=0;
		fseek(fptr,track,SEEK_SET);
	}
	scanf(a);
	return 0;
}

