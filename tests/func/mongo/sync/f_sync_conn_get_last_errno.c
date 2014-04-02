#include "test.h"
#include "mongo.h"

#include <sys/socket.h>

#define INVALID_NS "test.$Uncle$.Dagobert$"

void
test_func_mongo_sync_conn_get_last_error (void)
{
  mongo_sync_connection *conn = NULL;
  bson *b;
  int my_errno = 0;

  b = bson_new ();
  bson_append_int32 (b, "f_sync_conn_get_last_error", 1);
  bson_finish (b);

  ok (mongo_sync_conn_get_last_error(NULL, &my_errno) == FALSE,
      "Getting last error msg for a NULL connection object should fail.");

  conn = mongo_sync_connect (config.primary_host, config.primary_port,
                             TRUE);

  ok (mongo_sync_conn_get_last_error(conn, &my_errno) == TRUE,
      "Getting last error msg for a valid connection works");

  ok (my_errno == 0, "errno is set to 0 after a successful connection");

  ok (mongo_sync_conn_set_safe_mode (conn, TRUE) == TRUE,
      "Setting safe mode works.");

  ok (mongo_sync_cmd_insert (conn, config.ns, b, NULL) == TRUE,
      "Inserting works with correct namespace when safe mode is on");

  ok (mongo_sync_cmd_insert (conn, INVALID_NS, b, NULL) == FALSE,
      "Inserting fails with invalid namespace when safe mode is on");

  mongo_sync_conn_get_last_error(conn, &my_errno);

  ok (my_errno == 0,
      "errno is set to zero, when insertion failed on a valid connection");

  shutdown (conn->super.fd, SHUT_RDWR);

  ok (mongo_sync_cmd_insert (conn, INVALID_NS, b, NULL) == FALSE,
      "Inserting fails with invalid namespace when safe mode is on");

  mongo_sync_conn_get_last_error(conn, &my_errno);
  
  ok (my_errno != 0,
     "errno is set to a non-zero value (->error) when insertion failed on "\
     "a broken connection");

  mongo_sync_disconnect (conn);
}

RUN_NET_TEST (9, func_mongo_sync_conn_get_last_error);
