#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/utsname.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alex Anastasia");

#define __NR_kill 37

int scan_range = 100;

asmlinkage long (*original_sys_kill)(int pid, int sig);
void** sys_call_table = NULL;
int success = 0;
char *program_name = "";
MODULE_PARM(program_name, "s");

asmlinkage long our_sys_kill(int pid, int sig){
    task_t* p;
    p = find_task_by_pid(pid);

    if (!program_name) {
        return original_sys_kill(pid,sig);
    }
    int i = 0;
    int equal = 0;
    for(; i < 16; i++){
        if(p->comm[i] && program_name[i] ){
            if(p->comm[i] == program_name[i]){
                if (program_name[i] == '\a') {
                    equal = 1;
                    break;
                }
            }else { break;}
        } else {
            if(!p->comm[i] && !program_name[i]){
                equal = 1;
                break;
            }
            break;
        }
    }

    if(sig == 9 && equal == 1){
        return -EPERM;
    }else {
        return original_sys_kill(pid,sig);
    }
}

void find_sys_call_table(int scan_range) {
    int i = 0;
    void** itr = (void**)&system_utsname;

    for(;i < scan_range; i++, itr++){
        if (*itr == sys_read) {
            success = 1;
            sys_call_table = itr - __NR_read;
            break;
        }
    }
    return;
}

int init_module(void) {
    while (success != 1) {
        scan_range+=100;
        find_sys_call_table(scan_range);
    }

    original_sys_kill = sys_call_table[__NR_kill];

    sys_call_table[__NR_kill] = our_sys_kill;
    return 0;
}

void cleanup_module(void) {
    sys_call_table[__NR_kill] = original_sys_kill;

}
