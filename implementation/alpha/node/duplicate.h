#define MAX_NUM_CHILDREN    (5)

int8_t children[MAX_NUM_CHILDREN];
int8_t last_received_packet[MAX_NUM_CHILDREN];

/* 
 *  initializes all slots in children array to -1
 */
void initialize_children_array() {
    for(int i = 0; i < MAX_NUM_CHILDREN; i++) {
        children[i] = -1;
        last_received_packet[i] = -1;
    }
}

/*
 *  find the index of children if it exists in chilren array
 *  else put it in the array
 */
int8_t find_child(uint8_t id) {
    bool found = false;
    uint8_t child_index = -1;

    for(int i = 0; i < MAX_NUM_CHILDREN; i++) {
        if(children[i] == id) {
            found = true;
            child_index = i;
            break;
        } else if(children[i] == -1) {
            child_index = i;
            break;
        }
    }

    if(found == false && child_index != -1){
        children[child_index] = id;
    }

    return child_index;
}

/* 
 * checks whether a packet is a duplicate
 */
bool is_duplicate(uint8_t packet_number, uint8_t id) {
    uint8_t child_index = find_child(id);

    // ran out of slots in array!
    // should never come here
    if(child_index == -1) {
        return false;
    }

    if(last_received_packet[child_index] == packet_number) {
        return true;
    } else {
        last_received_packet[child_index] = packet_number;
        return false;
    }
}
