#include "consola.h"

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "-test") == 0)
		return run_tests();
	else
	{
		/* ---------------- LOGGING ---------------- */

		logger = iniciar_logger("consola.log", "CONSOLA", LOG_LEVEL_DEBUG);

		log_info(logger, "\n Iniciando consola...");

		/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

		obtenerArgumentos(argc, argv); // Recibe 3 argumentos, ./consola, la ruta del archivoConfig y la ruta de las instrucciones de pseudocodigo


		/* ---------------- LEER DE CONSOLA ---------------- */
	
		FILE *instructionsFile = abrirArchivo(argv[2]);
		
		t_informacion *informacion = crearInformacion();

		agregarInstruccionesDesdeArchivo(instructionsFile, informacion->instrucciones);

		t_paquete *nuevoPaquete = crear_paquete_programa(informacion);

		conexion = crear_conexion(configConsola.ipKernel, configConsola.puertoKernel);

		enviar_mensaje("Hola", conexion);

		// Armamos y enviamos el paquete
		paquete(conexion);

		
//--------
		enviar_paquete(nuevoPaquete, conexion);
		eliminar_paquete(nuevoPaquete);
		liberar_programa(informacion);
		//---
		terminar_programa(conexion, logger, config);
	}
}

void leerConfig(char *rutaConfig)
{
	config = iniciar_config(rutaConfig);
	extraerDatosConfig(config);

	printf(PRINT_COLOR_GREEN "\n===== Archivo de configuracion =====\n IP: %s \n PUERTO: %s" PRINT_COLOR_RESET, configConsola.ipKernel, configConsola.puertoKernel);
	 printf("=== SEGEMENTOS: %s", configConsola.segmentos->head);


}

void obtenerArgumentos(int argc, char **argv)
{

	if (argc != 3)
	{
		printf(PRINT_COLOR_RED "\nError: cantidad de argumentos incorrecta:" PRINT_COLOR_RESET "\n");
		// return -1;
	}
	else
	{
		rutaArchivoConfiguracion = argv[1];
		rutaInstrucciones = argv[2];

		leerConfig(rutaArchivoConfiguracion);

		printf(PRINT_COLOR_GREEN "\nCantidad de argumentos de entrada Correctos!" PRINT_COLOR_RESET "\n");
	}

    printf("=== Argumentos de entrada ===\n rutaArchivoConfig: %s \n rutaArchivoDeInstrucciones: %s \n\n", rutaArchivoConfiguracion, rutaInstrucciones);
}

FILE *abrirArchivo(char *filename)
{
	if (filename == NULL)
	{
		log_error(logger, "Error: debe informar un archivo de instrucciones.");
		exit(1);
	}
	return fopen(filename, "r");
}


void agregarInstruccionesDesdeArchivo(FILE *instructionsFile, t_list *instrucciones)
{
	if (instructionsFile == NULL)
	{
		log_error(logger,
				  "Error: no se pudo abrir el archivo de instrucciones.");
		exit(1);
	}
	const unsigned MAX_LENGTH = 256;
	char buffer[MAX_LENGTH];
	while (fgets(buffer, 256, instructionsFile) != NULL)
	{
		int i = 0;
		char **palabra = string_split(buffer, " ");
		t_instruccion *instr = malloc(sizeof(t_instruccion));

		if (strcmp(palabra[i], "SET") == 0)
		{
			instr->instCode = SET;
			instr->paramReg[0] = devolverRegistro(palabra[1]);
			instr->paramInt = atoi(palabra[2]);
			free(palabra[0]);
			free(palabra[1]);
			free(palabra[2]);
		}
		else if (strcmp(palabra[0], "ADD") == 0)
		{
			instr->instCode = ADD;
			instr->paramReg[0] = devolverRegistro(palabra[1]);
			instr->paramReg[1] = devolverRegistro(palabra[2]);
			free(palabra[0]);
			free(palabra[1]);
			free(palabra[2]);
		}
		else if (strcmp(palabra[0], "MOV_IN") == 0)
		{
			instr->instCode = MOV_IN;
			instr->paramReg[0] = devolverRegistro(palabra[1]);
			instr->paramInt = atoi(palabra[2]);
			free(palabra[0]);
			free(palabra[1]);
			free(palabra[2]);
		}
		else if (strcmp(palabra[0], "MOV_OUT") == 0)
		{
			instr->instCode = MOV_OUT;
			instr->paramInt = atoi(palabra[1]);
			instr->paramReg[0] = devolverRegistro(palabra[2]);
			free(palabra[0]);
			free(palabra[1]);
			free(palabra[2]);
		}
		else if (strcmp(palabra[0], "I/O") == 0)
		{
			instr->instCode = IO;
			if (strcmp(palabra[1],"DISCO")==0) 
			{
				instr->paramIO = DISCO;
				instr->paramInt = atoi(palabra[2]);
				free(palabra[0]);
				free(palabra[1]);
				free(palabra[2]);
			} else if(strcmp(palabra[1],"TECLADO")==0) 
			{
				instr->paramIO = TECLADO;
				instr->paramReg[0] = devolverRegistro(palabra[2]);
				free(palabra[0]);
				free(palabra[1]);
				free(palabra[2]);
			} else if(strcmp(palabra[1],"PANTALLA")==0) 
			{
				instr->paramIO = PANTALLA;
				instr->paramReg[0] = devolverRegistro(palabra[2]);
				free(palabra[0]);
				free(palabra[1]);
				free(palabra[2]);
			}		
		} else if (strcmp(palabra[0], "EXIT") == 0)
		{
			instr->instCode = EXIT;
			instr->paramInt = NULL; //TODO Cheaquear este Warning!

			free(palabra[0]);
		}
		list_add(instrucciones, instr);

		free(palabra);
	}
	fclose(instructionsFile);
	log_info(logger, "Se parsearon #Instrucciones: %d", list_size(instrucciones));

}

