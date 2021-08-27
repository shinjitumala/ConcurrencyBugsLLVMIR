/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached Client and Server 
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  All rights reserved.
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

#include <config.h>

#include <vector>
#include <iostream>
#include <string>
#include <errno.h>

#define BUILDING_LIBMEMCACHED
#include <libmemcached/memcached.h>

#include "tests/parser.h"
#include "tests/print.h"

enum scanner_type_t
{
  NIL,
  UNSIGNED,
  SIGNED,
  ARRAY
};


struct scanner_string_st {
  const char *c_str;
  size_t size;
};

static inline scanner_string_st scanner_string(const char *arg, size_t arg_size)
{
  scanner_string_st local= { arg, arg_size };
  return local;
}

#define make_scanner_string(X) scanner_string((X), static_cast<size_t>(sizeof(X) - 1))

static struct scanner_string_st scanner_string_null= { 0, 0};

struct scanner_variable_t {
  enum scanner_type_t type;
  struct scanner_string_st option;
  struct scanner_string_st result;
  test_return_t (*check_func)(memcached_st *memc, const scanner_string_st &hostname);
};

// Check and make sure the first host is what we expect it to be
static test_return_t __check_host(memcached_st *memc, const scanner_string_st &hostname)
{
  memcached_server_instance_st instance=
    memcached_server_instance_by_position(memc, 0);

  test_true(instance);

  const char *first_hostname = memcached_server_name(instance);
  test_true(first_hostname);
  test_strcmp(first_hostname, hostname.c_str);

  return TEST_SUCCESS;
}

// Check and make sure the prefix_key is what we expect it to be
static test_return_t __check_prefix_key(memcached_st *memc, const scanner_string_st &hostname)
{
  memcached_server_instance_st instance=
    memcached_server_instance_by_position(memc, 0);

  test_true(instance);

  const char *first_hostname = memcached_server_name(instance);
  test_true(first_hostname);
  test_strcmp(first_hostname, hostname.c_str);

  return TEST_SUCCESS;
}

static test_return_t __check_IO_MSG_WATERMARK(memcached_st *memc, const scanner_string_st &value)
{
  uint64_t value_number;

  value_number= atoll(value.c_str);

  test_true(memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK) == value_number);
  return TEST_SUCCESS;
}

static test_return_t __check_REMOVE_FAILED_SERVERS(memcached_st *memc, const scanner_string_st &)
{
  test_true(memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS));
  return TEST_SUCCESS;
}

static test_return_t __check_NOREPLY(memcached_st *memc, const scanner_string_st &)
{
  test_true(memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_NOREPLY));
  return TEST_SUCCESS;
}

static test_return_t __check_VERIFY_KEY(memcached_st *memc, const scanner_string_st &)
{
  test_true(memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_VERIFY_KEY));
  return TEST_SUCCESS;
}

static test_return_t __check_distribution_RANDOM(memcached_st *memc, const scanner_string_st &)
{
  test_true(memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_DISTRIBUTION) == MEMCACHED_DISTRIBUTION_RANDOM);
  return TEST_SUCCESS;
}

scanner_variable_t test_server_strings[]= {
  { ARRAY, make_scanner_string("--server=localhost"), make_scanner_string("localhost"), __check_host },
  { ARRAY, make_scanner_string("--server=10.0.2.1"), make_scanner_string("10.0.2.1"), __check_host },
  { ARRAY, make_scanner_string("--server=example.com"), make_scanner_string("example.com"), __check_host },
  { ARRAY, make_scanner_string("--server=localhost:30"), make_scanner_string("localhost"), __check_host },
  { ARRAY, make_scanner_string("--server=10.0.2.1:20"), make_scanner_string("10.0.2.1"), __check_host },
  { ARRAY, make_scanner_string("--server=example.com:1024"), make_scanner_string("example.com"), __check_host },
  { NIL, scanner_string_null, scanner_string_null, NULL }
};

scanner_variable_t test_server_strings_with_weights[]= {
  { ARRAY, make_scanner_string("--server=10.0.2.1:30/?40"), make_scanner_string("10.0.2.1"), __check_host },
  { ARRAY, make_scanner_string("--server=example.com:1024/?30"), make_scanner_string("example.com"), __check_host },
  { ARRAY, make_scanner_string("--server=10.0.2.1/?20"), make_scanner_string("10.0.2.1"), __check_host },
  { ARRAY, make_scanner_string("--server=example.com/?10"), make_scanner_string("example.com"), __check_host },
  { NIL, scanner_string_null, scanner_string_null, NULL }
};

