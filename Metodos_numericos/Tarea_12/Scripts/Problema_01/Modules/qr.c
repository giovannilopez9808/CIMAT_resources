#include "qr.h"
/* 
Obtiene la submatriz dado el número de filas que debe de tener
 */
void obtain_sub_matrix(double *matrix, double **sub_matrix, int *dimension_matrix, int n)
{
    // Definicion de la sub matriz
    *sub_matrix = (double *)malloc((dimension_matrix[0] - n) * (dimension_matrix[0] - n) * sizeof(double));
    double *Sub_m_ij, m_ij;
    // Obtiene la submatriz desde 0-n
    for (int i = 0; i < dimension_matrix[0] - n; i++)
    {
        for (int j = 0; j < dimension_matrix[0] - n; j++)
        {
            m_ij = *(matrix + (j + n) * dimension_matrix[0] + (i + n));
            Sub_m_ij = (*sub_matrix + j * (dimension_matrix[0] - n) + i);
            *Sub_m_ij = m_ij;
        }
    }
}
/* 
Aplica el metodo de Householder a los vectores para normalizarlos
 */
void obtain_Householder_vector(double *vector, double **vector_H, int *dimension)
{
    *vector_H = (double *)malloc(dimension[0] * sizeof(double));
    double norm, v_1, *VH_1;
    copy_matrix(vector,
                *vector_H,
                dimension);
    norm = obtain_cdot_multiplication(vector,
                                      vector,
                                      dimension);
    norm = sqrt(norm);
    VH_1 = *vector_H;
    v_1 = *(vector);
    *VH_1 = v_1 + norm;
}
/* 
Aplica la normalizacion Householder a la matriz de eigenvectores
 */
double *obtain_Householder_matrix(double *vector, int *dimension)
{
    double *vector_H = NULL, *H_ij, norm, I;
    int dimension_vector[2] = {dimension[0], 1};
    // Obtiene el vector Householder
    obtain_Householder_vector(vector,
                              &vector_H,
                              dimension_vector);
    //   Obtiene la norma del vector
    norm = obtain_cdot_multiplication(vector_H,
                                      vector_H,
                                      dimension_vector);
    //   Realiza la multiplicacion del vector por su transpuesta
    double *Householder_matrix = obtain_multiplication_vvT(vector_H,
                                                           dimension);
    for (int i = 0; i < dimension[0]; i++)
    {
        for (int j = 0; j < dimension[0]; j++)
        {
            // Inicializa la matriz
            H_ij = (Householder_matrix + j * dimension[0] + i);
            I = 0.0;
            if (i == j)
            {
                I = 1.0;
            }
            *H_ij = I - 2 * *H_ij / norm;
        }
    }
    free(vector_H);
    return Householder_matrix;
}
/* 
Llena la matriz de Householder sobre una matriz identidad
 */
void fill_Householder_matrix(double *matrix, double *matrix_H, int *dimension, int n)
{
    double *M_ij, mh_ij;
    for (int i = 0; i < dimension[0] - n; i++)
    {
        for (int j = 0; j < dimension[0] - n; j++)
        {
            M_ij = (matrix + (j + n) * dimension[0] + i + n);
            mh_ij = *(matrix_H + j * (dimension[0] - n) + i);
            *M_ij = mh_ij;
        }
    }
}
/*
Factorizacion QR
 */
void QR_decomposition(double *matrix, double **r_matrix, double **q_matrix, int *dimension)
{
    double *sub_matrix = NULL, *vector = NULL, *householder, *householder_aux = NULL;
    double *matrix_aux = (double *)malloc(dimension[0] * dimension[1] * sizeof(double));
    *r_matrix = (double *)malloc(dimension[0] * dimension[1] * sizeof(double));
    *q_matrix = create_identity_matrix(dimension);
    int sub_dimension[2];
    copy_matrix(matrix,
                *r_matrix,
                dimension);
    for (int i = 0; i < dimension[0]; i++)
    {
        sub_dimension[0] = dimension[0] - i;
        sub_dimension[1] = dimension[1] - i;
        householder = create_identity_matrix(dimension);
        vector = (double *)malloc(sub_dimension[0] * sizeof(double));
        obtain_sub_matrix(*r_matrix,
                          &sub_matrix,
                          dimension,
                          i);
        obtain_vector_i(sub_matrix,
                        vector,
                        sub_dimension,
                        0);
        householder_aux = obtain_Householder_matrix(vector,
                                                    sub_dimension);
        fill_Householder_matrix(householder,
                                householder_aux,
                                dimension,
                                i);
        obtain_multiplication_matrix(householder,
                                     *r_matrix,
                                     matrix_aux,
                                     dimension,
                                     dimension);
        copy_matrix(matrix_aux,
                    *r_matrix,
                    dimension);
        obtain_multiplication_matrix(*q_matrix,
                                     householder,
                                     matrix_aux,
                                     dimension,
                                     dimension);
        copy_matrix(matrix_aux,
                    *q_matrix,
                    dimension);
        free(householder_aux);
        free(householder);
        free(sub_matrix);
        free(vector);
    }
    free(matrix_aux);
}