#include "allinea_metric_plugin_api.h"
#include "mpwide_perf.h"
#include <stdbool.h>
#include <assert.h>
#include <inttypes.h>

//> Success exitcode as defined by the ALLINEA Custom Metric Plugin Template
#define SUCCESS 0;
//> Failure exitcode as defined by the ALLINEA Custom Metric Plugin Template
#define FAILURE (-1);

/* Helper functions */
int calculate_ns_per_call(mpwide_perf_counter_t call_count_id, mpwide_perf_counter_t call_duration_id,
                          uint64_t *prev_calls_total, uint64_t *prev_duration_total, 
                          struct timespec *current_sample_time, uint64_t *out_value);
int calculate_s_per_call(mpwide_perf_counter_t call_count_id, mpwide_perf_counter_t call_duration_id,
                          uint64_t *prev_calls_total, uint64_t *prev_duration_total, 
                          struct timespec *current_sample_time, double *out_value);

uint64_t duration_ns(const struct timespec *start, const struct timespec *end);

/**
 * Initialises metric plugin. 
 * It will be called when that plugin library is loaded, it is NOT called from a signal handler.
 * If libmpwide can't be loaded, MAP will fail before this function could be called.
 * @return SUCCESS.
 */
int allinea_plugin_initialize(plugin_id_t plugin_id, void *data) {
    return SUCCESS;
}

/** 
 * Cleans up metric plugin. 
 * It will be called when that plugin library is unloaded, it is NOT called from a signal handler.
 * @return SUCCESS or FAILURE as appropriate
 */
int allinea_plugin_cleanup(plugin_id_t plugin_id, void *data) {
    return SUCCESS;
}



/**
 * Sets out_value to the duration of admin / connection calls
 * @return SUCCESS or FAILURE as appropriate 
 */
int allinea_mpwide_get_admin_duration(metric_id_t id, struct timespec *current_sample_time, double *out_value) {
    static uint64_t prev_admin_calls_total = 0;    // only initialized on the first call to this function
    static uint64_t prev_admin_duration_total = 0; // only initialized on the first call to this function
    return calculate_s_per_call(MPWIDE_PERF_COUNTER_ADMIN_CALLS, MPWIDE_PERF_COUNTER_ADMIN_DURATION,
                                 &prev_admin_calls_total, &prev_admin_duration_total, current_sample_time, out_value);
}

/** 
 * Sets out_value to the data sent (in bytes) since the last sample 
 * @return SUCCESS or FAILURE as appropriate
 */
int allinea_mpwide_get_bytes_sent(metric_id_t id, struct timespec *current_sample_time, uint64_t *out_value) {
    static uint64_t prev_send_size = 0; // initialized only on the first call to this function
    uint64_t curr;
    int is_successful = MPWIDE_Perf_Get_Counter(MPWIDE_PERF_COUNTER_SEND_SIZE, &curr); 
    if (is_successful != 0) {
        return FAILURE;
    }
    uint64_t diff = curr - prev_send_size;
    prev_send_size = curr;
    *out_value = diff;
    return SUCCESS;
}

/** 
 * Sets out_value to the number of send calls since the last sample
 * @return SUCCESS or FAILURE as appropriate
 */
int allinea_mpwide_get_send_calls(metric_id_t id, struct timespec *current_sample_time, uint64_t *out_value) {
    static uint64_t prev_send_calls = 0; // initialized only on the first call to this function
    uint64_t curr;
    int is_successful = MPWIDE_Perf_Get_Counter(MPWIDE_PERF_COUNTER_SEND_CALLS, &curr); 
    if (is_successful != 0) {
        return FAILURE;
    }
    uint64_t diff = curr - prev_send_calls;
    prev_send_calls = curr;
    *out_value = diff;
    return SUCCESS;
}

/**
 * Sets out_value to the duration of send calls
 * @return SUCCESS or FAILURE as appropriate 
 */
int allinea_mpwide_get_send_duration(metric_id_t id, struct timespec *current_sample_time, double *out_value) {
    static uint64_t prev_send_calls_total = 0;    // only initialized on the first call to this function
    static uint64_t prev_send_duration_total = 0; // only initialized on the first call to this function
    return calculate_s_per_call(MPWIDE_PERF_COUNTER_SEND_CALLS, MPWIDE_PERF_COUNTER_SEND_DURATION,
                                 &prev_send_calls_total, &prev_send_duration_total, current_sample_time, out_value);
}


