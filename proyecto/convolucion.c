#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// compilar: gcc convolucion.c -o blurr -fopenmp
// ejecutar: ./blurr icono-128-pixmap.ppm
void cargar_imagen(u_int8_t* R, u_int8_t* G, u_int8_t* B, FILE *IMG_ppm );
void imprimir_imagen_matriz(u_int8_t* A);
void crear_blurr(u_int8_t* R,u_int8_t* R_blurr);    
void subir_imagen_rgb(u_int8_t* R, u_int8_t* G, u_int8_t* B);



// variables del formato p6 globlaes por utilidad
char formato[3];
int ancho;
int alto;
int max_color;

// nombre del ppm de salida por comodidad
char* nombre_img_out;

// handles por comodidad globales
FILE *img_in;
FILE *img_out_r;
FILE *img_out_g;
FILE *img_out_b;
FILE *img_out;




int main(int argc, char *argv[]){


// argumentos recividos: 1) imagen
// formato ppm:
//  1° linea: fomarto = p6 = texto
//  2° linea: dimenciones x,y == asci
//  todas las demas: datos en formato 255 0 0  pixeles, pero en binario.
    
// vamos a usar la variable unsigned-8bit-int para ahorrar memoria  u_int8_t x

    char* nombre_img_in=argv[1];
    nombre_img_out="out.ppm";

    u_int8_t* img_r;
    u_int8_t* img_g;
    u_int8_t* img_b;
    u_int8_t* img_r_blurr;
    u_int8_t* img_g_blurr;
    u_int8_t* img_b_blurr;

    img_in = fopen(nombre_img_in,"rb");



    fscanf(img_in,"%2s",&formato);
    fscanf(img_in,"%d %d",&ancho,&alto);
    fscanf(img_in,"%d",&max_color);
    fscanf(img_in, " ");

    img_r=malloc(alto*ancho*sizeof(u_int8_t));
    img_g=malloc(alto*ancho*sizeof(u_int8_t));
    img_b=malloc(alto*ancho*sizeof(u_int8_t));
    img_r_blurr=malloc(alto*ancho*sizeof(u_int8_t));
    img_g_blurr=malloc(alto*ancho*sizeof(u_int8_t));
    img_b_blurr=malloc(alto*ancho*sizeof(u_int8_t));

    
    cargar_imagen(img_r,img_g,img_b,img_in);
    crear_blurr(img_r,img_r_blurr);
    crear_blurr(img_g,img_g_blurr);
    crear_blurr(img_b,img_b_blurr);
    subir_imagen_rgb(img_r_blurr,img_g_blurr,img_b_blurr);



    //imprimir_imagen_matriz(img_r);
    //imprimir_imagen_matriz(img_g);
    //imprimir_imagen_matriz(img_b);

    free(img_r);
    free(img_g);
    free(img_b);
    free(img_r_blurr);
    free(img_g_blurr);
    free(img_b_blurr);
    fclose(img_in);
    return(0);
}



void cargar_imagen(u_int8_t* R,u_int8_t* G,u_int8_t* B,FILE *IMG_ppm){
    u_int8_t  temp_R;
    u_int8_t  temp_G;
    u_int8_t  temp_B;
    for(long long i=0;i<alto;i++){
        for(long long j=0;j<ancho;j++){

            fread(&temp_R,1,1,IMG_ppm);
            fread(&temp_G,1,1,IMG_ppm);
            fread(&temp_B,1,1,IMG_ppm);
            R[i*ancho+j]=temp_R;
            G[i*ancho+j]=temp_G;
            B[i*ancho+j]=temp_B;

        }
    }
    printf("\nCREAR IMAGEN FINALIZADO\n");
}



void imprimir_imagen_matriz(u_int8_t* A){
    for(long long i=0;i<alto;i++){
        for(long long j=0;j<ancho;j++){
            printf("%d\t",A[i*ancho+j]);
        }
        printf("\n");
    }
    printf("\nIMPRIMIR IMAGEN MATRIZ FINALIZADO\n");
}

