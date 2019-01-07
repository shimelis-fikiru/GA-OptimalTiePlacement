/*************************************************************************************
 * Optimal Reconfiguration of Electrical Distribution System using Genetic Algorithm	*
 * 	Written by Vasudevan.B (vasu@ee.iitkgp.ernet.in)				*
 * ***********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#define MITER 100

typedef struct {
  int fb;
  int tb;
  int status;
}Feederdata;

typedef struct {
  int no;
  double Pg;
  double Qg;
  double Pl;
  double Ql;
}Busdata;

// கோப்பு சுட்டிகள்
FILE *fpf, *fpb, *fpg, *fpw;		

int **initializeAdjMatrixN(int **A, int n) {
    int i,j;
    for(i = 0; i < n; i++) {
	for(j = 0; j < n; j++) {
	    A[i][j] = 0;
	}
    }
    return A;
}

int **createAdjMatrixN(int **A, int n) {
    int i, j;
    A = (int **) malloc(n * sizeof(int *) );
    if(A == NULL) {
      printf("\nOut of memory\n");
      exit(1);
    }

    for(i = 0; i < n; i++) {
      	A[i] = (int *) malloc ( n * sizeof(int) );
	if(A[i] == NULL) {
	    printf("\nMemory error\n");
	    exit(1);
	}
    }
    A = initializeAdjMatrixN(A, n);
    return A;
}

void printAdjMatrixN(int **A, int n) {
    if (A == NULL) {
	printf("Matrix is NULL\n");
    } else {
	int i, j;
	printf("\nPrinting the Square Matrix\n-----------------\n");
	for(i = 0; i < n; i++) {
	    for(j = 0; j < n; j++) {
		printf("%d\t ", A[i][j]);
	    }
	    printf("\n");
	}
	printf("-----------------\n\n");
    }
}

Feederdata *initializeFeederData(Feederdata *fd, int n) {
    int i;
    for (i = 0; i < n; i++) {
	fd[i].fb = 0;
	fd[i].tb = 0;
	fd[i].status = 0;
    }
    return fd;
}

Feederdata *getFeederData(Feederdata *fd, char* fname, int n) {
    int i;
    fd = (Feederdata*)malloc(n * sizeof(Feederdata));
    if(fd == NULL) {
	printf("\nError in memory Allocation\n\n");
    }
    
    fd = initializeFeederData(fd, n);
    
    //Reading pdt file 
    FILE *fpf = fopen(fname, "r");
    if (fpf == NULL) {
	printf("The file %s was not opened\n", fname);
	exit(1);
    } else {
	for (i = 0; i < n; i++) {
	    fscanf(fpf, "%d %d %d", &fd[i].fb, &fd[i].tb, &fd[i].status);
// 	    printf( "\n%d\t%d\t%d", fd[i].fb, fd[i].tb, fd[i].status);
	}
    }
    fclose(fpf);
    return fd;
}

void printFeederdata(Feederdata *fd,int n) {
  int i;
    printf("\nFeeder data is as follows\n");
    for (i = 0; i < (n-1); i++) {
	printf("\n%d\t",fd[i].fb);
	printf("%d\t",fd[i].tb);
	printf("%d\t",fd[i].status);
    }
}

Busdata *initializeBusData(Busdata *bd, int n) {
    int i;
    for (i = 0; i < n; i++) {
	bd[i].no = 0;
	bd[i].Pg = 0.0;
	bd[i].Pl = 0.0;
	bd[i].Qg = 0.0;
	bd[i].Ql = 0.0;
    }
    return bd;
}

Busdata *getBusData(Busdata *bd, char* fname, int nb) {
    int i;
    bd = (Busdata *) malloc (nb * sizeof(Busdata));	
    if(bd == NULL) {
	printf("\nError in memory Allocation\n\n");
    }
    
    bd = initializeBusData(bd, nb);

    FILE *fpb = fopen(fname, "r");
    if (fpb == NULL) {
	printf("The file %s was not opened\n", fname);
	exit(1);
    } else {
	for (i = 0; i < nb; i++) {
	    fscanf(fpb, "%d %lf %lf %lf %lf", &bd[i].no, &bd[i].Pg, &bd[i].Qg, &bd[i].Pl, &bd[i].Ql);
	}
    }
    fclose(fpb);
    return bd;
}

void printBusdata(Busdata *bd,int n) {
  int i;
    printf("\nBus data is as follows\n");
    for (i = 0; i < n; i++) {
	printf("\n%d\t",bd[i].no);
	printf("%lf\t%lf\t", bd[i].Pg, bd[i].Pl);
	printf("%lf\t%lf", bd[i].Qg, bd[i].Ql);
    }
}

int **populateMatrixN(int **A, int nb, Feederdata *fd, int nl) {
    int i, j, l;
    // Creating the adjacency matrix for the given network
    for(i=0; i < nb; i++){
	for(j=0; j < nb; j++) {
	      if( i == j)
		  A[i][j] = 0;
	      else {
		for(l = 0; l < nl; l++) {
		    if(i+1 == fd[l].fb && j+1 == fd[l].tb && fd[l].status == 1) {
		      A[i][j] = 1; }
		    if(i+1==fd[l].tb && j+1 == fd[l].fb && fd[l].status ==1) {
		      A[i][j] = 1; }
		  }
	      }
	  }
	}
   return A;
}

double *initializeArrayValuesN(double *A, int n) {
    int i;
    for(i = 0; i < n; i++) {
      A[i] = 0.0;
    }
    return A;
}

double *createArrayValuesN(double *A, int n ) {
  double *array;
  array = (double *) malloc (n * sizeof(double));
  initializeArrayValuesN(array, n);
  return array;
}

void printArrayValuesN(double *A, int n) {
    if (A == NULL) {
	printf("Matrix is NULL\n");
    } else {
	printf("\nArray Element is as follows\n");
	int i;
	for(i = 0; i < n; i++) {
	  printf("%lf\n", A[i]);
	}
    }
}

double *getGenerationData(double *generation, char *fname, int n) {
    int i;
    
    generation = createArrayValuesN(generation, n);
    
    //Reading pdt file 
    FILE *fpg = fopen(fname, "r");
    if (fpg == NULL) {
	printf("The file %s was not opened\n", fname);
	exit(1);
    } else {
	for (i = 0; i < n; i++) {
	    fscanf(fpg, "%lf",&generation[i] );
	}
    }
    fclose(fpg);
    return generation;
}

double *getLoadWeightage(double *weightage, char *fname, int n) {
    int i;
    
    weightage = createArrayValuesN(weightage, n);
    
    //Reading pdt file 
    FILE *fpw = fopen(fname, "r");
    if (fpw == NULL) {
	printf("The file %s was not opened\n", fname);
	exit(1);
    } else {
	for (i = 0; i < n; i++) {
	    fscanf(fpw, "%lf",&weightage[i] );
	}
    }
    fclose(fpw);
    return weightage; 
}

int *initializeArrayValuesNint(int *A, int n) {
    int i;
    for(i = 0; i < n; i++) {
      A[i] = 0;
    }
    return A;
}

int *createArrayValuesNint(int *A, int n) {
  int *array;
  array = (int *) malloc (n * sizeof(int));
  initializeArrayValuesNint(array, n);
  return array;
}

void printArrayValuesNint(int *A, int n) {
    if (A == NULL) {
	printf("Matrix is NULL\n");
    } else {
	printf("\nArray Element is as follows\n");
	int i;
	for(i = 0; i < n; i++) {
	  printf("%d\n", A[i]);
	}
    }
}

int **initializeAdjMatrixNM(int **A, int n, int m) {
    int i,j;
    for(i = 0; i < n; i++) {
	for(j = 0; j < m; j++) {
	    A[i][j] = 0;
	}
    }
    return A;
}

int **createAdjMatrixNM(int **A, int n, int m) {
    int i;
    A = (int **) malloc(n * sizeof(int *) );

    for(i = 0; i < n; i++) {
	A[i] = (int *) malloc ( m * sizeof(int) );
    }   
    A = initializeAdjMatrixNM(A, n, m);
    
    return A;
}

void printAdjMatrixNM(int **A, int n, int m) {
    if (A == NULL) {
	printf("Matrix is NULL\n");
    } else {
	int i, j;
	printf("\n\nPrinting the matrix\n-----------------\n");
	for(i = 0; i < n; i++) {
	    for(j = 0; j < m; j++) {
		printf("%d\t", A[i][j]);
	    }
	    printf("\n");
	}
	printf("-----------------\n\n");
    }
}

int random_bus(int nb) {
    int x;
    label1:
    x = rand() % (nb+1);
    if (x <= nb && x > 1)
	return x;
    else {
	goto label1;
    }
    return x;
}

int **populateBMatrix(int **B, int nb, int NPART, int nt, int ntie) {
    int i, j, k, l, m;
    for (i = 0; i < NPART; i++) {
	for (j = 0; j < nt; j++) {
	    B[i][j] = random_bus(nb);
	}
	
	
	// Consequtive are not same
	for(l = 0; l < (nt-2); l++) {
	  for (k = l; k < nt-1; k++) {
	    if(B[i][k] == B[i][k+1]) {
	      label1:
	      B[i][k] = random_bus(nb);
	      for( m = 0; m < l; m ++) {
		if( (B[i][k] == B[i][m]) || ( B[i][k] == B[i][k+1] ) )
		  goto label1;
	      }
	    }
	  }
	}
	
    }
    return B;
} 




void FreeMemory(int **A, int n) {
    int i;
    for (i = 0; i < n; i++) free(A[i]);
    free(A);
}

// Main Program starts here
int main(int argc, char *argv[]) {
  
    int **A, **C, nb, nl;
    double *generation, *weightage; 
    
    nb = 69; nl = 68;
    
    // Objective function pointers
    int *x, *y, *prod, *madd, *LC, *Is1_Node, *Is2_Node;
    double *sirf;
    
    // Geneticc algorithm ppointers
    int **B;
    int pop_size, nt, ntie;
    
    
    pop_size = 10;
    ntie = 5; nt = 2*ntie;    
    
    A = createAdjMatrixN(A, nb);
    //printAdjMatrixN(A, nb);
    
    Feederdata *fd = getFeederData(fd, "ldata69.pdt", nl);
    //printFeederdata(fd, nb);
    
    
    Busdata *bd = getBusData(bd, "bdata69.pdt", nb);
    //printBusdata(bd, nb);
    
     A = populateMatrixN(A, nb, fd, nl);
     //printAdjMatrixN(A, nb);
     
     generation = getGenerationData(generation, "gendata69.pdt", nb);
     //printArrayValuesN(generation, nb);
    
     weightage = getLoadWeightage(weightage, "eweight69.pdt", nb);
     //printArrayValuesN(weightage, nb);
     
     C = createAdjMatrixN(C, nb);
     //printAdjMatrixN(C, nb);
     
     x = createArrayValuesNint(x, nb);
     //printArrayValuesNint(x, nb);
     
     y = createArrayValuesNint(y, nb);
     //printArrayValuesNint(y, nb);
      
     prod = createArrayValuesNint(prod, nb);
     //printArrayValuesNint(prod, nb);
     
     madd = createArrayValuesNint(madd, nb);
     //printArrayValuesNint(madd, nb);
     
     LC = createArrayValuesNint(LC, nb);
     //printArrayValuesNint(LC, nb);
    
     Is1_Node = createArrayValuesNint(Is1_Node, nb);
     //printArrayValuesNint(Is1_Node, nb);
    
     Is2_Node = createArrayValuesNint(Is2_Node, nb);
     //printArrayValuesNint(Is2_Node, nb);
     
     sirf = createArrayValuesN(sirf, nb);
     //printArrayValuesN(sirf, nb);
     
     B = createAdjMatrixNM(B, pop_size, nt);
     //printAdjMatrixNM(B, pop_size, nt);
     
     B = populateBMatrix(B, nb, pop_size, nt, ntie);
     printAdjMatrixNM(B, pop_size, nt);
     
     

     
     
     
     
     


  
    printf("\n\nவெற்றி\n\n");
    
    // Memory Clearing
    FreeMemory(A, nb);
    free(fd); free(bd);
    free(generation);
    free(weightage);
    FreeMemory(C, nb);
    free(x); free(y);
    free(prod); free(madd);
    free(LC); free(Is1_Node);
    free(Is2_Node); free(sirf);
    FreeMemory(B, pop_size);
    
    
    return 0;
}