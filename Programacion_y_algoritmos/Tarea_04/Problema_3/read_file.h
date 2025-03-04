void valid_file(FILE *file)
{
    /* 
    Validacion del archivo
     */
    if (file == NULL)
    {
        printf("File error\n");
        exit(1);
    }
}
int obtain_number_persons(FILE *file)
{
    char letter;
    int size = 0;
    letter = fgetc(file);
    while (letter != EOF)
    {
        letter = fgetc(file);
        if (letter == '\n')
        {
            size++;
        }
    }
    fclose(file);
    return size+1;
}
void read_names(FILE *file, int size, struct person *names, struct person **positions)
{
    /* 
    Lectura de los nombres del archivo con el siguiente orden: apellido_paterno apellido materno nombre(s). Realiza cada estructura a otra a través de punteros.
     */
    for (int i = 0; i < size; i++)
    {
        fscanf(file, "%s %s %[^\n]",
               names[i].last_name_1,
               names[i].last_name_2,
               names[i].name);
        positions[i] = &names[i];
    }
    fclose(file);
}