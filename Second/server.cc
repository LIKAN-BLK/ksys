#include <stdio.h>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/re/util/object_registry>
#include <l4/cxx/ipc_server>
#include <l4/cxx/string>
#include "shared.h"
#include <string.h>
static L4Re::Util::Registry_server<> server;

class Crypt_server : public L4::Server_object
{
public:
  int dispatch(l4_umword_t obj, L4::Ipc::Iostream &ios);
};
void encrypt(char * str){
        char *key =new char[strlen("abcdefgh")];
    strncpy(key,"abcdefgh",strlen("abcdefgh"));
        for(unsigned int i=0;i<strlen(str);++i){
                str[i]=str[i]^key[i%strlen(key)];
        }
}


int
Crypt_server::dispatch(l4_umword_t, L4::Ipc::Iostream &ios)
{
  l4_msgtag_t t;
  ios >> t;

  if (t.label() != Protocol::Crypt)
    return -L4_EBADPROTO;

  L4::Opcode opcode;
  ios >> opcode;
  unsigned long val_size;
  ios>>val_size;
  char val[val_size];
  switch (opcode)
    {
    case Opcode::func_enc:{
      ios.get(val,val_size);
      encrypt(val);
      ios.put(val,val_size);
      return L4_EOK;
     }
    case Opcode::func_dec:
      ios.get(val,val_size);
      encrypt(val);
      ios.put(val,val_size);
      return L4_EOK;
    default:{return -L4_ENOSYS;}
    }
}

int
main()
{
  static Crypt_server crypt;

  if (!server.registry()->register_obj(&crypt, "enc_server").is_valid())
    {
      printf("Could not register my service, is there a 'enc_server' in the caps table?\n");
      return 1;
    }

  printf("Welcome to the server!\n"
         "I can do decryptions and encryptions.\n");

  server.loop();

  return 0;
}
