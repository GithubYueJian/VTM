块划分是视频编码的第一步，为了得到划分后的CU信息，在VVC标准测试软件VTM 7.0的基础上修改代码，将每个帧的CU信息打印出来。在xCompressCU中，将对CTU进行递归划分，用RDcost选择出最优的划分方式，因此在xCompressCU中很难提取到QTMTT的最优划分方式。要得到最终划分结果，必须在xCompressCU之后。VTM提供了遍历每个CodingStructure中所有CU的函数traverseCUs()，因此只需要在xCompressCU之后，遍历CTU中每个CU就可以得到其位置和尺寸信息。代码插入在EncSlice.cpp文件encodeCTUs（）函数后面（在geihub上可以从以下路径找到VTM/blob/master/VVCSoftware_VTM-VTM-7.0/source/Lib/EncoderLib/EncSlice.cpp）。

