#pragma once

#include "testing.h"
#include "ad_input_recorder.h"

const char* test_input_recorder_init()
{
    ad_input_recorder recorder(8, 2);
    t_assert(recorder.chunk_size == 8);
    t_assert(recorder.num_initial_chunks == 2);
    t_assert(recorder.first == nullptr);
    t_assert(recorder.write_head == nullptr);

    const bool init_ok = recorder.init();
    t_assert(init_ok);
    t_assert(recorder.first != nullptr);
    t_assert(recorder.write_head == recorder.first);

    t_assert(recorder.first->capacity == 8);
    t_assert(recorder.first->size == 0);
    t_assert(recorder.first->data != nullptr);
    t_assert(recorder.first->next != nullptr);

    const ad_input_record_chunk* second = recorder.first->next;
    t_assert(second->capacity == 8);
    t_assert(second->size == 0);
    t_assert(second->data != nullptr);
    t_assert(second->next == nullptr);

	return nullptr;
}

const char* test_input_recorder_chunks()
{
    // 4 samples per chunk, with 2 chunks preallocated
    ad_input_recorder recorder(4, 2);
    const bool init_ok = recorder.init();
    t_assert(init_ok);

    // Handle three samples: they should all be written to the first chunk, and our
    // write head should remain at that chunk
    bool ok;
    ok = recorder.handle_sample(0.0f, 1.0f); t_assert(ok);
    ok = recorder.handle_sample(0.1f, 2.0f); t_assert(ok);
    ok = recorder.handle_sample(0.2f, 3.0f); t_assert(ok);
    t_assert(recorder.first->size == 3);
    t_assert(recorder.first->data[2].time == 0.2f);
    t_assert(recorder.first->data[2].value == 3.0f);
    t_assert(recorder.write_head == recorder.first);

    // Handle one more sample: it should be written to the first chunk, and the write
    // head should advance to the next chunk now that we've hit our 4-sample capacity
    ok = recorder.handle_sample(0.3f, 4.0f); t_assert(ok);
    const ad_input_record_chunk* second = recorder.first->next;
    t_assert(recorder.first->size == 4);
    t_assert(recorder.first->data[3].time == 0.3f);
    t_assert(recorder.first->data[3].value == 4.0f);
    t_assert(recorder.write_head == second);

    // Write four more samples to fill up our second chunk and allocate a third
    t_assert(second->size == 0);
    t_assert(second->next == nullptr);
    ok = recorder.handle_sample(0.4f, 5.0f); t_assert(ok);
    ok = recorder.handle_sample(0.5f, 6.0f); t_assert(ok);
    ok = recorder.handle_sample(0.6f, 7.0f); t_assert(ok);
    ok = recorder.handle_sample(0.7f, 8.0f); t_assert(ok);
    t_assert(second->size == 4);
    t_assert(second->data[3].time == 0.7f);
    t_assert(second->data[3].value == 8.0f);
    t_assert(second->next != nullptr);
    t_assert(recorder.write_head == second->next);

    return nullptr;
}

const char* test_input_recorder_constant_value()
{
    // 5 samples per chunk, with 1 chunk preallocated
    ad_input_recorder recorder(5, 1);
    const bool init_ok = recorder.init();
    t_assert(init_ok);

    // Record a unique value at time 100, then handle a succession of samples with
    // increasing times but identical values: this should only record 2 samples
    bool ok;
    ok = recorder.handle_sample(100.0f, 42.0f); t_assert(ok);
    ok = recorder.handle_sample(101.0f, 45.0f); t_assert(ok);
    ok = recorder.handle_sample(102.0f, 45.0f); t_assert(ok);
    ok = recorder.handle_sample(103.0f, 45.0f); t_assert(ok);
    ok = recorder.handle_sample(104.0f, 45.0f); t_assert(ok);
    ok = recorder.handle_sample(106.0f, 45.0f); t_assert(ok);
    ok = recorder.handle_sample(108.0f, 45.0f); t_assert(ok);
    ok = recorder.handle_sample(110.0f, 45.0f); t_assert(ok);
    t_assert(recorder.first->size == 2);
    t_assert(recorder.last_value_recorded == 45.0f);
    t_assert(recorder.last_time_seen == 110.0f);
    t_assert(recorder.last_time_recorded == 101.f);

    // Record another, different value: this should record a second key to end our hold
    // on a value of 45.0f, followed by our new sample
    ok = recorder.handle_sample(111.0f, 47.0f); t_assert(ok);
    t_assert(recorder.first->size == 4);
    t_assert(recorder.first->data[1].time < recorder.first->data[2].time);
    t_assert(recorder.first->data[1].value == recorder.first->data[2].value);
    t_assert(recorder.last_value_recorded == 47.0f);
    t_assert(recorder.last_time_seen == 111.0f);
    t_assert(recorder.last_time_recorded == 111.f);

    // Hold on that value: we should stay at 4 out of of 5 samples buffered in our
    // current chunk, without recording anything or allocating a new chunk
    ok = recorder.handle_sample(120.0f, 47.0f); t_assert(ok);
    ok = recorder.handle_sample(130.0f, 47.0f); t_assert(ok);
    ok = recorder.handle_sample(140.0f, 47.0f); t_assert(ok);
    t_assert(recorder.first->size == 4);
    t_assert(recorder.first->next == nullptr);
    t_assert(recorder.last_value_recorded == 47.0f);
    t_assert(recorder.last_time_seen == 140.0f);
    t_assert(recorder.last_time_recorded == 111.f);

    // Change the value again: this inserts 2 samples again, so we should end up with
    // our hold key as the last sample in our initial chunk, with a newly-allocated
    // second chunk containing the sample at our new time
    ok = recorder.handle_sample(141.0f, 50.0f); t_assert(ok);
    t_assert(recorder.first->size == 5);
    t_assert(recorder.first->data[4].time == 140.0f);
    t_assert(recorder.first->data[4].value == 47.0f);
    t_assert(recorder.first->next != nullptr);
    t_assert(recorder.last_value_recorded == 50.0f);
    t_assert(recorder.last_time_seen == 141.0f);
    t_assert(recorder.last_time_recorded == 141.0f);
    const ad_input_record_chunk* second = recorder.first->next;
    t_assert(second->size == 1);
    t_assert(second->data[0].time == 141.0f);
    t_assert(second->data[0].value == 50.0f);

    return nullptr;
}
