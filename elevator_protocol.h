#ifndef ELEVATOR_PROTOCOL_H
#define ELEVATOR_PROTOCOL_H

#define ELEVATOR_HEIGHT_PER_FLOOR 5000 // mm
#define N_FLOORS 16
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
typedef enum {
    MSG_UNKNOWN = 0,
    MSG_RESPONSE_1,
    MSG_RESPONSE_2,
    MSG_COMMAND_1,
    MSG_COMMAND_2
} elevator_msg_type;

typedef enum {
    WAITING_COMMAND,
    RECEIVING_COMMAND,
    SENDING_COMMAND_RESPONSE,
    DOOR_OPENING,
    DOOR_CLOSING,
    MOVING_UP,
    MOVING_DOWN,
    STOPPED
} elevator_state_machine;

// Elevadores
typedef enum{
    LEFT = 'e',
    CENTER = 'c',
    RIGHT = 'd'
} elevator_position;

// Comandos simples
typedef enum{
    INIT_CMD = 'r',
    OPEN_CMD = 'a',
    CLOSE_CMD = 'f',
    UP_CMD = 's',
    DOWN_CMD = 'd',
    STOP_CMD = 'p',
    QUERY_POSITION_CMD = 'x'
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
} receive_response_1;

typedef struct{
    elevator_event event;
} receive_response_2;

//<elevador>I (i maiúsculo)<botão>
typedef struct{
    elevator_position elevator;
    char carecter_ID; // caracter: 'I'
    elevator_btt_id btt_pressed;
} receive_command_1;

//<elevador>E<andar_dezena><andar_unidade><direção do botâo>
typedef struct{
    elevator_position elevator;
    char carecter_ID; // caracter: 'E'
    uint8_t floor;         // 0–15
    call_direction dir;
} receive_command_2;

//<elevador><comando><parâmetro>D
typedef struct {
    elevator_position elevator;
    elevator_command command;
    elevator_btt_id button;
    uint8_t end; // caracter: 0xD
} trasmit_command;

typedef struct {
    elevator_position elevator;
    uint8_t current_floor;         // 0–15
    call_direction dir;
    uint32_t elevator_height;      // mm
    bool requests[N_FLOORS];
    elevator_state_machine state_machine;
} elevator_current_state;

void run_operation(elevator_current_state *state);

#endif // ELEVATOR_PROTOCOL_H
