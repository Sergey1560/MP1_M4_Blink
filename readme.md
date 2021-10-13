# Пример проекта для ядра M4 в STM32MP157

После сборки blink.elf скопировать в /lib/firmware/ целевой системы.

```
echo blink.elf > /sys/class/remoteproc/remoteproc0/firmware #название файла для загрузки
echo start > /sys/class/remoteproc/remoteproc0/state #запустить ядро M4
echo stop > /sys/class/remoteproc/remoteproc0/state #остановить ядро M4
cat /sys/class/remoteproc/remoteproc0/state  #проверка состояния ядра M4
```
