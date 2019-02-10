echo off
rem download Apache Ant from http://ant.apache.org/bindownload.cgi
rem and add path to ant to PATH environment variable

set PATH=%CGATE_HOME%\bin;%PATH%

echo on

call ant.bat ^
run.sendMt

rem run.aggrSpy -DisinId=259726 -Ddepth=50 -DfileName=asout.txt
rem run.orderBook -DisinId=259726 -Ddepth=50 -DfileName=obout.txt
rem run.p2sys -Dlogin=your_login -Dpwd=your_pwd
rem run.repl -Dindex=0
rem run.send
rem run.sendMt