#include "heat_equation.h"
/*
  Crea el el vector del lado derecho de la ecuacion de calor

  Parametros:
    Input, int n, numero de puntos.
    Input, double* vector, posiciones.
*/
void create_vector(Parameters parameters, double (*f)(double, double), double t, double *vector, int n)
{
  double dx = (parameters.x_max - parameters.x_min) / parameters.x_num;
  double x;
  for (int i = 1; i < n - 1; i++)
  {
    x = parameters.x_min + dx * i;
    vector[i] = f(x, t);
  }
}
/*
  Validación del proceso de factorizacion de matriz. Si el valor es cero, el programa se detiene.

  Parametros:
    Input, double* matrix, matriz que se va a factorizar
    Input, int i, posicion del valor a checar
 */
void valid_obtain_factorization(double *matrix, int i)
{
  if (matrix[1 + (i - 1) * 3] == 0.0)
  {
    printf("\n");
    printf("==== obtain_factorization - Error ====\n");
    printf("====  Division entre cero %d ====\n", i);
    exit(1);
  }
}
/*
  Realiza la factorizacion de la matriz tridimensional

  Parametros:
  Input: int dimension, dimension de la matriz
  Input: double* matrix, matriz tridiagonal

*/
void obtain_factorization(int dimension, double *matrix)
{
  for (int i = 1; i < dimension; i++)
  {
    valid_obtain_factorization(matrix, i);
    // Store the multiplier in L.
    matrix[2 + (i - 1) * 3] = matrix[2 + (i - 1) * 3] / matrix[1 + (i - 1) * 3];
    // Modify the diagonal entry in the next column.
    matrix[1 + i * 3] = matrix[1 + i * 3] - matrix[2 + (i - 1) * 3] * matrix[0 + i * 3];
  }
  valid_obtain_factorization(matrix, dimension);
}

/*
  Resuelve el sistema de ecuaciones obtenido de la matriz de la funcion obtain_factorization

  Parametros:
    Input, int n, dimension de la matriz.
    Input, double* matrix, factorizacion LU de obtain_factorization.
    Input, double* vector, vector del lado izquierdo del sistema lienal.
    Output, double* solution, solucion del sistema de ecuaciones.
*/
double *solve_matrix(int n, double *matrix, double *vector)
{
  double *solution;
  solution = (double *)malloc(n * sizeof(double));
  for (int i = 0; i < n; i++)
  {
    solution[i] = vector[i];
  }
  // Resuelve L * Y = B.
  for (int i = 1; i < n; i++)
  {
    solution[i] = solution[i] - matrix[2 + (i - 1) * 3] * solution[i - 1];
  }
  // Resuelve U * X = Y.
  for (int i = n; 1 <= i; i--)
  {
    solution[i - 1] = solution[i - 1] / matrix[1 + (i - 1) * 3];
    if (i > 1)
    {
      solution[i - 2] = solution[i - 2] - matrix[0 + (i - 1) * 3] * solution[i - 1];
    }
  }
  return solution;
}
/*
  Crea el estado inicial del sistema

  Parametros:
    Input, Parameters parameters, estructura que contiene valor del estado inicial
    Input, int n, numero de puntos.
    Input, double* vector, datos de cada posicion.
*/
void set_initial_state(Parameters parameters, double (*f)(double), double *vector, int n)
{
  double dx = (parameters.x_max - parameters.x_min) / parameters.x_num;
  double x;
  for (int i = 1; i < n - 1; i++)
  {
    x = parameters.x_min + dx * i;
    vector[i] = f(x);
  }
}
/*
  La matriz A no cambia en el tiempo
  Parametros:
    Input, double k, constante de la ecuación diferencial
    Input, double dt, diferencial del tiempo
    Input, double dx, diferencial del espacio
    Input, double x_num, numero de elementos en el espacio
    Output, double* matrix, matriz que contiene la informacion del espacio de la ecuacion de calor

*/
double *define_A_matrix(double k, double dt, double dx, int x_num)
{
  // Definicion de w
  double w = k * dt / (dx * dx);
  double *matrix = (double *)malloc(3 * x_num * sizeof(double));
  matrix[0] = 0.0;
  matrix[1] = 1.0;
  matrix[3] = 0.0;
  for (int i = 1; i < x_num - 1; i++)
  {
    matrix[2 + (i - 1) * 3] = -w;
    matrix[1 + i * 3] = 1.0 + 2.0 * w;
    matrix[0 + (i + 1) * 3] = -w;
  }
  matrix[2 + (x_num - 2) * 3] = 0.0;
  matrix[1 + (x_num - 1) * 3] = 1.0;
  matrix[2 + (x_num - 1) * 3] = 0.0;
  return matrix;
}
/*
  Funcion principal que ejecuta la solución a la ecuacion de calor no homogénea
*/
void solve_system(Parameters parameters)
{
  double *matrix, *b, *fvec, *x, *t, *u;
  int x_num, t_num;
  char *u_file = "Output/output.txt";
  double k, x_max, x_min, dx, t_max, t_min, dt, ti;
  double (*f0)(double) = parameters.f0;
  double (*f)(double, double) = parameters.f;
  k = parameters.k;

  // Guardado de los datos espaciales
  x_min = parameters.x_min;
  x_max = parameters.x_max;
  x_num = parameters.x_num;
  dx = (x_max - x_min) / (double)(x_num - 1);
  x = linspace(x_min, x_max, x_num);
  // Guardado de los valores temporales
  t_min = parameters.t_min;
  t_max = parameters.t_max;
  t_num = parameters.t_num;
  dt = (t_max - t_min) / (double)(t_num - 1);
  t = linspace(t_min, t_max, t_num);
  b = (double *)malloc(x_num * sizeof(double));
  fvec = (double *)malloc(x_num * sizeof(double));
  // Datos iniciales para el tiempo t_min
  u = (double *)malloc(x_num * t_num * sizeof(double));
  set_initial_state(parameters, f0, u, x_num);
  matrix = define_A_matrix(k, dt, dx, x_num);
  // Factorizacion de la matriz tridiagonal
  obtain_factorization(x_num, matrix);
  for (int i = 1; i < t_num; i++)
  {
    ti = parameters.t_min + dt * i;
    // Creacion del vector
    create_vector(parameters, f, ti, fvec, x_num);
    // Aporte temporal
    for (int j = 1; j < x_num - 1; j++)
    {
      b[j] = u[j + (i - 1) * x_num] + dt * fvec[j];
    }
    free(fvec);
    // Solucion del sistema
    fvec = solve_matrix(x_num, matrix, b);
    // Guardado de los resultados
    for (int j = 0; j < x_num; j++)
    {
      u[j + i * x_num] = fvec[j];
    }
  }
  // Escritura de los resultados en un archivo
  write_results(u_file, x_num, t_num, u);
  free(matrix);
  free(b);
  free(fvec);
  free(t);
  free(u);
  free(x);
}
