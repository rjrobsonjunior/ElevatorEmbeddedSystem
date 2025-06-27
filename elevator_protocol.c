#include "elevator_protocol.h"

call_direction get_call_direction(elevator_current_state *state) {

    bool has_down = false;
    bool has_up = false;
    for (uint8_t i = state->current_floor + 1; i < N_FLOORS; ++i) {
        if (state->requests[i]) {
            has_up = true;
            break;
        }
    }

    for (int i = state->current_floor - 1; i >= 0; --i) {
        if (state->requests[i]) {
            has_down = true;
            break;
        }
    }

    if (state->dir == UP) {
        if (has_up) state->dir = UP;
        if (has_down) state->dir = DOWN;
    } else {
        if (has_down) state->dir = DOWN;
        if (has_up) state->dir = UP;
    }
}

uint8_t get_current_floor(uint32_t elevator_height)
{
    return (uint8_t)(elevator_height / ELEVATOR_HEIGHT_PER_FLOOR);
}

elevator_msg_type detect_msg_type(const char *msg, size_t len) {
    if (!msg || len < 2) return MSG_UNKNOWN;

    // Response 1: <elevador>X<altura_4bytes>
    // Response 2: <elevador>Y<evento>
    // Command 1:  <elevador>I<botao>
    // Command 2:  <elevador>E<andar_dezena><andar_unidade><direcao>

    char id = msg[1];
    switch (id) {
        case 'X':
            return MSG_RESPONSE_1;
        case 'Y':
            return MSG_RESPONSE_2;
        case 'I':
            return MSG_COMMAND_1;
        case 'E':
            return MSG_COMMAND_2;
        default:
            return MSG_UNKNOWN;
    }
}

void run_operation(elevator_current_state *state)
{
    switch (state->state_machine)  // Assuming state_machine is an enum
    {
    case WAITING_COMMAND:
        /* code */
        break;
    case RECEIVING_COMMAND:
        /* code */
        break;
    case SENDING_COMMAND:
        /* code */
        break;
    case PROCESSING_COMMAND:    
        /* code */
        break;                  
    case DOOR_OPENING:
        /* code */      
        break;
    case DOOR_CLOSING:  
        /* code */
        break;
    case MOVING_UP:
        if (state->current_floor < N_FLOORS - 1) {  
            state->current_floor++;
            state->elevator_height += ELEVATOR_HEIGHT_PER_FLOOR;
        } else {
            state->state_machine = STOPPED;  // Reached the top floor
        }   
        break;
    case MOVING_DOWN:
        if (state->current_floor > 0) {  
            state->current_floor--;
            state->elevator_height -= ELEVATOR_HEIGHT_PER_FLOOR;
        } else {
            state->state_machine = STOPPED;  // Reached the ground floor
        }
        break;  
    case STOPPED:
        // The elevator is stopped, possibly waiting for a command or door operation    
        // You might want to check if there are any requests to process
        if (state->requests[state->current_floor]) {
            // Process the request for the current floor
            state->requests[state->current_floor] = false;  // Clear the request
            state->state_machine = DOOR_OPENING;  // Open the door
        } else {
            // No requests, stay stopped
        }
        break;
    default:
        break;
    }
}