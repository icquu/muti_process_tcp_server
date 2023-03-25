# [Pandas 库之 DataFrame](https://www.cnblogs.com/geaozhang/p/11335960.html)

> How to use DataFrame ?
>
> > [简介](https://www.cnblogs.com/geaozhang/p/11335960.html#a) 分布式数据集(常用于数据分析)
>
> > [创建 DataFrame](https://www.cnblogs.com/geaozhang/p/11335960.html#b)
>
> > [查看与筛选数据](https://www.cnblogs.com/geaozhang/p/11335960.html#c)：[行列选取](https://www.cnblogs.com/geaozhang/p/11335960.html#d)
>
> > [DataFrame 数据操作](https://www.cnblogs.com/geaozhang/p/11335960.html#e)：[增删改](https://www.cnblogs.com/geaozhang/p/11335960.html#f)

一、About DataFrame

　　DataFrame 是 Python 中 Pandas 库中的一种数据结构，是一种二维表。它类似 excel，或许说它可能有点像 matlab 的矩阵，但是 matlab 的矩阵只能放数值型值（当然 matlab 也可以用 cell 存放多类型数据），DataFrame 的单元格可以存放数值、字符串等，这就和 excel 表很像。

　　同时 DataFrame 可以设置列名 columns 与行名 index，可以通过像 matlab 一样通过位置获取数据也可以通过列名和行名定位。

 

二、创建 DataFrame

版本声明：Python 3.6.7

```
import pandas as pd
import numpy as np　　# pandas 常与 numpy 一起配合使用
```

如果还没安装直接在 cmd 里 pip 安装：

```
> pip install pandas
> pip install numpy
```

1、直接创建

　　可以直接使用 pandas 的 DataFrame 函数创建，比如随机创建一个 4*4 的 DataFrame：

![img](https://img2018.cnblogs.com/blog/1113510/201908/1113510-20190811162333201-100393019.png)

　　np.random.randn(4,4)：存放在DataFrame里的数据

　　index=list('ABCD')：行名（或者可以说是索引）

　　columns=list('ABCD')：列名

　　后两个参数可以使用 list 输入，但是注意，这个list的长度要和 DataFrame 的大小匹配，不然会报错。当然，这两个参数是可选的，你可以选择不设置。而且发现，这两个list是可以一样的，但是每行每列的名字在 index 或 columns 里要是唯一的。

　　小数据量，手工键入数据：

```
df2 = pd.DataFrame([[1,2,3,4],[2,3,4,5], [3,4,5,6],[4,5,6,7]], 
                   index=(['r1','r2','r3','r4']),
                   columns=(['c1','c2','c3','c4']))
```

2、使用字典创建

　　使用 DataFrame 方法，但是字典的每个 key 的 value 代表一列，而 key 是这一列的列名：

![img](https://img2018.cnblogs.com/blog/1113510/201908/1113510-20190811164652081-210345353.png)

 

三、查看与筛选数据

1、查看列的数据类型：使用 dtypes 方法可以查看各列的数据类型

2、查看DataFrame的头尾

　　使用 head 可以查看前几行的数据，默认的是前5行，不过也可以自己设置；

　　使用 tail 可以查看后几行的数据，默认也是5行，参数可以自己设置；

3、查看行名与列名

　　使用 index 查看行名，columns 查看列名

```
In [6]: df3.dtypes
Out[6]:
name      object
age        int64
gender    object
dtype: object

In [7]: df3.head(1)
Out[7]:
  name  age gender
0   张三   18      男

In [8]: df3.tail(1)
Out[8]:
  name  age gender
2   王五   22      男

In [9]: df3.index
Out[9]: RangeIndex(start=0, stop=3, step=1)

In [10]: df3.columns
Out[10]: Index(['name', 'age', 'gender'], dtype='object')
```

4、查看数据值

　　使用 values 可以查看 DataFrame 里的数据值，返回的是一个数组：

```
# 查看所有的数据值
In [11]: df3.values    
Out[11]:
array([['张三', 18, '男'],
       ['李四', 20, '女'],
       ['王五', 22, '男']], dtype=object)
# 查看某一列所有的数据值
In [12]: df3['name'].values
Out[12]: array(['张三', '李四', '王五'], dtype=object)
```

　　使用 loc 或者 iloc （切片）查看数据值，区别是 loc 是根据行名，iloc 是根据数字索引：

　　①loc：(location)，works on labels in the index，只能使用字符型标签来索引数据，不能使用数字来索引数据，不过有特殊情况，当数据框dataframe的行标签或者列标签为数字，loc就可以来其来索引。

　　②iloc：(i=integer)，works on the positions in the index (so it only takes integers)，主要使用数字来索引数据，而不能使用字符型的标签来索引数据。

```
In [13]: df3.loc[1]
Out[13]:
name      李四
age       20
gender     女
Name: 1, dtype: object

In [14]: df3.iloc[1]
Out[14]:
name      李四
age       20
gender     女
Name: 1, dtype: object
```

5、查看行列数：使用 shape 查看行列数，注意：参数为0 表示查看行数，参数为1 表示查看列数。

6、DataFrame 数据格式的行列选取

```
import numpy as np
import pandas as pd

data = pd.DataFrame(np.arange(16).reshape(4,4),index=list('abcd'),columns=list('wxyz'))

data['w']  　　   #选择表格中的'w'列，使用类字典属性,返回的是Series类型

data.w    　　    #选择表格中的'w'列，使用点属性,返回的是Series类型

data[['w']]      #选择表格中的'w'列，返回的是DataFrame属性

data[['w','z']]  #选择表格中的'w'、'z'列

data[0:2]        #返回第1行到第2行的所有行，前闭后开，包括前不包括后

data[1:2]        #返回第2行，从0计，返回的是单行，通过有前后值的索引形式，
                 #如果采用data[1]则报错

data.ix[1:2]     #(deprecated)返回第2行的第三种方法，返回的是DataFrame，跟data[1:2]同

data['a':'b']    #利用index值进行切片，返回的是**前闭后闭**的DataFrame, 
                 #即末端是包含的  
data.irow(0)     #(deprecated)取data的第一行
data.icol(0)     #(deprecated)取data的第一列

data.head()      #返回data的前几行数据，默认为前五行，需要前十行则dta.head(10)
data.tail()      #返回data的后几行数据，默认为后五行，需要后十行则data.tail(10)

data.iloc[-1]      #选取DataFrame最后一行，返回的是Series
data.iloc[-1:]     #选取DataFrame最后一行，返回的是DataFrame

data.loc['a',['w','x']]   #返回‘a’行'w'、'x'列，这种用于选取行索引列索引已知

data.iat[1,1]             #选取第二行第二列，用于已知行、列位置的选取
```

 

四、DataFrame 数据操作

1、转置：直接字母T，这就有点 线性代数 的味道了哈

![img](https://img2018.cnblogs.com/blog/1113510/201908/1113510-20190811172200628-1182064085.png)

2、描述性统计

　　使用 describe 方法可以对数据根据“列”进行描述性统计：

```
In [15]: df3.describe()
Out[15]:
        age
count   3.0
mean   20.0
std     2.0
min    18.0
25%    19.0
50%    20.0
75%    21.0
max    22.0
```

　　由上可以看出，如果有的列是非数值型的，那么就跳过不会进行统计；如果想对行进行描述性统计，可以灵活处理一下：转置后进行 describe。

3、计算

　　使用 sum 默认对每“列”求和，sum(1) 为对每“行”求和：

```
In [16]: df3.sum()        # 列求和
Out[16]:
name      张三李四王五      # 如果元素是字符串，使用sum也会加起来（字符串拼接）
age           60
gender       男女男
dtype: object    

In [17]: df3.sum(1)       # 行求和
Out[17]:
0    18                   # 行中，如果有字符串有数值，则只计算数值
1    20
2    22
dtype: int64
```

　　数乘运算使用 apply（应用 lambda 实现）：

```
In [18]: df2
Out[18]:
    c1  c2  c3  c4
r1   1   2   3   4
r2   2   3   4   5
r3   3   4   5   6
r4   4   5   6   7

In [19]: df2.apply(lambda x:x*2)
Out[19]:
    c1  c2  c3  c4
r1   2   4   6   8
r2   4   6   8  10
r3   6   8  10  12
r4   8  10  12  14
```

　　如果元素是字符串，则会把字符串再重复一遍。

　　乘方运算跟 matlab 类似，直接使用两个 *，乘方运算如果有元素是字符串的话，就会报错。

4、增删改

```
In [36]: df2
Out[36]:
    c1  c2  c3  c4
r1   1   2   3   4
r2   2   3   4   5
r3   3   4   5   6
r4   4   5   6   7

# 1.增
In [37]: df2['c5'] = 8                      # 创建新列并赋值（统一值）

In [38]: df2
Out[38]:
    c1  c2  c3  c4  c5
r1   1   2   3   4   8
r2   2   3   4   5   8
r3   3   4   5   6   8
r4   4   5   6   7   8

In [39]: df2.insert(0,'c0',[9, 9, 9, 9])    # 使用 insert 方法可以指定把列插入到第几列，其他的列顺延

In [40]: df2
Out[40]:
    c0  c1  c2  c3  c4  c5
r1   9   1   2   3   4   8
r2   9   2   3   4   5   8
r3   9   3   4   5   6   8
r4   9   4   5   6   7   8

# 2.删
In [41]: del df2['c0']

In [42]: df2
Out[42]:
    c1  c2  c3  c4  c5
r1   1   2   3   4   8
r2   2   3   4   5   8
r3   3   4   5   6   8
r4   4   5   6   7   8

In [43]: df2.drop('c1',axis=1)    # 使用 drop() 方法删除
Out[43]:
    c2  c3  c4  c5
r1   2   3   4   8
r2   3   4   5   8
r3   4   5   6   8
r4   5   6   7   8

In [44]: df2                      # 用 drop 删除时，删的是视图，并没有真正删除
Out[44]:
    c1  c2  c3  c4  c5
r1   1   2   3   4   8
r2   2   3   4   5   8
r3   3   4   5   6   8
r4   4   5   6   7   8

    # dorp()可以通过axis（行：axis=0 ，列：axis=1）可以控制删除行或列，默认是行
    # dorp()可以同时删除多行或多列，如 df2.drop(['c1','c2'], axis=1)

# 3.改
In [45]: df2['c5']['r3'] = 99

In [46]: df2
Out[46]:
    c1  c2  c3  c4  c5
r1   1   2   3   4   8
r2   2   3   4   5   8
r3   3   4   5   6  99
r4   4   5   6   7   8
```

5、合并拼接

　　使用 join 可以将两个 DataFrame 合并，但只根据行列名合并，并且以作用的那个 DataFrame 的为基准。

　　但是，join 这个方法还有 how 这个参数可以设置，合并两个 DataFrame 的交集或并集。参数为 'inner' 表示交集，'outer' 表示并集。

　　如果要合并多个 Dataframe，可以用 list 把几个 Dataframe 装起来，然后使用 concat 转化为一个新的 Dataframe。