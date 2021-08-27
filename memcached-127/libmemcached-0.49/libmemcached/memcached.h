/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached library
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2006-2009 Brian Aker All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __LIBMEMCACHED_MEMCACHED_H__
#define __LIBMEMCACHED_MEMCACHED_H__

#include <inttypes.h>
#include <stdlib.h>
#include <sys/types.h>


#if !defined(__cplusplus)
# include <stdbool.h>
#endif

#include <libmemcached/visibility.h>
#include <libmemcached/configure.h>
#include <libmemcached/platform.h>
#include <libmemcached/constants.h>
#include <libmemcached/types.h>
#include <libmemcached/string.h>
#include <libmemcached/array.h>
#include <libmemcached/error.h>
#include <libmemcached/stats.h>
#include <libhashkit/hashkit.h>
// Everything above this line must be in the order specified.
#include <libmemcached/allocators.h>
#include <libmemcached/analyze.h>
#include <libmemcached/auto.h>
#include <libmemcached/behavior.h>
#include <libmemcached/callback.h>
#include <libmemcached/delete.h>
#include <libmemcached/dump.h>
#include <libmemcached/fetch.h>
#include <libmemcached/flush.h>
#include <libmemcached/flush_buffers.h>
#include <libmemcached/get.h>
#include <libmemcached/hash.h>
#include <libmemcached/options.h>
#include <libmemcached/parse.h>
#include <libmemcached/quit.h>
#include <libmemcached/result.h>
#include <libmemcached/server.h>
#include <libmemcached/server_list.h>
#include <libmemcached/storage.h>
#include <libmemcached/strerror.h>
#include <libmemcached/verbosity.h>
#include <libmemcached/version.h>
#include <libmemcached/sasl.h>

struct memcached_st {
  /**
    @note these are static and should not change without a call to behavior.
  */
  struct {
    bool is_purging:1;
    bool is_processing_input:1;
    bool is_time_for_rebuild:1;
  } state;

  struct {
    // Everything below here is pretty static.
    bool auto_eject_hosts:1;
    bool binary_protocol:1;
    bool buffer_requests:1;
    bool hash_with_prefix_key:1;
    bool no_block:1; // Don't block
    bool no_reply:1;
    bool randomize_replica_read:1;
    bool support_cas:1;
    bool tcp_nodelay:1;
    bool use_sort_hosts:1;
    bool use_udp:1;
    bool verify_key:1;
    bool tcp_keepalive:1;
  } flags;

  memcached_server_distribution_t distribution;
  hashkit_st hashkit;
  uint32_t number_of_hosts;
  memcached_server_st *servers;
  memcached_server_st *last_disconnected_server;
  int32_t snd_timeout;
  int32_t rcv_timeout;
  uint32_t server_failure_limit;
  uint32_t io_msg_watermark;
  uint32_t io_bytes_watermark;
  uint32_t io_key_prefetch;
  uint32_t tcp_keepidle;
  int32_t poll_timeout;
  int32_t connect_timeout;
  int32_t retry_timeout;
  int send_size;
  int recv_size;
  void *user_data;
  uint64_t query_id;
  uint32_t number_of_replicas;
  hashkit_st distribution_hashkit;
  memcached_result_st result;

  struct {
    bool weighted;
    uint32_t continuum_count; // Ketama
    uint32_t continuum_points_counter; // Ketama
    time_t next_distribution_rebuild; // Ketama
    memcached_continuum_item_st *continuum; // Ketama
  } ketama;

  struct memcached_virtual_bucket_t *virtual_bucket;

  struct _allocators_st {
    memcached_calloc_fn calloc;
    memcached_free_fn free;
    memcached_malloc_fn malloc;
    memcached_realloc_fn realloc;
    void *context;
  } allocators;

  memcached_clone_fn on_clone;
  memcached_cleanup_fn on_cleanup;
  memcached_trigger_key_fn get_key_failure;
  memcached_trigger_delete_key_fn delete_trigger;
  memcached_callback_st *callbacks;
  struct memcached_sasl_st sasl;
  struct memcached_error_t *error_messages;
  struct memcached_array_st *prefix_key;
  struct {
    uint32_t initial_pool_size;
    uint32_t max_pool_size;
    struct memcached_array_st *filename;
  } configure;
  struct {
    bool is_allocated:1;
  } options;

};

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
void memcached_servers_reset(memcached_st *ptr);

LIBMEMCACHED_API
memcached_st *memcached_create(memcached_st *ptr);

LIBMEMCACHED_API
memcached_st *memcached(const char *string, size_t string_length);

LIBMEMCACHED_API
void memcached_free(memcached_st *ptr);

LIBMEMCACHED_API
memcached_return_t memcached_reset(memcached_st *ptr);

LIBMEMCACHED_API
void memcached_reset_last_disconnected_server(memcached_st *ptr);

LIBMEMCACHED_API
memcached_st *memcached_clone(memcached_st *clone, const memcached_st *ptr);

LIBMEMCACHED_API
void *memcached_get_user_data(const memcached_st *ptr);

LIBMEMCACHED_API
void *memcached_set_user_data(memcached_st *ptr, void *data);

LIBMEMCACHED_API
memcached_return_t memcached_push(memcached_st *destination, const memcached_st *source);

LIBMEMCACHED_API
memcached_server_instance_st memcached_server_instance_by_position(const memcached_st *ptr, uint32_t server_key);

LIBMEMCACHED_API
uint32_t memcached_server_count(const memcached_st *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* __LIBMEMCACHED_MEMCACHED_H__ */

