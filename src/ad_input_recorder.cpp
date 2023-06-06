#include "ad_input_recorder.h"

#include <cassert>
#include <cstdlib>
#include <cstdio>

ad_input_record_chunk::ad_input_record_chunk(size_t in_capacity)
    : capacity(in_capacity)
    , size(0)
    , data(nullptr)
    , next(nullptr)
{
}

ad_input_record_chunk::~ad_input_record_chunk()
{
    free(data);
}

bool ad_input_record_chunk::init()
{
    data = reinterpret_cast<ad_input_sample*>(malloc(capacity * sizeof(ad_input_sample)));
    return data != nullptr;
}

ad_input_recorder::ad_input_recorder(size_t in_chunk_size, size_t in_num_initial_chunks)
    : chunk_size(in_chunk_size)
    , num_initial_chunks(in_num_initial_chunks)
    , first(nullptr)
    , write_head(nullptr)
    , last_value_recorded(0.0f)
    , last_time_recorded(-1.0f)
    , last_time_seen(-1.0f)
{
}

ad_input_recorder::~ad_input_recorder()
{
    ad_input_record_chunk* chunk = first;
    while (chunk)
    {
        ad_input_record_chunk* next = chunk->next;
        delete chunk;
        chunk = next;
    }
}

bool ad_input_recorder::init()
{
    // We should be properly constructed and not yet initialized
    assert(chunk_size > 0);
    assert(num_initial_chunks > 0);
    assert(!first);
    assert(!write_head);

    // Allocate our initial chunk to contain the start of our recorded data
    first = new ad_input_record_chunk(chunk_size);
    if (!first || !first->init())
    {
        return false;
    }
    write_head = first;

    // Allocate each successive chunk that we want to start out with
    ad_input_record_chunk* prev = first;
    for (size_t i = 1; i < num_initial_chunks; i++)
    {
        // Halt on any allocation failures
        ad_input_record_chunk* chunk = new ad_input_record_chunk(chunk_size);
        if (!chunk || !chunk->init())
        {
            return false;
        }

        // Link this chunk to the previous one before continuing
        prev->next = chunk;
        prev = chunk;
    }
    return true;
}

bool ad_input_recorder::handle_sample(float time, float value)
{
    // Sample times are relative to the start of recording and should always increase
    assert(time >= 0.0f);
    assert(last_time_seen == -1.0f || time > last_time_seen);

    // If this new sample maintains the same value as the last sample we recorded,
    // don't record a new sample
    const bool value_is_unchanged = last_time_seen >= 0.0f && last_value_recorded == value;
    if (value_is_unchanged)
    {
        // We've handled the sample simply by recording its time as last_time_seen,
        // with a value distinct from last_time_recorded: we now know that the value at
        // this time is the same as last_value_recorded, and we don't have to modify
        // our actual buffered data at all
        last_time_seen = time;
        return true;
    }

    // Otherwise, we need to record a new sample into the current write chunk - if we
    // skipped recording any prior samples because their value was unchanged from the
    // last sample we recorded, we need to insert another sample to ensure that our
    // value will remain constant during that time span with linear interpolation
    if (last_time_seen > last_time_recorded)
    {
        if (!write(last_time_seen, last_value_recorded))
        {
            return false;
        }
    }

    // Write the sample we're currently handling into our current write chunk
    if (!write(time, value))
    {
        return false;
    }
    last_time_seen = time;
    last_time_recorded = time;
    last_value_recorded = value;
    return true;
}

bool ad_input_recorder::write(float time, float value)
{
    // We should have a valid chunk to write to, with space available
    assert(write_head);
    assert(write_head->data);
    assert(write_head->size < write_head->capacity);

    // Write our new sample into the current write chunk
    const size_t write_index = write_head->size;
    write_head->data[write_index].time = time;
    write_head->data[write_index].value = value;
    write_head->size++;
    last_time_recorded = time;
    last_value_recorded = value;

    // If we've now filled that chunk, advance our write head to the next chunk
    if (write_head->size == write_head->capacity)
    {
        // If this was the last chunk we had allocated, allocate a new one
        if (!write_head->next)
        {
            write_head->next = new ad_input_record_chunk(chunk_size);
            if (!write_head->next || !write_head->next->init())
            {
                // Allocation failed: this is the only error case
                return false;
            }
        }

        // Subsequent writes should target the next chunk in line
        assert(write_head->next);
        assert(write_head->next->size == 0);
        write_head = write_head->next;
    }
    return true;
}
