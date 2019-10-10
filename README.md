# stm32_usb_test_wcid

A simple example of a WCID capable usb-firmware for the [STM32F4-Discovery](https://www.st.com/en/evaluation-tools/stm32f4discovery.html) board.

You'll find a suitable test/benchmark application for the host in this [repository](https://github.com/pavolk/libusb_stm32_testapp.git).


# Prepare the workspace

This firmware is a [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) project that can be imported to a existing workspace.

## Get the sources

```
git clone https://github.com/pavolk/stm32_usb_test_wcid.git
```

## Import the project to STM32CubeIDE/Eclipse

Start STM32CubeIDE. The import the project like this...

*"File" -> "Import" -> "General" -> "Existing Project into Workspace"*.

Then make sure that *"Select root directory"* points to the `stm32_usb_test_wcid` subdirectory, created by git. Afterward, select `usb_test_wcid` project and press *"Finish"*.

# Build and run on target

Follow the usual Eclipse/STM32CubeIDE workflow for building the firmware and running it on the target.