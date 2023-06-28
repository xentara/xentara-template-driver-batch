# Batch Processing I/O Driver Template for Xentara

This repository contains skeleton code for a Xentara driver with the following features:

Connection based | Batch processing | Polymorphic Data Points
:--------------: | :--------------: | :---------------------:
NO               | YES              | NO

## Prerequisites

This driver template requires the Xentara development environment, as well as a Xentara licence. You can get a Xentara
licence in the [Xentara Online Shop](https://www.xentara.io/product/xentara-for-industrial-automation/).

The documentation for Xentara can be found at https://docs.xentara.io/xentara.

This driver template uses the Xentara Utility Library, as well as the Xentara Plugin Framework. The corresponding documentation can be found here:

https://docs.xentara.io/xentara-utils/  
https://docs.xentara.io/xentara-plugin/

## Build Environment

The repository include a [CMakeLists.txt](CMakeLists.txt) file for use with [CMake](https://cmake.org/), which allows you to build the source code
out of the box, as long as the Xentara development environment is installed. If you whish to use a different build system, you must generate the
necessary build configuration file yourself.

## Source Code Documentation

The source code in this repository is documented using [Doxygen](https://doxygen.nl/) comments. If you have Doxygen installed, you can
generate detailed documentation for all classes, functions etc., including a TODO list. To generate the documentation using CMake, just
build the target *docs* by executing the following command in your [build directory](https://cmake.org/cmake/help/latest/manual/cmake.1.html#generate-a-project-buildsystem):

~~~sh
cmake --build . --target docs
~~~

This will generate HTML documentation in the subdirectory *docs/html*.

## Xentara I/O Component Template

*(See [I/O Components](https://docs.xentara.io/xentara/xentara_io_components.html) in the [Xentara documentation](https://docs.xentara.io/xentara/))*

[src/TemplateIoComponent.hpp](src/TemplateIoComponent.hpp)  
[src/TemplateIoComponent.cpp](src/TemplateIoComponent.cpp)

The I/O component template provides template code for I/O components that can never be disconnected or reconnected to the computer.
Such components are either entirely stateless, or require a single initialization and deinitialization call. This generally includes
three types of components:

- I/O components that are physically located inside the computer (PCIe expansion boards, SPI or I2C components etc.),
- I/O components that are permanently attached to the computer, and cannot be removed or reattached without shutting down, or
- virtual I/O components, that do not represent physical devices at all (simulators, A/I models, computational units etc.).

## Xentara Batch Transaction Template

*(See [Batch Transactions](https://docs.xentara.io/xentara/xentara_batch_transactions.html) in the [Xentara documentation](https://docs.xentara.io/xentara/))*

[src/TemplateBatchTransaction.hpp](src/TemplateBatchTransaction.hpp)  
[src/TemplateBatchTransaction.cpp](src/TemplateBatchTransaction.cpp)

The batch transaction template provides template code for a batch communications request that can read and write multiple data points at the same time.

The template code has the following features:

- All data points attached to a batch transaction share common [Xentara attributes](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_attributes)
  for update time, [quality](https://docs.xentara.io/xentara/xentara_quality.html) and error code, which are maintained
  centrally in the batch transaction. These attributes are then inherited by the data points, so that they can be accessed as attributes of the data point as well.
- The batch transaction publishes a [Xentara task](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_tasks) called *read*,
  which acquires the current values of all data points from the I/O component using a read command.
- The batch transaction publishes a [Xentara task](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_tasks) called *write*,
  that checks which outputs have pending output values, and writes those outputs to the I/O component using a write command (if there are any).
- The batch transaction publishes [Xentara events](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_events) to signal if
  a write command was sent, or if a write error occurred. These events are *not* inherited by the data points, who have their own individual events instead.
  This is done so that the events of the individual outputs can be raised individually for only those outputs that were actually written.

## Xentara Skill Data Point Templates

*(See [Skill Data Points](https://docs.xentara.io/xentara/xentara_skill_data_points.html) in the [Xentara documentation](https://docs.xentara.io/xentara/))*

### Input Template

[src/TemplateInput.hpp](src/TemplateInput.hpp)  
[src/TemplateInput.cpp](src/TemplateInput.cpp)  

The input template provides template code for a read-only skill data point whose value is read using a batch transaction.

The template code has the following features:

- The input inherits [Xentara attributes](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_attributes)
  for update time, [quality](https://docs.xentara.io/xentara/xentara_quality.html) and error code from the
  batch transaction, and shares them with all other data points belonging to the same batch transaction.

### Output Template

[src/TemplateOutput.hpp](src/TemplateOutput.hpp)  
[src/TemplateOutput.cpp](src/TemplateOutput.cpp)

The output template provides template code for a read/write skill data point whose value must be read and written using individual commands for each output.

The template code has the following features:

- The input and output values are handled entirely separately. A written output value is not reflected in the input value until
  it has been read back from the I/O component by the batch transaction. This is necessary because the I/O component might reject or
  modify the written value.
- The value of the output is not sent to the I/O component directly when it is written, but placed in a queue to be written by the batch transaction.
- The output inherits [Xentara attributes](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_attributes)
  for update time, [quality](https://docs.xentara.io/xentara/xentara_quality.html) and error code from the
  batch transaction, and shares them with all other data points belonging to the same batch transaction.
- The output publishes [Xentara events](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_events) to signal if
  a new value was written, or if a write error occurred.
