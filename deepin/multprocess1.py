import os
from multiprocessing import Process
from time import sleep
# 多进程更适合计算密集型程序，多线程更适合IO操作较密集的程序

def task1(s, name):
    for i in range(3):
        sleep(1)
        print('任务1', os.getpid(), os.getppid(), s, name)


def task2(s, name):
    for i in range(3):
        sleep(1)
        print('任务2', os.getpid(), os.getppid(), s, name)


if __name__ == '__main__':
    print(os.getpid())
    p1 = Process(target=task1, name='task1', args=[1, 'one'])
    p1.start()
    print(p1.name)
    p2 = Process(target=task2, name='task2', args=[2, 'two'])
    p2.start()
    print(p2.name)
    p1.join()
    p2.join()
