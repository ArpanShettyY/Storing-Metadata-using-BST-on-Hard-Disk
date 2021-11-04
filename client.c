#include "assignment_4.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
	char filename[100] = "myfile.dat";
	// scanf("%s", filename);
	

	FILE * fp = init_tree(filename);
	int n;
	int choice;

	do{
		// printf("\n1.Insert Key\n2.Delete key\n3.Inorder display\n4.Preorder display\n5.Print FileSize\n>>> ");
		scanf("%d", &choice);
		switch(choice){
			
			case 1: 
				scanf("%d", &n);
				insert_key(n, fp);
				break;

			case 2:
				scanf("%d", &n);
				delete_key(n, fp);
				break;

			case 3:
				display_inorder(fp);
				break;

			case 4:
				display_preorder(fp);
				break;

			case 5:
				fseek(fp, 0, SEEK_END);
				printf("%ld Bytes --> %ld entries\n", ftell(fp), (ftell(fp)-8)/12);
				break;

			default:
				return 0;
		}
	}while(choice);


	close_tree(fp);
	return 0;
}