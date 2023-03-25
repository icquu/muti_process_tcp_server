
# 元组的访问,元组类似静态列表或者静态数组
# 字典(hash表) 集合(不重复)

a={'a': 1, 'b': 2, 'c': 3}
b=dict()
a['a']="key"
print(a.get("a"))
print(a.keys())
print(a.values())
print(a.items())
a.clear()
print(a.keys())


set1 = {'a', 'b', 'c'}
print(type(set1))

t = 'a' in set1
print(t)

# 分支判断循环
input_str = input("输入一个数字:")
input_num = int(input_str)
if input_num > 0:
    print('True')
    print(f"{input_num}>0")
elif input_num==0:
    print("input_num=0")
else:
    print(f"{input_num}<0")
print('over')


# 循环
num = int(input("输入一个数字:"))
while num > 0:
    print(num)
    num = int(input("输入一个数字:"))


# 循环遍历字符串,列表,元组,字典,集合
str1 = '3434343'
for item in str1:
    print(item,end=' ')

# for item in range(1,100,2):
#    print(item)

# break continue
import random
a = random.randint(0,100)
while True:
    num = int(input("输入一个数字:"))
    if num == a:
        print("猜对了")
        break
    elif num > a:
        print("大了")
    else:
        print("小了")
print("游戏结束")

# 函数
def demo():
    print("我是demo函数")

demo()

def demo1(a, b):
    print(f"我有{a}和{b}两个参数")

demo1(a="aaa", b="dde")

def sum(a,b):
    return a+b

print(sum([3,2,3,3],[3,3,3,2]))

# 不清楚python是否有可变参数和goto语句
# python函数中参数传值方式需要注意.pycharm会有所提示

z = 1
def demo2(a):
    global z
    z = a + 1
    print(z)
demo2(a=100)
print(z)

# 可变参数函数
def add(*a):
    ret = 0
    for item in a:
        ret += item
    return ret
    print(a)

add(1,2,3)

def add1(**kwargs):
    print(kwargs)

add1(a=1,b=2)
    
def test(a, b=False):
    if b:
        return a
    else:
        return a*a

test(a=2,b=True)
    





























