#pragma once

#include <cstdlib>
#include <cinttypes>

enum class ad_input_type : uint8_t
{
    digital,
    analog
};

struct ad_input_sample
{
    float time;
    float value;
};

struct ad_input_record_chunk
{
    size_t capacity; // Number of samples we can hold
    size_t size; // Number of samples currently buffered
    ad_input_sample* data; // Array of samples, allocated up to capacity if non-null
    struct ad_input_record_chunk* next;

    ad_input_record_chunk(size_t in_capacity);
    ~ad_input_record_chunk();

    bool init();
};

struct ad_input_recorder
{
    size_t chunk_size;
    size_t num_initial_chunks;
    ad_input_record_chunk* first;
    ad_input_record_chunk* write_head;

    float last_value_recorded;
    float last_time_recorded;
    float last_time_seen;

    ad_input_recorder(size_t in_chunk_size, size_t in_num_initial_chunks);
    ~ad_input_recorder();

    bool init();
    bool handle_sample(float time, float value);
    bool write(float time, float value);
};
