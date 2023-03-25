除了通过pip进行在线安装，还可以下载安装包，进行本地安装。
https://pypi.org/project/PyMySQL/#files
下载*.whl的文件,执行 pip install *.whl 完成本地安装

全民一起玩Python50集
# https://www.bilibili.com/video/BV1n7411r7AD?p=31
python全套视频教程700集
# https://www.bilibili.com/video/BV1R7411F7JV?p=192
Python 的内容实在太多了，幸亏我有一个学习目录，分享给大家，每天打卡学习
https://www.bilibili.com/video/bv1Lt4y1U7uN/
1 Python 基础 BV1Lt4y1U7uN
2 Python 进阶 BV1jz411b7DH
3 Python 数据库 BV1Pg4y1a7a7
4 Python GUI与坦克大战 BV1jE411F72o
5 Python Numpy BV1jE411F79F
6 Python Matplotlib BV1EE411F7od
7 Python 并发编程 BV1EE411F7vV
8 Python 网络编程 BV1EE411F7Ew
9 Python 算法与数据结构 BV1hE411F7FS
10 Python 函数式编程 BV18E411F7YA
11 Python 正则 BV1N64y1u7Ty
12 Python 图像处理 BV18E411F781
13 Python 人脸识别 BV1aE411F7KC
14 Python 语音识别(PocketSphinx) BV1hE411F747
15 Python 神经网络(TensorFlow) BV1CE411F7z4
16 Python 协程和异步 IO BV1vE411F7Z9
17 Python 人工智能和聊天机器人实战 BV1zE411F7a4
18 Python Django 在线课堂项目实战 BV1DE411F7gx
19 Python 内存管理 BV195411677k
20 Linux 基础教程 BV1sz411b7um
21 WEB 前端开发基础 HTML CSS JS 基础 BV1np4y1C7rP
22 Python Tornado Web 基础与项目实战 BV1kA41187TD
23 Python Django WEB 基础与项目实战 BV1YQ4y1M7w9
24 Python Django WEB 商城项目实战 BV1z54y1R7Cf
25 爬虫基础与实战 BV1z541167mu

无可奈何哈哈灬推荐一套最新 高口碑的2020年python教程：av327814069

bili_658616697712020版Python3.8极速入门教程，3小时入门，av58530260，这个学不会，你可以来打我[喜欢]

初级入门基础：BV1ex411x7Em
入门基础进阶：BV19441137xy
深度学习：BV1Wt411C75s
机器学习：BV1nt411r7tj
人工智能,机器算法：BV1a7411d7fk
更多IT技术教程以及资料，君羊：10.943.644.98

import math
import winsound
import os
# 播放音频文件
os.system('start <FULLPATH>')
if int(input())>5:
	os.system('start <FULLPATH>')
else:
	os.system('start <FULLPATH>')

# 文件操作
f=open('<FULLPATH>', 'r')
p=f.readlines()
for s in p:
	if '' in s:
		print(s)
f.close()

input()
print()
raw_input()
int()
float()
# 避免浮点误差的方法,使用Decimal类，转换为整数的计算，把结果转换为字符串，再用点号拼接整数部分和小数部分

# 异常处理
try:
	pass
except Exception as e:
	raise
else:
	print('执行顺利完成')
finally:
	print('不管怎样，执行完了')

try:
	pass
except FileNotFoundError:
	pass
except Exception as e:
	raise e

# with方式会自动关闭打开的文件，连接的打开和关闭
with open('01.txt', encoding='utf8') as f:
	pass


isinstance(变量名, 类型名)比 使用type().__name__再判断
这个字符串更好用。

# 函数设计，软件设计思路
# 提供一个统一的调用接口，在这个接口里进行逻辑判读调用不同的函数。
# 这样就实现了分层和解耦。

# 变量的作用域，全局变量（模块变量）和局部变量（函数变量）
# 函数中使用模块变量（全局变量）可以使用关键字global

# python变量的储存原理：有指针的感觉

python包管理pip：
python协程的使用套路（解决IO等耗时操作，CPU空闲等待不能进行其他任务的情况，使用该方法可以控制CPU进行其他任务，等待耗时操作完成，再回来继续执行）：
生成器
greenlet
gevent

加上丰富的三方包，可以实现非常多的功能。
给几个国内速度较快的pip源
cat ~/.pip/pip.conf
index-url=https://pypi.doubanio.com/simple
# trusted-host=mirrors.aliyun.com
# index-url=http://mirrors.aliyun.com/pypi/simple/
# index-url=http://pypi.tuna.tsinghua.edu.cn/simple/
# index-url=http://mirrors.ustc.edu.cn/pypi/web/

