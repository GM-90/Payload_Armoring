#include "Payload_Armoring.h"


/**********************************************************************
 *      Funciones de conversion y desconversion de caracteres ASCII
 *      para datos de origen binario.
**********************************************************************/

//Convierte el caracter ASCII a su entero equivalente de 6 bits para datos Binarios.
uint8_t Data_ASCII_2_6bit(char character){
    uint8_t val = character - 48 ;
    if( val > 40 ){
        val-=8;
    }
    return val;
}

//Convierte datos binarios en partes de 6bits a su caracter ASCII equivalente.
char Data_6bit_2_ASCII(uint8_t six_bit_data){
    uint8_t character;
    if (six_bit_data > 39)
        character = six_bit_data + 56;
    else
        character = six_bit_data + 48;
    
    return character;
}

/**********************************************************************
 *      Funciones de conversion y desconversion de caracteres ASCII
 *      para datos de origen en forma de texto. 
***********************************************************************/

char Text_6bit_2_ASCII(uint8_t six_bit_data){
    if( six_bit_data>=0 && six_bit_data<=31 ){
        return (six_bit_data | 0x40 );
    }
    else{
        return six_bit_data ;
    }
}

//Las letras minusculas se convierten a mayuscula.
//El caracter final se trunca en 6 bits.
uint8_t Text_ASCII_2_6bit(char character){
    //primero se convierte a mayusculas las letras que sean minusculas
    if(character>='a' && character<='z'){
        character -= 0x20 ;
    }
    else if(character>'z'){
        character='@';
    }
    //Ahora trunco a 6 bits y retorno valor
    return (character & 0x3F) ;
}

/********************************************************************************/


size_t decode_NMEA_message(const char * encoded_msg, char* payload_out, size_t payload_out_tam){
    size_t pay_len=strlen(encoded_msg);
    //Verifico que el payload entre en el array de salida
    if( (pay_len/4)*3 > payload_out_tam ){
        //no entra en el array de salida
        return 0;
    }
    
    unsigned int index=0;
    size_t payload_out_size=0;   //tamanio del mensaje de salida
    while(index<pay_len){
        char decoded_char = Data_ASCII_2_6bit(encoded_msg[index]);
        decoded_char&= 0x3F ;       //me aseguro de que los 2 primeros bits sean 0.
        //ahora debo encolar los bits en el array "payload_out"
        int group_index=index/4;
        int local_index=index;
        while(local_index>=4)  local_index-=4;

        switch (local_index){
        case 0:
            payload_out[(group_index*3)]  = (uint8_t)decoded_char<<2 ;
            payload_out_size++;
            break;
        case 1:
            payload_out[(group_index*3)]  = payload_out[(group_index*3)] | (uint8_t)decoded_char>>4 ;
            if(index < (pay_len-1)){        //condicion para que el ultimo caracter no genere un byte mas de salida
                payload_out[(group_index*3)+1]= (uint8_t)decoded_char<<4 ;
                payload_out_size++;                
            }
            break;
        case 2:
            payload_out[(group_index*3)+1]= payload_out[(group_index*3)+1] | (uint8_t)decoded_char>>2 ;
            if(index < (pay_len-1)){        //IDEM
                payload_out[(group_index*3)+2]= (uint8_t)decoded_char<<6 ;
                payload_out_size++;
            }
            
            break;
        case 3:
            payload_out[(group_index*3)+2]= payload_out[(group_index*3)+2] | (uint8_t)decoded_char ;
            break;
        }
        index++;
    }
    return payload_out_size;
}

int encode_NMEA_payload(const char* payload_in, size_t payload_in_tam, char * NMEA_str_out, size_t NMEA_str_out_tam){
    //borro el string de salida
    NMEA_str_out[0]='\0';

    int fill_bits = 0 ;
    size_t total_out_chars = (payload_in_tam*8)/6 ;         //cantidad de caracteres a extraer del payload.
    int bits_restantes     = (payload_in_tam*8)%6 ;            //bits restantes que no completan un caracter de 6 bits

    if(bits_restantes!=0){
        total_out_chars++;
        fill_bits = 6 - bits_restantes ;
    }
    //verifico que los caracteres de salida entren el el array NMEA_str_out.
    if(total_out_chars>NMEA_str_out_tam){
        return 0;
    }

    for(unsigned int index=0 ; index<total_out_chars ; index++ ){

        int group_index=index/4;
        int local_index=index;
        while(local_index>=4)  local_index-=4;

        uint8_t six_bit;

        int payload_index=group_index*3;

        switch(local_index){
        case 0:
            six_bit = payload_in[payload_index]>>2 ;
            break;
        case 1:
            if(index==total_out_chars-1){
                six_bit = ((payload_in[payload_index]<<4)&0x30);     //los ultimos 2 bits y 4 de relleno
            }
            else{
                six_bit = ((payload_in[payload_index]<<4)&0x30)   | (payload_in[payload_index+1]>>4) ;
            }
            break;
        case 2:
            if(index==total_out_chars-1){
                six_bit = ((payload_in[payload_index+1]<<2)&0x3C);     //los ultimos 4 bits y 2 de relleno
            }
            else{
                six_bit = ((payload_in[payload_index+1]<<2)&0x3C) | (payload_in[payload_index+2]>>6) ;
            }
            break;
        case 3:
            six_bit = payload_in[payload_index+2]&0x3F ;
            break;
        }
        char nmea_char = Data_6bit_2_ASCII(six_bit);        
        char char_str[2];
        char_str[0]=nmea_char;
        char_str[1]='\0';

        strcat(NMEA_str_out,char_str);

    }

    return fill_bits;

}


void encode_NMEA_Text(const char * input_text, char * out_text, size_t out_text_tam){
    size_t input_text_tam = strlen(input_text);
    //Verifico tamanio del array de salida
    if((input_text_tam+1)>out_text_tam){
        *out_text = '\0';
        return;
    }

    for(int i=0 ; i<=input_text_tam ; i++){
        uint8_t binary_6bit = Text_ASCII_2_6bit(input_text[i]);
        out_text[i] = Data_6bit_2_ASCII(binary_6bit);
    }
    out_text[input_text_tam] = '\0';

}


void decode_NMEA_Text(const char * input_text, char * out_text, size_t out_text_tam){
    size_t input_text_tam = strlen(input_text);
    //Verifico tamanio del array de salida
    if((input_text_tam+1)>out_text_tam){
        *out_text = '\0';
        return;
    }

    for(int i=0 ; i<=input_text_tam ; i++){
        uint8_t binary_6bit = Data_ASCII_2_6bit(input_text[i]);
        out_text[i] = Text_6bit_2_ASCII(binary_6bit);
    }
    out_text[input_text_tam] = '\0';
}