#define MAX_NUM_CHILDREN 16

uint8_t children[MAX_NUM_CHILDREN];
uint8_t packet_numbers[MAX_NUM_CHILDREN];
uint8_t origin_ids[MAX_NUM_CHILDREN];

/* 
 *  initializes all slots in children array to -1
 */
void initialize_children_array() {
    for (uint8_t i = 0; i < MAX_NUM_CHILDREN; i++) {
        // NOTE: in terms of making the first lookup correctly invalid, default packet_id doesn't matter - the first lookup is invalidated because origin_id is set to NO_ID
        children[i] = NO_ID;
        origin_ids[i] = NO_ID;
    }
}

/*
 *  find the index of children if it exists in chilren array
 *  else put it in the array
 */
uint8_t find_child(uint8_t current_id) {
    uint8_t child_index = NO_ID;

    for (uint8_t i = 0; i < MAX_NUM_CHILDREN; i++) {
        if (children[i] == current_id) {
            child_index = i;
            break;
        } else if (children[i] == NO_ID) {
            // Empty slot, insert new child
            children[i] = current_id;
            child_index = i;
            break;
        }
    }

    return child_index;
}

/* 
 * checks whether a packet is a duplicate
 */
bool is_duplicate(Packet p) {
    uint8_t child_index = find_child(p.current_id);

    // ran out of slots in array!
    // should never come here
    if (child_index == NO_ID) {
        return false;
    }

    if (packet_numbers[child_index] == p.number && origin_ids[child_index] == p.origin_id) {
        return true;
    } else {
        packet_number[child_index] = p.number;
        origin_ids[child_index] = p.origin_id;
        return false;
    }
}
