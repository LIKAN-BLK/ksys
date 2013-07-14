#include <l4/sys/err.h>
#include <l4/sys/types.h>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/cxx/ipc_stream>
#include <l4/cxx/string>
#include <stdio.h>
#include <l4/cxx/iostream>
#include "shared.h"
#include <string.h>
static int
func_enc_call(L4::Cap<void> const &server, char **result,
                			char *val)
{
  L4::Ipc::Iostream s(l4_utcb());
  unsigned long val_size = strlen(val);
  s << l4_umword_t(Opcode::func_enc) << val_size;
  s.put(val,val_size);
  int r = l4_error(s.call(server.cap(), Protocol::Crypt));
  if (r)
    return r; // failure
  *result=new char[val_size];
  s.get(*result,val_size);
  return 0; // ok
}

static int
func_dec_call(L4::Cap<void> const &server, char **result,
              				char *val)
{
  L4::Ipc::Iostream s(l4_utcb());
  unsigned long val_size = strlen(val);
  s << l4_umword_t(Opcode::func_dec) << val_size;
  
  s.put(val,val_size);
  int r = l4_error(s.call(server.cap(), Protocol::Crypt));
  *result = new char[val_size];
  if (r)
    return r; // failure
  s.get(*result,val_size);
  return 0; // ok
}

int
main()
{
  L4::Cap<void> server = L4Re::Env::env()->get_cap<void>("enc_server");
  if (!server.is_valid())
    {
      printf("Could not get server capability!\n");
      return 1;
    }

  char *result;
  char *result1;
  char *val =new char[strlen("HelloWorld")];
  strncpy(val,"HelloWorld",strlen("HelloWorld"));
  
  printf("Asking for \"%s\" \n",val);
  if (func_enc_call(server, &result, val))
    {
      printf("Error talking to server\n");
      return 1;
    }
  printf("Result of encryption call: %s;\n",result);
  if (func_dec_call(server, &result1,result))
    {
      printf("Error talking to server\n");
      return 1;
    }
  printf("Result of decryption call: %s\n", result1);
  delete result;
  delete result1;
  return 0;
}
