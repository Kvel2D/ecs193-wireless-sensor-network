
#define NO_ID     255

struct NodeData {
    float rx_frequency;
    uint8_t parent;
    bool has_sensor;
};

const PROGMEM NodeData tree_data[] = {
    // 0
    {
        .rx_frequency = 433.0f,
        .parent =  1,
        .has_sensor =  true,
    },
    // 1
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 2
    {
        .rx_frequency = 435.0f,
        .parent =  NO_ID,
        .has_sensor =  false,
    },
    
    // 3
    {
        .rx_frequency = 435.0f,
        .parent =  NO_ID,
        .has_sensor =  false,
    },
    
    // 4
    {
        .rx_frequency = 434.0f,
        .parent =  3,
        .has_sensor =  false,
    },
    // 5
    {
        .rx_frequency = 433.0f,
        .parent =  4,
        .has_sensor =  true,
    },

    // 6
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 7
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 8
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 9
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 10
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 11
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 12
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 13
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 14
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 15
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 16
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 17
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 18
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 19
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
    // 20
    {
        .rx_frequency = 434.0f,
        .parent =  2,
        .has_sensor =  false,
    },
};