# obs-studio for HQ

*Read this in other languages: [English](README.en.md)*

##build指导

* windows上build HQ使用的obs-studio的要求
    * dependencies2013.zip
   
    * Windows版本的CMake
   
    * visual studio 2013 
    
* 用CMake-gui build项目：

    * 启动CMake-gui 选好源码路径和项目生成路径，configure
    
    * 设置windows环境变量:
        * DepsPath 

               dedependencies2013.zip解压后Win32所在的路径
        
        * 取消Enable_UI和ENABLE_SCRIPTING
        * configure成功之后，Generate即可生成vs工程
            
              
