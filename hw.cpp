#include "v8/include/v8.h"
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

char* run(char* js) {
  // Create a stack-allocated handle scope.
  v8::HandleScope handle_scope;
  
  // Create a new context.
  v8::Persistent<v8::Context> context = v8::Context::New();
  
  // Enter the created context for compiling and
  // running the hello world script. 
  v8::Context::Scope context_scope(context);

  // Create a string containing the JavaScript source code.
  v8::Handle<v8::String> source = v8::String::New(js);

  v8::TryCatch trycatch;
  // Compile the source code.
  // TODO support SyntaxErrors
  v8::Handle<v8::Script> script = v8::Script::Compile(source);
  
  if (script.IsEmpty()) {
    std::cout << "Whoops!";
    char* ascii[1];
    return *ascii;
  }

  // Run the script to get the result.
  v8::Handle<v8::Value> result = script->Run();
  if (result.IsEmpty()) {  
    v8::Handle<v8::Value> exception = trycatch.Exception();
    v8::String::AsciiValue exception_str(exception);
    printf("Exception: %s\n", *exception_str);
    return *exception_str;
    // ...
  }
  
  // Dispose the persistent context.
  context.Dispose();

  v8::String::Utf8Value ascii(result);

  return *ascii;
}

static char*
message_to_str (zmq::message_t & message) {
  return static_cast<char*>(message.data());
}

int main(int argc, char* argv[]) {
  
  zmq::context_t context (1);
  zmq::socket_t socket(context, ZMQ_REP);
  socket.bind("tcp://*:9000");

  while(true){
    zmq::message_t request;

    socket.recv(&request);
    std::cout << "Recieved message" << std::endl;

    std::string str = run(message_to_str(request));

    zmq::message_t reply(str.length());
    memcpy(static_cast<char*>(reply.data()), str.data(), str.length());
    socket.send(reply);
  }
  return 0;
}