/** 
 * Sets out_value to the data sent (in bytes) since the last sample 
 * @return SUCCESS or FAILURE as appropriate
 */
int allinea_mpwide_get_bytes_sendrecv_sent(metric_id_t id, struct timespec *current_sample_time, uint64_t *out_value) {
    static uint64_t prev_sendrecv_ssize = 0; // initialized only on the first call to this function
    uint64_t curr;
    int is_successful = MPWIDE_Perf_Get_Counter(MPWIDE_PERF_COUNTER_SENDRECV_SEND_SIZE, &curr); 
    if (is_successful != 0) {
        return FAILURE;
    }
    uint64_t diff = curr - prev_sendrecv_ssize;
    prev_sendrecv_ssize = curr;
    *out_value = diff;
    return SUCCESS;
}


/** 
 * Sets out_value to the data sent (in bytes) since the last sample 
 * @return SUCCESS or FAILURE as appropriate
 */
int allinea_mpwide_get_bytes_sendrecv_recv(metric_id_t id, struct timespec *current_sample_time, uint64_t *out_value) {
    static uint64_t prev_sendrecv_rsize = 0; // initialized only on the first call to this function
    uint64_t curr;
    int is_successful = MPWIDE_Perf_Get_Counter(MPWIDE_PERF_COUNTER_SENDRECV_RECV_SIZE, &curr); 
    if (is_successful != 0) {
        return FAILURE;
    }
    uint64_t diff = curr - prev_sendrecv_rsize;
    prev_sendrecv_rsize = curr;
    *out_value = diff;
    return SUCCESS;
}

/** 
 * Sets out_value to the number of sendrecv calls since the last sample
 * @return SUCCESS or FAILURE as appropriate
 */
int allinea_mpwide_get_sendrecv_calls(metric_id_t id, struct timespec *current_sample_time, uint64_t *out_value) {
    static uint64_t prev_sendrecv_calls = 0; // initialized only on the first call to this function
    uint64_t curr;
    int is_successful = MPWIDE_Perf_Get_Counter(MPWIDE_PERF_COUNTER_SENDRECV_CALLS, &curr); 
    if (is_successful != 0) {
        return FAILURE;
    }
    uint64_t diff = curr - prev_sendrecv_calls;
    prev_sendrecv_calls = curr;
    *out_value = diff;
    return SUCCESS;
}

/**
 * Sets out_value to the duration of sendrecv calls
 * @return SUCCESS or FAILURE as appropriate 
 */
int allinea_mpwide_get_sendrecv_duration(metric_id_t id, struct timespec *current_sample_time, double *out_value) {
    static uint64_t prev_sendrecv_calls_total = 0;    // only initialized on the first call to this function
    static uint64_t prev_sendrecv_duration_total = 0; // only initialized on the first call to this function
    return calculate_s_per_call(MPWIDE_PERF_COUNTER_SENDRECV_CALLS, MPWIDE_PERF_COUNTER_SENDRECV_DURATION,
                                 &prev_sendrecv_calls_total, &prev_sendrecv_duration_total, current_sample_time, out_value);
}


/**
 * Sets out_value to the data received (in bytes) since the last sample 
 * @return SUCCESS or FAILURE as appropriate 
 */
int allinea_mpwide_get_bytes_received(metric_id_t id, struct timespec *current_sample_time, uint64_t *out_value) {
    static uint64_t prev_receive_size = 0; // initialized only on the first call to this function
    uint64_t curr;
    int is_successful = MPWIDE_Perf_Get_Counter(MPWIDE_PERF_COUNTER_RECEIVE_SIZE, &curr); 
    if (is_successful != 0) {
        return FAILURE;
    }
    uint64_t diff = curr - prev_receive_size;
    prev_receive_size = curr;
    *out_value = diff;
    return SUCCESS;
}

/**
 * Sets out_value to the number of receive calls since the last sample
 * @return SUCCESS or FAILURE as appropriate 
 */
int allinea_mpwide_get_receive_calls(metric_id_t id, struct timespec *current_sample_time, uint64_t *out_value) {
    static uint64_t prev_receive_calls = 0; // initialized only on the first call to this function
    uint64_t curr;
    int is_successful = MPWIDE_Perf_Get_Counter(MPWIDE_PERF_COUNTER_RECEIVE_CALLS, &curr); 
    if (is_successful != 0) {
        return FAILURE;
    }
    uint64_t diff = curr - prev_receive_calls;
    prev_receive_calls = curr;
    *out_value = diff;
    return SUCCESS;
}

