#ifndef ELEVATOR_PROTOCOL_H
#define ELEVATOR_PROTOCOL_H

#include <stdint.h>

// Elevadores
typedef enum{
    LEFT = 'e',
    CENTER = 'c',
    RIGHT = 'd'
} elevator_position;

// Comandos simples
typedef enum{
    INIT = 'r',
    OPEN = 'a',
    CLOSE = 'f',
    UP = 's',
    DOWN = 'd',
    STOP = 'p',
    QUERY_POSITION = 'x'
} elevator_command;

// Comandos com parâmetro
typedef enum{
    TURN_ON = 'L',
    TURN_OFF = 'D'
} elevator_btt_light;

// Botões internos
typedef enum{
    GROUND = 'a',
    FLOOR_1 = 'b',
    FLOOR_2 = 'c',
    FLOOR_3 = 'd',
    FLOOR_4 = 'e',
    FLOOR_5 = 'f',
    FLOOR_6 = 'g',
    FLOOR_7 = 'h',
    FLOOR_8 = 'i',
    FLOOR_9 = 'j',
    FLOOR_10 = 'k',
    FLOOR_11 = 'l',
    FLOOR_12 = 'm',
    FLOOR_13 = 'n',
    FLOOR_14 = 'o',
    FLOOR_15 = 'p'
} elevator_btt_id;

typedef enum{
    AT_GROUND   = '0',
    AT_FLOOR_1  = '1',
    AT_FLOOR_2  = '2',
    AT_FLOOR_3  = '3',
    AT_FLOOR_4  = '4',
    AT_FLOOR_5  = '5',
    AT_FLOOR_6  = '6',
    AT_FLOOR_7  = '7',
    AT_FLOOR_8  = '8',
    AT_FLOOR_9  = '9',
    AT_FLOOR_10 = 'a',
    AT_FLOOR_11 = 'b',
    AT_FLOOR_12 = 'c',
    AT_FLOOR_13 = 'd',
    AT_FLOOR_14 = 'e',
    AT_FLOOR_15 = 'f',
    DOOR_OPEN   = 'A',
    DOOR_CLOSED = 'F'
} elevator_event;

typedef enum{
    UP = 's',
    DOWN = 'd'
} call_direction;

typedef struct{
    uint32_t elevator_height;
} receive_command_1;

typedef struct{
    elevator_event event;
} receive_command_2;

typedef struct{
    elevator_position elevator;
    elevator_btt_id btt_pressed;
} receive_command_3;


typedef struct{
    elevator_position elevator;
    uint8_t floor;         // 0–15
    call_direction dir;
} receive_command_4;

typedef struct {
    elevator_position elevator;
    elevator_command command;
    elevator_btt_id button;
    uint8_t end; // caracter: 0xD
} trasmit_command;

#endif // ELEVATOR_PROTOCOL_H
