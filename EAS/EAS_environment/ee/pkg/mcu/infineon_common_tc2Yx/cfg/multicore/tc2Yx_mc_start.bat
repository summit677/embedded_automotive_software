@ECHO OFF
REM script to start debugger for core_0.
REM other core(s) will be started by practice script

SET P1_PORT=10000
SET P2_TITLE=Tricore_CORE0
SET P3_TMP=#T32TMP#
SET P4_SYS=#T32SYS#
SET P5_HELP=#T32SYS#/pdf
SET P10_SCRIPT=#T32SCRIPT#
SET P11_ARCH=#T32ARCH#

IF "%1"=="" (
  SET P6_PBI=USB
  SET P7_OPT=CORE=1
  SET P8_OPT=
  SET P9_OPT=
) ELSE (
  SET P6_PBI=NET
  SET P7_OPT=NODE=%1
  SET P8_OPT=PACKLEN=1024
  SET P9_OPT=CORE=1
)
pushd %~dp0
start %P4_SYS%\bin\%P11_ARCH%\t32mtc -c config_#TC2YX#_mc.t32 %P1_PORT% %P2_TITLE% %P3_TMP% %P4_SYS% %P5_HELP% %P6_PBI% %P7_OPT% %P8_OPT% %P9_OPT% -s %P10_SCRIPT%
popd
