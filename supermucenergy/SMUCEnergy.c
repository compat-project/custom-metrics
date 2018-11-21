//! \file
/*! This example metric plugin provides a custom metric showing the number of
 *  interrupts handled by the system.
 *
 *  The information is obtained from /proc/interrupts.
 *  
 *  This file is paired with an XML file (custom1.xml) which informs the Arm HPC
 *  tools as to how this custom metric is loaded and displayed.
 */

/* The following functions are assumed to be async-signal-safe, although not
 * required by POSIX:
 * 
 * strchr strstr strtoull
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "allinea_metric_plugin_api.h"

#define NODE_ENERGY "/sys/devices/platform/aem.0/energy2_input"

//! Error code if /proc/interrupts does not exist.
#define ERROR_NO_NODE_ENERGY                        1000
#define ERROR_EMPTY_NODE_ENERGY                     1001
#define ERROR_ZERO_NODE_ENERGY                      1001

#define BUFSIZE     256
#define OVERLAP    64


// 0.5s in ns
#define time_diff 500000000

#ifndef min
#define min(x, y) ( ((x) < (y)) ? (x) : (y) )
#endif

//! initial value.
static uint64_t initial = 0;
//! previous value.
static uint64_t previous = 0;
//! current value.
static uint64_t current = 0;
//! current value in W
static double current_w = 0;
//! Timestamp of last sample
static uint64_t last_sample;


uint64_t ts_to_ns(struct timespec *sample_time){
   uint64_t ts = (uint64_t)((sample_time->tv_sec*1000000000)+sample_time->tv_nsec);

   return ts;
}

int new_sample(struct timespec *sample_time){
 
  uint64_t ts = ts_to_ns(sample_time);
  if(last_sample > ts) return -1;

  uint64_t diff = ts - last_sample;



  if(diff >= time_diff){
    last_sample = ts;
    return diff;
  }
  return -1;

}

//! This function is called when the metric plugin is loaded.
/*!
 *  We do not have to restrict ourselves to async-signal-safe functions because
 *  the initialization function will be called without any locks held.
 * 
 *  \param plugin_id an opaque handle for the plugin.
 *  \param unused unused
 *  \return 0 on success; -1 on failure and set errno
 */
int allinea_plugin_initialize(plugin_id_t plugin_id, void *unused)
{
    // Check that /proc/interrupts exists.
    if (access(NODE_ENERGY, F_OK) != 0) {
        if (errno == ENOENT)
            allinea_set_plugin_error_messagef(plugin_id, ERROR_NO_NODE_ENERGY,
                "Not supported (no /sys/devices/platform/aem.0/energy2_input)");
        else
            allinea_set_plugin_error_messagef(plugin_id, errno,
                "Error accessing %s: %s", NODE_ENERGY, strerror(errno));
        return -1;
    }

    // Record timestamp of initialisation
    struct timespec init_t = allinea_get_current_time();
    last_sample = ts_to_ns(&init_t);

char buf[BUFSIZE];


    // We must use the allinea_safe variants of open / read / write / close so
    // that we are not included in the Allinea sampler's I/O accounting.
    const int fd = allinea_safe_open(NODE_ENERGY, O_RDONLY);
    if (fd == -1) {
        allinea_set_plugin_error_messagef(plugin_id, errno, 
            "Error opening %s: %d", NODE_ENERGY, strerror(errno));
        return -1;
    }


    const ssize_t bytes_read = allinea_safe_read_line(fd, buf, BUFSIZE);

    allinea_safe_close(fd);

    if (bytes_read == -1) {
        // read failed
        allinea_set_plugin_error_messagef(plugin_id, errno,
            "Error opening %s: %d", NODE_ENERGY, strerror(errno));
        return -1;
    }
    if (bytes_read == 0) {
        // end of file
        allinea_set_plugin_error_messagef(plugin_id, ERROR_EMPTY_NODE_ENERGY,
            "Empty File %s: %d", NODE_ENERGY, bytes_read);
        return -1;
    }

    current = strtoull(buf, NULL, 10);
    if(current == 0){
        allinea_set_plugin_error_messagef(plugin_id, ERROR_ZERO_NODE_ENERGY,
            "Zero node energy %s", NODE_ENERGY);
        return -1;
    }

    // Convert to J
    current = current / 1000000;
    initial = current;
    previous = current;
    
    return 0;

}

//! This function is called when the metric plugin is unloaded.
/*!
 *  We do not have to restrict ourselves to async-signal-safe functions because
 *  the initialization function will be called without any locks held.
 * 
 *  \param plugin_id an opaque handle for the plugin.
 *  \param unused unused
 *  \return 0 on success; -1 on failure and set errno
 */
int allinea_plugin_cleanup(plugin_id_t plugin_id, void *unused)
{
}

//! This callback is called once every sample.
/*!
 *  The current value of the metric (the number of interrupts taken since the
 *  start of the run) is copied to out_value. This value is modified by the
 *  Arm HPC tools depending on the value of the divideBySampleTime XML switch
 *  in \c custom1.xml. If the switch is set to true then out_value is divided
 *  by the sample interval, else it is left untouched.
 * 
 *  This function is called from a signal handler so it must be async-signal-safe.
 * 
 *  \param metric_id an opaque handle for the metric.
 *  \param in_out_sample_time [in, out] the time at which this sample is being taken
 *         We set this to the current time just before opening /proc/stat to get
 *         the most accurate sample time.
 *  \param out_value [out] the value of the metric
 *
 *  \return 0 on success; -1 on failure and set errno
 */
int sample_IBM_energy(metric_id_t metric_id, struct timespec *in_out_sample_time, double *out_value)
{

    uint64_t diff = new_sample(in_out_sample_time);
    if(diff == -1){
       *out_value = current_w;
       return 0;
    }

    char buf[BUFSIZE];


    // We must use the allinea_safe variants of open / read / write / close so
    // that we are not included in the Allinea sampler's I/O accounting.
    const int fd = allinea_safe_open(NODE_ENERGY, O_RDONLY);
    if (fd == -1) {
        allinea_set_metric_error_messagef(metric_id, errno, 
            "Error opening %s: %d", NODE_ENERGY, strerror(errno));
        return -1;
    }


    const ssize_t bytes_read = allinea_safe_read_line(fd, buf, BUFSIZE);

    allinea_safe_close(fd);

    if (bytes_read == -1) {
        // read failed
        allinea_set_metric_error_messagef(metric_id, errno,
            "Error opening %s: %d", NODE_ENERGY, strerror(errno));
        return -1;
    }
    if (bytes_read == 0) {
        // end of file
        allinea_set_metric_error_messagef(metric_id, ERROR_EMPTY_NODE_ENERGY,
            "Empty File %s: %d", NODE_ENERGY, bytes_read);
        return -1;
    }

    // Swap the old values around
    previous = current;

    current = strtoull(buf, NULL, 10);

    if(current == 0){
        allinea_set_metric_error_messagef(metric_id, ERROR_ZERO_NODE_ENERGY,
            "Zero node energy %s", NODE_ENERGY);
        return -1;
    }

    // Convert to J
    current = current / 1000000;

    // Convert to W
    current_w = (double)((current - previous)/((double)diff/1000000000));


    *out_value = current_w;
    
    return 0;
}

int sample_IBM_energy_tot(metric_id_t metric_id, struct timespec *in_out_sample_time, uint64_t *out_value)
{
    // Very simple metric, just reurn the last 'previous' value - may be 1 sample old
    *out_value = current - initial;
    return 0;
}
