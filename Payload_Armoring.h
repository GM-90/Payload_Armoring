#ifndef _NMEA_TOOLS_
#define _NMEA_TOOLS_

#include <Arduino.h>


/// @brief Decodifica un mensaje de ASCII a un array binario
/// @brief El payload de salida no termina con caracter nulo.
/// @param encoded_msg          Array con los datos codificados. Debe ser Null_terminated.
/// @param payload_out          Buffer donde se colocarán los datos binarios de salida.
/// @param payload_out_tam      Tamanio del buffer de salida.
/// @return Tamanio de los datos binarios.
size_t decode_NMEA_message(const char * encoded_msg, char* payload_out, size_t payload_out_tam);

/// @brief Codifica un payload de datos para ser insertado como caracteres
/// @brief en un mensaje NMEA. Es null terminated.
/// @param payload_in            Array con la informacion de entrada.
/// @param payload_in_tam        tamaño del payload de entrada.
/// @param NMEA_str_out          Array de salida con los datos codificados.
/// @param NMEA_str_out_tam      sizeof de NMEA_str_out.
/// @return Fill bits. Parametro a agregar en la sentencia NMEA.
int encode_NMEA_payload(const char* payload_in, size_t payload_in_tam, char * NMEA_str_out, size_t NMEA_str_out_tam);

/// @brief Conversion de texto a secuencia de caracteres
void encode_NMEA_Text(const char * input_text, char * out_text, size_t out_text_tam);

/// @brief Conversion de secuencia de caracteres a texto.
void decode_NMEA_Text(const char * input_text, char * out_text, size_t out_text_tam);


#endif