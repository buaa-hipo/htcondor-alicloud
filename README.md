此代码基于 htcondor 8_7_2 分支

grid分支为最新修改源码

--------------------------------------------------------------

2020-7-12

# 功能
我们为htcondor添加了一个新的服务器提供商alicloud，支持灵活的计算资源申请和任务调度操作


# 接口修改

## 现有接口

按量申请七个接口加竞价计费的申请接口

## 还需修改

1.若实现竞价一整套接口还需要修改：

ECS_VM_START_SPOT
ECS_VM_STOP_SPOT
ECS_VM_STATUS_SPOT
ECS_VM_STATUS_ALL_SPOT

2.多量申请虚拟机
阿里云ECS_VM_STATUS是分页查询，默认每页10个 最大100个 修改到100后DONE现象未发现 但是会有HOLD，超过100台就只能从网格部分下手修改

3.地区选择还是以读取txt形式实现，希望写入job

## 阅读需求

ec2_gahp或ecs_gahp整个文件夹约5k行代码

# 网格修改



目前已修改完毕，未进行测试，已经通过编译

修改内容如下：

src/condor_includes/condor_attributes.h [done]
内容：EC2JOB宏定义 line934-964
修改思路：在文件内复制一份ESC

src/condor_utils/submit_utils.h [done]
内容：宏定义 定义了sub文件各项参数
修改思路：在文件内复制一份ESC，[增加地区项，可以选择性删除][undo]

src/condor_utils/submit_utils.cpp [done]
内容：sub文件参数检索 处理 line：3058-4929
修改思路：在文件内复制一份ESC，删除重定义，[增加地区项，可以选择性删除][undo]

src/condor_utils/param_info.in [done]
内容：配置文件 htcondor的配置项
修改思路：在文件内复制一份ESC

src/condor_gridmanager/gridmanager.cpp [done]
内容：网格内容选择 line:352-368
修改思路：添加头文件ecsjob.h 复制一份ECS，删除重定义

src/condor_gridmanager/ec2-gahp-client.cpp [done]
内容：ec2_gahp命令调用准备 将信息转化为标准ec2_gahp命令
修改思路：复制一份整个文件，修改重名函数，[对参数项，参数个数调整][undo]

src/condor_gridmanager/ec2job.h [done]
src/condor_gridmanager/ec2job.cpp [done]
内容：任务运行的函数
修改思路：复制一份，改名

src/condor_gridmanager/ec2resource.h [done]
src/condor_gridmanager/ec2resource.cpp [done]
内容：密钥对以及实例状态管理
修改思路：复制一份，改名

src/condor_gridmanager/CMakeList.txt [done]
内容：编译
修改思路：加一份ecs

src/condor_gridmanager/gahp-client.h [done]
内容：格式规范
修改思路：加一份ecs

src/condor_utils/condor_holdcodes.h [done]
内容：错误代码
修改思路：错误号递增添加ECS
