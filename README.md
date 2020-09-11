# TOPPERS SafeG-M MicroPython demo

This branch implements a demonstration of TOPPERS SafeG-M. The demo program runs MicroPython while protecting TOPPERS ASP3 kernel.

### Supported hardware and functionality

- STM32L552 - GPIO, SysTick, Internal flash storage

### Common setup sequence

Run the following commands to initialize a repository

```
$ git clone https://github.com/mtkrtk/micropython
$ cd micropython
$ git checkout SafeG-M
$ make -C mpy-cross
$ git submodule init
```

### Hardware specific configurations

#### STM32L552

1. Run the following commands to clone submodule.

   ```
   $ git submodule update lib/stm32lib/
   ```

2. Setup option bytes as shown below.

   - DBANK = 1
   - TZEN = 1
   - SECWM1_PSTRT = 0x00
   - SECWM1_PEND = 0x51
   - SECWM2_PSTRT = 0x7F
   - SECWM2_PEND = 0x00

   Note: DBANK cannot be changed if there is any secure or write protected area.

3. Launch STM32CubeIDE and specify safeg-m/target/stm32l552/ as the workspace.

4. Select "File" → "Import..." → "General" → "Existing Projects into Workspace" and specify the workspace directory to "Select root directory:" Then import all the projects.

5. Right clink sample/sample_Secure/configure.launch and select "Run As" → "configure" to generate Makefile of ASP3.

6. Build projects.

7. Connect any USB-Serial device to PD_8 and PD_9 since the serial terminal of MicroPython is routed to those pins.

8. Connect ST-Link.

9. Right clink sample/sample_Secure/sample.launch and select "Debug As" → "sample" to debug.

### External links

- [TOPPERS SafeG-M](https://www.toppers.jp/safeg-m.html)
- [TOPPERS ASP3](https://www.toppers.jp/asp3-kernel.html)
- [Original MicroPython repository](https://github.com/micropython/micropython)

