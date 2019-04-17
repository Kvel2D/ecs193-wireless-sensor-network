#define QUEUE_SIZE_MAX  20

struct Queue {
    Packet data[QUEUE_SIZE_MAX];
    uint8_t head = 0;
    uint8_t size = 0;

    void push(Packet packet) {
        if (size < QUEUE_SIZE_MAX - 1) {
            data[(head + size) % QUEUE_SIZE_MAX] = packet;
            size++;
        }
    }

    Packet front() {
        return data[head];
    }

    Packet pop() {
        if (size > 0) {
            uint8_t old_head = head;
            head = (head + 1) % QUEUE_SIZE_MAX;
            size--;

            return data[old_head];
        }
    }
};
