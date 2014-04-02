#include "test.h"
#include "mongo.h"

#include <sys/socket.h>

#define PACKET_SIZE 64

void
test_func_client_get_last_errno (void)
{
  mongo_connection *conn;
  mongo_packet *p;
  bson *b;
  guint8 *data;

  conn = mongo_connect (config.primary_host, config.primary_port);

  ok (conn != NULL, "mongo_connect() works");
  ok (mongo_connection_get_last_errno (conn) == 0, "errno is 0 when connected to server");

  b = bson_new_sized (PACKET_SIZE + 1024);

  data = g_malloc (PACKET_SIZE);

  bson_append_binary (b, "data", BSON_BINARY_SUBTYPE_GENERIC,
                      data, PACKET_SIZE);

  bson_finish (b); 
  
  p = mongo_wire_cmd_insert (1, config.ns, b, NULL);

  ok (mongo_packet_send (conn, p) == TRUE, "mongo_packet_send() works");

  ok (mongo_connection_get_last_errno (conn) == 0,
      "errno is 0 when mongo_packet_send() send works");

  shutdown (conn->fd, SHUT_RDWR);

  ok (mongo_packet_send (conn, p) == FALSE,
      "mongo_packet_send() fails when connection is broken");

  ok (mongo_connection_get_last_errno (conn) != 0,
      "errno is set when mongo_packet_send() is called" \
      " with a broken connection");

  bson_free (b);
  mongo_wire_packet_free (p);
  g_free (data);
  mongo_disconnect (conn);
}

RUN_NET_TEST (6, func_client_get_last_errno);