/**
 * Sets out_value to the duration of send calls
 * @return SUCCESS or FAILURE as appropriate 
 */
int allinea_mpwide_get_receive_duration(metric_id_t id, struct timespec *current_sample_time, double *out_value) {
    static uint64_t prev_receive_calls_total = 0;    // only initialized on the first call to this function
    static uint64_t prev_receive_duration_total = 0; // only initialized on the first call to this function
    return calculate_s_per_call(MPWIDE_PERF_COUNTER_RECEIVE_CALLS, MPWIDE_PERF_COUNTER_RECEIVE_DURATION,
                                 &prev_receive_calls_total, &prev_receive_duration_total, current_sample_time, out_value);
}

/**
 * Sets out_value to the number of barrier calls since the last sample
 * @return SUCCESS or FAILURE as appropriate 
 */
int allinea_mpwide_get_barrier_calls(metric_id_t id, struct timespec *current_sample_time, uint64_t *out_value) {
    static uint64_t prev_barrier_calls = 0; // initialized only on the first call to this function
    uint64_t curr;
    int is_successful = MPWIDE_Perf_Get_Counter(MPWIDE_PERF_COUNTER_BARRIER_CALLS, &curr); 
    if (is_successful != 0) {
        return FAILURE;
    }
    uint64_t diff = curr - prev_barrier_calls;
    prev_barrier_calls = curr;
    *out_value = diff;
    return SUCCESS;
}

/** 
 * Sets out_value to the duration of ns/call 
 * @return SUCCESS or FAILURE as appropriate 
 */
int allinea_mpwide_get_barrier_duration(metric_id_t id, struct timespec *current_sample_time, double *out_value) {
    static uint64_t prev_barrier_calls_total = 0;    // only initialized on the first call to this function
    static uint64_t prev_barrier_duration_total = 0; // only initialized on the first call to this function
    return calculate_s_per_call(MPWIDE_PERF_COUNTER_BARRIER_CALLS, MPWIDE_PERF_COUNTER_BARRIER_DURATION,
                                 &prev_barrier_calls_total, &prev_barrier_duration_total, current_sample_time, out_value);
}

/** 
 * Helper function to calculate ns/call for some `call_count_id` and matching `call_duration_id` 
 * Uses two variables, to store previous call_totals and duration_totals. They should be *static* variables
 * initialised in the *caller* function. (They need to retain their values in between calls to the *caller*).
 *
 * If MPWIDE2 is inside a send/barrier/metric call, gets the timespec for the start time of that call, calculates the
 * difference from the current time (current_sample_time) and increases the return value accordingly. This is needed 
 * because MAP might interrupt a MPWIDE2 call once it started, and would only report the duration once the call 
 * has finished. This would result in a single spike on the graph, while the user would expect a linearly growing 
 * value for the duration of the call.
 *
 * @param [in] call_count_id            counter of number of calls of the given metric 
 * @param [in] call_duration_id         counter of the total duration of calls
 * @param [inout] prev_calls_total      previous value of counter denoted by call_count_id
 * @param [inout] prev_duration_total   previous value of counter denoted by call_duration_id
 * @param [in] current_sample_time      ALLINEA Custom Metric API argument (timestamp)
 * @param [out] out_value               ALLINEA Custom Metric API argument (output)
 * @returns SUCCESS or FAILURE as appropriate
 */
int calculate_ns_per_call(mpwide_perf_counter_t call_count_id, mpwide_perf_counter_t call_duration_id,
                          uint64_t *prev_calls_total, uint64_t *prev_duration_total, 
                          struct timespec *current_sample_time, uint64_t *out_value) {
    // if we're inside call, return time spent since the start of the call
    // (see top comment for the calculate_ns_per_call function)
    struct timespec start_time;
    mpwide_perf_counter_t curr_call_id;
    bool is_inside_call = MPWIDE_Perf_In_Call(&start_time, &curr_call_id);
    if (is_inside_call && curr_call_id == call_duration_id) {
        uint64_t duration_in_window = duration_ns(&start_time, current_sample_time);
        *out_value = duration_in_window;
        return SUCCESS;
    }

    uint64_t curr_calls_total;
    int is_successful = MPWIDE_Perf_Get_Counter(call_count_id, &curr_calls_total); 
    if (is_successful != 0) {
        return FAILURE;
    }

    // duration within this sampling window
    uint64_t curr_duration_total;
    is_successful = MPWIDE_Perf_Get_Counter(call_duration_id, &curr_duration_total);
    if (is_successful != 0) {
        return FAILURE;
    }
    uint64_t duration_in_window = curr_duration_total - *prev_duration_total;
    *prev_duration_total = curr_duration_total;

    uint64_t calls_in_window = curr_calls_total - *prev_calls_total;
    *prev_calls_total = curr_calls_total;
    if (calls_in_window == 0) {
        *out_value = 0;
        return SUCCESS;
    }

    // calculate and set average duration
    uint64_t ns_per_calls = duration_in_window / calls_in_window;
    *out_value = ns_per_calls;
    return SUCCESS;
}