scanner_variable_t bad_test_strings[]= {
  { ARRAY, make_scanner_string("-servers=localhost:11221,localhost:11222,localhost:11223,localhost:11224,localhost:11225"), scanner_string_null, NULL },
  { ARRAY, make_scanner_string("-- servers=a.example.com:81,localhost:82,b.example.com"), scanner_string_null, NULL },
  { ARRAY, make_scanner_string("--servers=localhost:+80"), scanner_string_null, NULL},
  { ARRAY, make_scanner_string("--servers=localhost.com."), scanner_string_null, NULL},
  { ARRAY, make_scanner_string("--server=localhost.com."), scanner_string_null, NULL},
  { ARRAY, make_scanner_string("--server=localhost.com.:80"), scanner_string_null, NULL},
  { NIL, scanner_string_null, scanner_string_null, NULL}
};

scanner_variable_t test_number_options[]= {
  { ARRAY,  make_scanner_string("--CONNECT-TIMEOUT=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--IO-BYTES-WATERMARK=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--IO-KEY-PREFETCH=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--IO-MSG-WATERMARK=456"), make_scanner_string("456"), __check_IO_MSG_WATERMARK },
  { ARRAY,  make_scanner_string("--NUMBER-OF-REPLICAS=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--POLL-TIMEOUT=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--RCV-TIMEOUT=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--REMOVE-FAILED-SERVERS=3"), scanner_string_null, __check_REMOVE_FAILED_SERVERS },
  { ARRAY,  make_scanner_string("--RETRY-TIMEOUT=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--SND-TIMEOUT=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--SOCKET-RECV-SIZE=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--SOCKET-SEND-SIZE=456"), scanner_string_null, NULL },
  { NIL, scanner_string_null, scanner_string_null, NULL}
};

