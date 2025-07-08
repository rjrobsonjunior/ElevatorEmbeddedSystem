#include "elevator_protocol.h"

char * mount_cmd(elevator_command cmd, elevator_position elevator, elevator_btt_id floor) 
{
    char command[MAX_CMD_SIZE];
    command[0] = (char)elevator;  // 'e', 'd', or 'c'
    command[1] = (char)cmd;
    
    if (cmd == TURN_ON || cmd == TURN_OFF) 
    {
        command[2] = (char)floor;  // Button ID
        command[3] = 0x0D;  // Null-terminate the string
        command[4] = '\0';  // Null-terminate the string
    } 
    else
    {
        command[2] = 0x0D;  // Null-terminate the string
        command[3] = '\0';  // Null-terminate the string
    }
    
    return command;
}

void get_call_direction(elevator_current_state *state) 
{
    bool has_down = false;
    bool has_up = false;
    for (uint8_t i = state->current_floor + 1; i < N_FLOORS; ++i) 
    {
        if (state->requests[i]) 
        {
            has_up = true;
            break;
        }
    }

    for (int i = state->current_floor - 1; i >= 0; --i) 
    {
        if (state->requests[i]) 
        {
            has_down = true;
            break;
        }
    }

    if (state->dir == UP) 
    {
        if (has_up) state->dir = UP;
        if (has_down) state->dir = DOWN;
    } 
    else 
    {
        if (has_down) state->dir = DOWN;
        if (has_up) state->dir = UP;
    }
}

uint8_t get_current_floor(uint32_t elevator_height)
{
    return (uint8_t)(elevator_height / ELEVATOR_HEIGHT_PER_FLOOR);
}

elevator_msg_type detect_msg_type(const char *msg) {
    // Response 1: <elevador>X<altura_4bytes>
    // Response 2: <elevador>Y<evento>
    // Command 1:  <elevador>I<botao>
    // Command 2:  <elevador>E<andar_dezena><andar_unidade><direcao>

    char id = msg[1];
    switch (id) 
    {
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
    char msg[10] = {0};  // message buffer
    switch (state->state_machine)  // Assuming state_machine is an enum
    {
    case WAITING_COMMAND:
        osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
        // get_message(msg, sizeof(msg));  // This function should fill msg with the received command
        state->state_machine = RECEIVING_COMMAND;  // Transition to receiving state
        break;
    case RECEIVING_COMMAND:
            // get the message from the input buffer
            // get_message(msg, sizeof(msg));
            switch (detect_msg_type(state->command_buffer))
            {
                case MSG_COMMAND_1:
                    state->requests[msg[2] - '0'] = true;  // Assuming msg[2] is a valid digit representing the floor
                    get_call_direction(state);
                    if(state->dir == UP) {
                        state->state_machine = MOVING_UP;
                    } else if (state->dir == DOWN) {
                        state->state_machine = MOVING_DOWN;
                    } else {
                        state->state_machine = STOPPED;  // No direction, stay stopped
                    }
                    break;
                case MSG_COMMAND_2:
                    state->current_floor = (msg[2] - '0') * 10 + (msg[3] - '0');
                    state->dir = (call_direction)msg[4];
                    //state->state_machine = MOVING_UP;  // or MOVING_DOWN based on direction
                    break;
                default:
                    break;
                }
        break;
    case SENDING_COMMAND_RESPONSE:
        /* code */
        break;
    
    case RECEIVING_COMMAND_RESPONSE:
        switch (detect_msg_type(state->response_buffer))
        {
            case MSG_RESPONSE_1:
                // Extract the elevator height from the message 
                break;
            case MSG_RESPONSE_2:    
                break;
            case MSG_UNKNOWN:
                break;
            default:
                break;
        }
    case DOOR_OPENING:
        char *open_cmd = mount_cmd(OPEN_CMD, state->elevator, GROUND);
              
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

elevator_Thread(void *argument) 
{
    elevator_current_state *elevator_state = (elevator_current_state *)argument;
    for(;;)
    {
        run_operation(elevator_state);
        osDelay(50);
    }
}