/** 
 * Helper function to calculate ns/call for some `call_count_id` and matching `call_duration_id` 
 * Uses two variables, to store previous call_totals and duration_totals. They should be *static* variables
 * initialised in the *caller* function. (They need to retain their values in between calls to the *caller*).
 *
 * If MPWIDE2 is inside a send/barrier/metric call, gets the timespec for the start time of that call, calculates the
 * difference from the current time (current_sample_time) and increases the return value accordingly. This is needed 
 * because MAP might interrupt a MPWIDE2 call once it started, and would only report the duration once the call 
 * has finished. This would result in a single spike on the graph, while the user would expect a linearly growing 
 * value for the duration of the call.
 *
 * @param [in] call_count_id            counter of number of calls of the given metric 
 * @param [in] call_duration_id         counter of the total duration of calls
 * @param [inout] prev_calls_total      previous value of counter denoted by call_count_id
 * @param [inout] prev_duration_total   previous value of counter denoted by call_duration_id
 * @param [in] current_sample_time      ALLINEA Custom Metric API argument (timestamp)
 * @param [out] out_value               ALLINEA Custom Metric API argument (output)
 * @returns SUCCESS or FAILURE as appropriate
 */
int calculate_s_per_call(mpwide_perf_counter_t call_count_id, mpwide_perf_counter_t call_duration_id,
                          uint64_t *prev_calls_total, uint64_t *prev_duration_total, 
                          struct timespec *current_sample_time, double *out_value) {
    // if we're inside call, return time spent since the start of the call
    // (see top comment for the calculate_ns_per_call function)
    struct timespec start_time;
    mpwide_perf_counter_t curr_call_id;
    bool is_inside_call = MPWIDE_Perf_In_Call(&start_time, &curr_call_id);
    if (is_inside_call && curr_call_id == call_duration_id) {
        double duration_in_window = (duration_ns(&start_time, current_sample_time))/1000000000.0;
        *out_value = duration_in_window;
        return SUCCESS;
    }

    uint64_t curr_calls_total;
    int is_successful = MPWIDE_Perf_Get_Counter(call_count_id, &curr_calls_total); 
    if (is_successful != 0) {
        return FAILURE;
    }

    // duration within this sampling window
    uint64_t curr_duration_total;
    is_successful = MPWIDE_Perf_Get_Counter(call_duration_id, &curr_duration_total);
    if (is_successful != 0) {
        return FAILURE;
    }
    uint64_t duration_in_window = curr_duration_total - *prev_duration_total;
    *prev_duration_total = curr_duration_total;

    uint64_t calls_in_window = curr_calls_total - *prev_calls_total;
    *prev_calls_total = curr_calls_total;
    if (calls_in_window == 0) {
        *out_value = 0;
        return SUCCESS;
    }

    // calculate and set average duration
    double s_per_calls = (duration_in_window / calls_in_window)/1000000000.0;
    *out_value = s_per_calls;
    return SUCCESS;

}

/** 
 * Helper function to get the time difference between two <time.h> timespec structs in nanoseconds 
 * @param [in] start    timestamp to measure from
 * @param [in] end      timestamp to measure to
 * @returns the time difference from start to end in nanoseconds
 */
uint64_t duration_ns(const struct timespec *start, const struct timespec *end) {
    const uint64_t SEC_TO_NS = 1000000000;
    uint64_t start_nanosec   = start->tv_sec * SEC_TO_NS + start->tv_nsec;
    uint64_t end_nanosec     = end->tv_sec * SEC_TO_NS + end->tv_nsec;
    assert(start <= end);
    return (end_nanosec - start_nanosec);
}