scanner_variable_t test_boolean_options[]= {
  { ARRAY,  make_scanner_string("--BINARY-PROTOCOL"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--BUFFER-REQUESTS"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH-WITH-NAMESPACE"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--NOREPLY"), scanner_string_null, __check_NOREPLY },
  { ARRAY,  make_scanner_string("--RANDOMIZE-REPLICA-READ"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--SORT-HOSTS"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--SUPPORT-CAS"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--TCP-NODELAY"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--TCP-KEEPALIVE"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--TCP-KEEPIDLE"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--USE-UDP"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--VERIFY-KEY"), scanner_string_null, __check_VERIFY_KEY },
  { NIL, scanner_string_null, scanner_string_null, NULL}
};

scanner_variable_t prefix_key_strings[]= {
  { ARRAY, make_scanner_string("--NAMESPACE=foo"), make_scanner_string("foo"), __check_prefix_key },
  { ARRAY, make_scanner_string("--NAMESPACE=\"foo\""), make_scanner_string("foo"), __check_prefix_key },
  { ARRAY, make_scanner_string("--NAMESPACE=\"This_is_a_very_long_key\""), make_scanner_string("This_is_a_very_long_key"), __check_prefix_key },
  { NIL, scanner_string_null, scanner_string_null, NULL}
};

scanner_variable_t distribution_strings[]= {
  { ARRAY,  make_scanner_string("--DISTRIBUTION=consistent"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--DISTRIBUTION=consistent,CRC"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--DISTRIBUTION=consistent,MD5"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--DISTRIBUTION=random"), scanner_string_null, __check_distribution_RANDOM },
  { ARRAY,  make_scanner_string("--DISTRIBUTION=modula"), scanner_string_null, NULL },
  { NIL, scanner_string_null, scanner_string_null, NULL}
};

scanner_variable_t hash_strings[]= {
  { ARRAY,  make_scanner_string("--HASH=CRC"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=FNV1A_32"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=FNV1A_64"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=FNV1_32"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=FNV1_64"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=JENKINS"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=MD5"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=MURMUR"), scanner_string_null, NULL },
  { NIL, scanner_string_null, scanner_string_null, NULL}
};


static test_return_t _test_option(scanner_variable_t *scanner, bool test_true= true)
{
  (void)test_true;

  for (scanner_variable_t *ptr= scanner; ptr->type != NIL; ptr++)
  {
    memcached_st *memc;
    memc= memcached(ptr->option.c_str, ptr->option.size);
    if (test_true)
    {
      if (not memc)
      {
        char buffer[2048];
        memcached_return_t rc= libmemcached_check_configuration(ptr->option.c_str, ptr->option.size, buffer, sizeof(buffer));
        std::cerr << "About error for " << memcached_strerror(NULL, rc) << " : " << buffer << std::endl;
      }

      test_true(memc);

      if (ptr->check_func)
      {
        test_return_t test_rc= (*ptr->check_func)(memc, ptr->result);
        if (test_rc != TEST_SUCCESS)
        {
          memcached_free(memc);
          return test_rc;
        }
      }

      memcached_free(memc);
    }
    else
    {
      test_false_with(memc, ptr->option.c_str);
    }
  }

  return TEST_SUCCESS;
}

test_return_t server_test(memcached_st *)
{
  return _test_option(test_server_strings);
}

test_return_t server_with_weight_test(memcached_st *)
{
  return _test_option(test_server_strings_with_weights);
}

test_return_t servers_bad_test(memcached_st *)
{
  test_return_t rc;
  if ((rc= _test_option(bad_test_strings, false)) != TEST_SUCCESS)
  {
    return rc;
  }

  return TEST_SUCCESS;
}

test_return_t parser_number_options_test(memcached_st*)
{
  return _test_option(test_number_options);
}

test_return_t parser_boolean_options_test(memcached_st*)
{
  return _test_option(test_boolean_options);
}

test_return_t behavior_parser_test(memcached_st*)
{
  return TEST_SUCCESS;
}

test_return_t parser_hash_test(memcached_st*)
{
  return _test_option(hash_strings);
}

test_return_t parser_distribution_test(memcached_st*)
{
  return _test_option(distribution_strings);
}

test_return_t parser_key_prefix_test(memcached_st*)
{
  return _test_option(distribution_strings);
}

#define SUPPORT_EXAMPLE_CNF "support/example.cnf"

test_return_t memcached_create_with_options_with_filename(memcached_st*)
{
  if (access(SUPPORT_EXAMPLE_CNF, R_OK))
    return TEST_SKIPPED;

  memcached_st *memc_ptr;
  memc_ptr= memcached(STRING_WITH_LEN("--CONFIGURE-FILE=\"support/example.cnf\""));
  test_true_got(memc_ptr, memcached_last_error_message(memc_ptr));
  memcached_free(memc_ptr);

  return TEST_SUCCESS;
}

test_return_t libmemcached_check_configuration_with_filename_test(memcached_st*)
{
  if (access(SUPPORT_EXAMPLE_CNF, R_OK))
    return TEST_SKIPPED;

  memcached_return_t rc;
  char buffer[BUFSIZ];

  rc= libmemcached_check_configuration(STRING_WITH_LEN("--CONFIGURE-FILE=\"support/example.cnf\""), buffer, sizeof(buffer));
  test_true_got(rc == MEMCACHED_SUCCESS, buffer);

  rc= libmemcached_check_configuration(STRING_WITH_LEN("--CONFIGURE-FILE=support/example.cnf"), buffer, sizeof(buffer));
  test_false_with(rc == MEMCACHED_SUCCESS, buffer);

  rc= libmemcached_check_configuration(STRING_WITH_LEN("--CONFIGURE-FILE=\"bad-path/example.cnf\""), buffer, sizeof(buffer));
  test_true_got(rc == MEMCACHED_ERRNO, buffer);

  return TEST_SUCCESS;
}

test_return_t libmemcached_check_configuration_test(memcached_st*)
{
  memcached_return_t rc;
  char buffer[BUFSIZ];

  rc= libmemcached_check_configuration(STRING_WITH_LEN("--server=localhost"), buffer, sizeof(buffer));
  test_true_got(rc == MEMCACHED_SUCCESS, buffer);

  rc= libmemcached_check_configuration(STRING_WITH_LEN("--dude=localhost"), buffer, sizeof(buffer));
  test_false_with(rc == MEMCACHED_SUCCESS, buffer);
  test_true(rc == MEMCACHED_PARSE_ERROR);

  return TEST_SUCCESS;
}

test_return_t memcached_create_with_options_test(memcached_st*)
{
  memcached_st *memc_ptr;
  memc_ptr= memcached(STRING_WITH_LEN("--server=localhost"));
  test_true_got(memc_ptr, memcached_last_error_message(memc_ptr));
  memcached_free(memc_ptr);

  memc_ptr= memcached(STRING_WITH_LEN("--dude=localhost"));
  test_false_with(memc_ptr, memcached_last_error_message(memc_ptr));

  return TEST_SUCCESS;
}

test_return_t test_include_keyword(memcached_st*)
{
  if (access(SUPPORT_EXAMPLE_CNF, R_OK))
    return TEST_SKIPPED;

  char buffer[BUFSIZ];
  memcached_return_t rc;
  rc= libmemcached_check_configuration(STRING_WITH_LEN("INCLUDE \"support/example.cnf\""), buffer, sizeof(buffer));
  test_true_got(rc == MEMCACHED_SUCCESS, buffer);

  return TEST_SUCCESS;
}

test_return_t test_end_keyword(memcached_st*)
{
  char buffer[BUFSIZ];
  memcached_return_t rc;
  rc= libmemcached_check_configuration(STRING_WITH_LEN("--server=localhost END bad keywords"), buffer, sizeof(buffer));
  test_true_got(rc == MEMCACHED_SUCCESS, buffer);

  return TEST_SUCCESS;
}

test_return_t test_reset_keyword(memcached_st*)
{
  char buffer[BUFSIZ];
  memcached_return_t rc;
  rc= libmemcached_check_configuration(STRING_WITH_LEN("--server=localhost reset --server=bad.com"), buffer, sizeof(buffer));
  test_true_got(rc == MEMCACHED_SUCCESS, buffer);

  return TEST_SUCCESS;
}

test_return_t test_error_keyword(memcached_st*)
{
  char buffer[BUFSIZ];
  memcached_return_t rc;
  rc= libmemcached_check_configuration(STRING_WITH_LEN("--server=localhost ERROR --server=bad.com"), buffer, sizeof(buffer));
  test_true_got(rc != MEMCACHED_SUCCESS, buffer);

  return TEST_SUCCESS;
}

#define RANDOM_STRINGS 100
test_return_t random_statement_build_test(memcached_st*)
{
  std::vector<scanner_string_st *> option_list;

  for (scanner_variable_t *ptr= test_server_strings; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);

  for (scanner_variable_t *ptr= test_number_options; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);

  for (scanner_variable_t *ptr= test_boolean_options; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);

  for (scanner_variable_t *ptr= prefix_key_strings; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);

  for (scanner_variable_t *ptr= distribution_strings; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);

  for (scanner_variable_t *ptr= hash_strings; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);

  for (uint32_t x= 0; x < RANDOM_STRINGS; x++)
  {
    std::string random_options;

    uint32_t number_of= random() % option_list.size();
    for (uint32_t options= 0; options < number_of; options++)
    {
      random_options+= option_list[random() % option_list.size()]->c_str;
      random_options+= " ";
    }

    memcached_st *memc_ptr= memcached(random_options.c_str(), random_options.size() -1);
    if (not memc_ptr)
    {
      switch (errno) 
      {
      case EINVAL:
#if 0 // Testing framework is not smart enough for this just yet.
        {
          // We will try to find the specific error
          char buffer[2048];
          memcached_return_t rc= libmemcached_check_configuration(random_options.c_str(), random_options.size(), buffer, sizeof(buffer));
          test_true_got(rc != MEMCACHED_SUCCESS, "memcached_create_with_options() failed whiled libmemcached_check_configuration() was successful");
          std::cerr << "Error occured on " << random_options.c_str() << " : " << buffer << std::endl;
          return TEST_FAILURE;
        }
#endif
        break;
      case ENOMEM:
        std::cerr << "Failed to allocate memory for memcached_create_with_options()" << std::endl;
        memcached_free(memc_ptr);
        return TEST_FAILURE;
      default:
        std::cerr << "Unknown error from memcached_create_with_options?!!" << std::endl;
        memcached_free(memc_ptr);
        return TEST_FAILURE;
      }
    }
    memcached_free(memc_ptr);
  }

  return TEST_SUCCESS;
}