void crear_blurr(u_int8_t* R,u_int8_t* R_blurr){
    // hay que hacer los bordes primero
        for(long long i=1;i<alto-1;i++){
            long long borte_izq=i*ancho;
            long long borde_der=i*ancho+ancho-1;
            // ahora a las coordenadas, le sumamos ancho y restamos para subir o bajar linea, y le sumamos o restamos uno para movernos a la izquierda o derecha.
            R_blurr[borte_izq]=(    R[borte_izq-ancho]+ R[borte_izq-ancho+1]+
                                    R[borte_izq]+       R[borte_izq+1]+
                                    R[borte_izq+ancho]+ R[borte_izq+ancho+1])/6;

            R_blurr[borde_der]=(    R[borde_der-1-ancho]+   R[borde_der-ancho]+
                                    R[borde_der-1]+         R[borde_der]+
                                    R[borde_der-1+ancho]+   R[borde_der+ancho])/6;
        }
        for(long long j=1;j<ancho-1;j++){
            long long borde_sup=0+j;
            long long borde_inf=j+ancho*(alto-1);

            R_blurr[j]=(            R[borde_sup-1]+         R[borde_sup]+           R[borde_sup+1]+
                                    R[borde_sup+ancho-1]+   R[borde_sup+ ancho]+    R[borde_sup+ancho+1])/6;

            R_blurr[borde_inf]=(    R[borde_inf-1]+         R[borde_inf]+       R[borde_inf+1]+
                                    R[borde_inf-ancho-1]+   R[borde_inf-ancho]+ R[borde_inf-ancho+1])/6;
        }   
        //ahora las 4 esquinas  
        R_blurr[0]=(                R[0]+       R[1]+
                                    R[ancho]+   R[ancho+1])/4;

        R_blurr[ancho-1]=(          R[ancho-1-1]+       R[ancho-1]+
                                    R[ancho-1-1+ancho]+ R[ancho-1+ancho])/4;

        R_blurr[ancho*(alto-1)]=(   R[ancho*(alto-1)]+      R[ancho*(alto-1)+1]+
                                    R[ancho*(alto-1)-ancho]+R[ancho*(alto-1)-ancho+1])/4;

        R_blurr[ancho*alto-1]=(     R[ancho*alto-1-1]+        R[ancho*alto-1]+
                                    R[ancho*alto-1-1-ancho]+  R[ancho*alto-1-ancho])/4;

        // ahora el resto de la matriz:
    for(long long i=1;i<alto-1;i++){
        for(long long j=1;j<ancho-1;j++){
            long long coordenada=i*ancho+j;
            R_blurr[coordenada]=(   R[coordenada-1-ancho] + R[coordenada-ancho]  +  R[coordenada+1-ancho]+
                                    R[coordenada-1]       + R[coordenada]        +  R[coordenada+1]+
                                    R[coordenada-1+ancho] + R[coordenada+ancho]  +  R[coordenada+1+ancho] )/9;


        }
    }
    printf("\nCREAR BLURR MATRIZ FINALIZADO\n");
}





void subir_imagen_rgb(u_int8_t* R, u_int8_t* G, u_int8_t* B){
    img_out = fopen("out.ppm","wb");
    if (img_out == NULL) {
        perror("Error al abrir archivo de salida");
        return;
    }
    fprintf(img_out, "P6\n");
    fprintf(img_out, "%d %d\n", ancho, alto);
    fprintf(img_out, "255\n");
    for(long long i=0;i<alto;i++){
        for(long long j=0;j<ancho;j++){
            long long coordenada=i*ancho+j;
            fwrite(&R[coordenada], 1, 1, img_out);
            fwrite(&G[coordenada], 1, 1, img_out);
            fwrite(&B[coordenada], 1, 1, img_out);
        }
    }
    fclose(img_out);
    printf("\nSUBIR IMAGEN AL PPM FINALIZADO\n");
}