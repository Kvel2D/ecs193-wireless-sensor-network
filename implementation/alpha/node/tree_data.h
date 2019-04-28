
#define NO_ID 255

struct NodeData {
  float rx_frequency;
  uint8_t parent;
  bool has_sensor;
};

const PROGMEM NodeData tree_data[] = {
    // 0
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 1
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 2
    {
        .rx_frequency = 400.50f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 3
    {
        .rx_frequency = 433.00f,
        .parent = 4,
        .has_sensor = true,
    },
    // 4
    {
        .rx_frequency = 433.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 5
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 6
    {
        .rx_frequency = 433.00f,
        .parent = 4,
        .has_sensor = true,
    },
    // 7
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 8
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 9
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 10
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 11
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 12
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 13
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 14
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 15
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 16
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 17
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 18
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 19
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 20
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 21
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 22
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 23
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 24
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 25
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 26
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 27
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 28
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 29
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 30
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 31
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 32
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 33
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 34
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 35
    {
        .rx_frequency = 433.00f,
        .parent = 129,
        .has_sensor = true,
    },
    // 36
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 37
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 38
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 39
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 40
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 41
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 42
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 43
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 44
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 45
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 46
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 47
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 48
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 49
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 50
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 51
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 52
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 53
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 54
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 55
    {
        .rx_frequency = 434.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 56
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 57
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 58
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 59
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 60
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 61
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 62
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 63
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 64
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 65
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 66
    {
        .rx_frequency = 434.00f,
        .parent = 55,
        .has_sensor = false,
    },
    // 67
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 68
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 69
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 70
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 71
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 72
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 73
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 74
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 75
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 76
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 77
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 78
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 79
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 80
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 81
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 82
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 83
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 84
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 85
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 86
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 87
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 88
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 89
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 90
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 91
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 92
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 93
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 94
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 95
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 96
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 97
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 98
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 99
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 100
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 101
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 102
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 103
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 104
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 105
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 106
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 107
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 108
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 109
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 110
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 111
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 112
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 113
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 114
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 115
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 116
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 117
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 118
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 119
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 120
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 121
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 122
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 123
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 124
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 125
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 126
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 127
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 128
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 129
    {
        .rx_frequency = 433.25f,
        .parent = 254,
        .has_sensor = false,
    },
    // 130
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 131
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 132
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 133
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 134
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 135
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 136
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 137
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 138
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 139
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 140
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 141
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 142
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 143
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 144
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 145
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 146
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 147
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 148
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 149
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 150
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 151
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 152
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 153
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 154
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 155
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 156
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 157
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 158
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 159
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 160
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 161
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 162
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 163
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 164
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 165
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 166
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 167
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 168
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 169
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 170
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 171
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 172
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 173
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 174
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 175
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 176
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 177
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 178
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 179
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 180
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 181
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 182
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 183
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 184
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 185
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 186
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 187
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 188
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 189
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 190
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 191
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 192
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 193
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 194
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 195
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 196
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 197
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 198
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 199
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 200
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 201
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 202
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 203
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 204
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 205
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 206
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 207
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 208
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 209
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 210
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 211
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 212
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 213
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 214
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 215
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 216
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 217
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 218
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 219
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 220
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 221
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 222
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 223
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 224
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 225
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 226
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 227
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 228
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 229
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 230
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 231
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 232
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 233
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 234
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 235
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 236
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 237
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 238
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 239
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 240
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 241
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 242
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 243
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 244
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 245
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 246
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 247
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 248
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 249
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 250
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 251
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 252
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 253
    {
        .rx_frequency = 400.00f,
        .parent = NO_ID,
        .has_sensor = false,
    },
    // 254
    {
        .rx_frequency = 433.50f,
        .parent = NO_ID,
        .has_sensor = false,
    },
};
