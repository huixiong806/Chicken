#pragma once
const int32_t dx[4] = { -1,1,0,0 };
const int32_t dy[4] = { 0,0,-1,1 };
const unsigned int num_input = 675;
const unsigned int num_output = 225;
const unsigned int num_layers =7;
const unsigned int layer_array[num_layers] = { 675,64,64,64,64,64,225 };
const float desired_error = (const float) 0.001;
const unsigned int max_epochs = 20000;
const unsigned int epochs_between_reports = 1000;
const int train_playout = 3200;