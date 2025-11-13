@echo off
chcp 65001 >nul
echo 正在安全清理Keil5编译文件...

for /d %%i in (*) do (
    if exist "%%i" (
        echo 正在清理项目: %%i
        
        rem 删除编译过程中的临时文件
        del /q "%%i\*.bak" 2>nul    rem 备份文件
        del /q "%%i\*.lnp" 2>nul    rem 链接过程文件
        del /q "%%i\*.dep" 2>nul    rem 依赖文件
        del /q "%%i\*.d" 2>nul      rem 依赖文件
        del /q "%%i\*.crf" 2>nul    rem 交叉引用文件
        del /q "%%i\*.o" 2>nul      rem 目标文件
        
        rem 删除项目根目录的编译输出文件（保留Objects里的HEX）
        del /q "%%i\*.bin" 2>nul    rem 二进制文件
        del /q "%%i\*.axf" 2>nul    rem 调试文件
        
        rem 删除日志和临时文件
        del /q "%%i\*.tra" 2>nul    rem 跟踪文件
        del /q "%%i\*.iex" 2>nul    rem 执行文件
        del /q "%%i\*.htm" 2>nul    rem HTML报告
        del /q "%%i\*.sct" 2>nul    rem 分散加载文件
        del /q "%%i\*.map" 2>nul    rem 映射文件
        del /q "%%i\JLinkLog.txt" 2>nul  rem J-Link日志
        
        rem 删除Listings目录
        if exist "%%i\Listings" (
            rmdir /s /q "%%i\Listings" 2>nul    rem 列表文件目录
        )
        
        rem 清理Objects目录，但保留.hex文件
        if exist "%%i\Objects" (
            rem 删除Objects里除了.hex以外的所有文件
            del /q "%%i\Objects\*.o" 2>nul
            del /q "%%i\Objects\*.axf" 2>nul
            del /q "%%i\Objects\*.bin" 2>nul
            del /q "%%i\Objects\*.d" 2>nul
            del /q "%%i\Objects\*.crf" 2>nul
            del /q "%%i\Objects\*.tra" 2>nul
            del /q "%%i\Objects\*.map" 2>nul
			del /q "%%i\Objects\*.htm" 2>nul
			del /q "%%i\Objects\*.sct" 2>nul
			del /q "%%i\Objects\*.lnp" 2>nul
			del /q "%%i\Objects\*.dep" 2>nul
            echo 已保留Objects中的.hex文件
        )
        
        echo 项目 %%i 清理完成
    )
)

echo.
echo 安全清理完成！(已保留DebugConfig和Objects中的.hex文件)
echo.
pause