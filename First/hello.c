#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include <string.h>

#include <l4/re/env.h>
#include <l4/sys/ipc.h>
#include <l4/sys/vcon.h>
int main(void){
  l4re_env_t *env = l4re_env();
	l4_msg_regs_t *mr = l4_utcb_mr();
	mr->mr[0]=L4_VCON_WRITE_OP;
	mr->mr[1]=7;
	memcpy(&mr->mr[2],"hello\n",7);
	l4_msgtag_t tag,ret;
	tag=l4_msgtag(L4_PROTO_LOG,4,0,0);
	ret = l4_ipc_send(env->log,l4_utcb(),tag,L4_IPC_NEVER);
	if(l4_msgtag_has_error(ret)){
		puts("error\n");
		exit(1);
	}
	return 0;
}
