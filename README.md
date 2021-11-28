<center>

# 《高级数据库系统》课程实验 #

</center>

实验要求[原链接](http://staff.ustc.edu.cn/~jpq/courses/lab_adb-cs.html)。

根据下面的设计要求，实现一个Storage and Buffer Manager。
 设计具体要求：[adbs-lab.pdf](http://staff.ustc.edu.cn/~jpq/courses/adb-cs/adbs-lab.rar)

* 1. 按文档要求实现一个Storage and Buffer Manager，要求至少实现LRU算法。

* 2. 底层文件默认采用目录式堆文件。

* 3. 建议先构建一个5万个page（页号从0到49999）的堆文件（使用自己实现的FixNewPage()接口），然后再运行trace文件：[data-5w-50w-zipf.txt](http://staff.ustc.edu.cn/~jpq/courses/adb-cs/data-5w-50w-zipf.rar)（包含了50万次满足Zipfan分布-0.8的页面请求），根据设计文档要求统计磁盘IO、Buffer命中率、运行时间等数据。

* 4. 下面的实验为选做内容，不做强制要求。如果完成了选做实验（一个或者多个），最后实验成绩会根据完成的情况适当加分：
（1）实现CLOCK、LRU-2、LIRS等其它缓存置换算法（至少一种），并与LRU算法的性能进行对比；
（2）加入缓存请求时的并发控制，通过内存锁(latch)解决缓存访问时的冲突；要求通过多线程方式执行trace并给出测试结果。