t_configConsola extraerDatosConfig(t_config *archivoConfig) 
{
	configConsola.ipKernel = string_new();
	configConsola.puertoKernel = string_new();
	configConsola.segmentos = string_array_new();  //CHEQUEAR

	configConsola.ipKernel = config_get_string_value(archivoConfig, "IP_KERNEL");
	configConsola.puertoKernel = config_get_string_value(archivoConfig, "PUERTO_KERNEL");
	configConsola.segmentos =  config_get_array_value(archivoConfig,"SEGMENTOS"); //CHEQUEAR, CREO QUE TOMA LA LISTA PERO FALTARIA AGREGAR
																				 //UN FOR TAL VEZ PARA RECORRER LA LISTA DE LOS SEGMENTOS


	return configConsola;
}

t_registro devolverRegistro(char* registro){

	if (strcmp(registro, "AX") == 0) {
		return AX;
		} else if (strcmp(registro, "BX") == 0) {
			return BX;
		} else if (strcmp(registro, "CX") == 0) {
			return CX;
		} else if (strcmp(registro, "DX") == 0) {
			return DX;
		}
}

t_informacion* crearInformacion() {
	t_informacion *informacion = malloc(sizeof(t_informacion));
	informacion->instrucciones = list_create();
	informacion->segmentos = list_create(); // Hay que cambiarlo por la lista de segmentos!
	return informacion;
}


void liberar_programa(t_informacion* informacion) {
	list_destroy_and_destroy_elements(informacion->instrucciones, free);
	free(informacion);
}

t_paquete* crear_paquete_programa(t_informacion* informacion) {
	t_buffer *buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(uint32_t)+ list_size(informacion->instrucciones) * sizeof(t_instruccion) + list_size(informacion-> segmentos) * sizeof(t_instruccion);

	void* stream = malloc(buffer->size);

	int offset = 0; // Desplazamiento

		//Serializa las instrucciones
	int i = 0;
	int j = 0;
	while (i < list_size(informacion->instrucciones)) {
		memcpy(stream + offset, list_get(informacion->instrucciones, i),sizeof(t_instruccion));
		offset += sizeof(t_instruccion);
		i++;
		printf(PRINT_COLOR_MAGENTA "Estoy serializando las instruccion %d" PRINT_COLOR_RESET "\n",i);
	}


		//Serializa los segmentos (La lista esta vacia todavia, hay que apuntar a esa lista!)
		while (j < list_size(informacion->segmentos)) {
	
		memcpy(stream + offset, list_get(informacion->segmentos, j),sizeof(t_instruccion));
		offset += sizeof(t_instruccion);
		j++;
		printf(PRINT_COLOR_YELLOW "Estoy serializando el segmento %d" PRINT_COLOR_RESET "\n",j);
	}

	
	buffer->stream = stream;

	//lleno el paquete
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = NEW;
	paquete->buffer = buffer;
	return paquete;
}