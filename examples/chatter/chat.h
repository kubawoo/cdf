#ifndef CHATTER_CHAT_H
#define CHATTER_CHAT_H

#include <core.h>
#include <http.h>
#include <json.h>
#include <log.h>

/* One turn in the conversation. The API takes the whole history on every
   request, so the turns are kept in the order they were exchanged.

   This is a CDF class rather than a bare struct because List reference counts
   whatever it holds: passing a plain struct makes List write through its first
   words, which corrupts whatever else lives there. */
typedef struct {
    inherits(Object);
    String * role;      /* "user" or "assistant" */
    String * content;
} ChatTurn;

ChatTurn * ChatTurn_new2(ChatTurn *, String *, String *);
void ChatTurn_delete(ObjectPtr);

typedef struct {
    String * base_url;      /* e.g. http://localhost:8080 */
    String * token;         /* bearer token, or NULL when the server needs none */
    String * model;
    List   * turns;         /* of ChatTurn, owned by this client */
    Logger * logger;

    /* Whether to let a reasoning model deliberate before answering. Off by
       default, because the deliberation is long and the user asked a question,
       not for the working. */
    bool thinking;

    HttpClient * client;
} ChatClient;

ChatClient * new_chat_client(const char * base_url, const char * model, const char * token, bool thinking);

/* Appends a turn to the history the next request will carry. */
void chat_add_turn(ChatClient * chat, const char * role, const char * content);

/* Drops every turn, keeping the client usable for a fresh conversation. */
void chat_clear(ChatClient * chat);

/* Serializes the history as the "messages" array of a chat completion
   request. Caller frees the result. */
String * chat_build_request_body(ChatClient * chat, int max_tokens);

/* Sends the history to /v1/chat/completions and returns the assistant's reply,
   which the caller frees. Returns NULL when the exchange failed, leaving the
   reason in err (also a caller-owned string). A model that returns only
   reasoning_content is treated as an empty reply rather than a failure. */
String * chat_send(ChatClient * chat, int max_tokens, String * err);

/* Asks the server which models it serves and returns them as a List of String
   the caller owns. Returns NULL when the call failed, leaving the reason in
   err. /v1/models answers with a bare JSON array, so the root of the response is
   a list rather than an object. */
List * chat_list_models(ChatClient * chat, String * err);

void chat_client_delete(ChatClient * chat);

/* Renders a raw HTTP/JSON failure into something worth showing a user. */
String * chat_describe_failure(const char * url);

#endif