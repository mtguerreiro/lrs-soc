# Documentation of Changes in the Labview Code
## Introduction

This document outlines the significant changes made to the naming conventions within our project. The primary objective of these changes is to enhance readability, maintainability, and consistency across our codebase. 
The changes span across various components of our project, including VI, Functions, TCP Client, and Variables. Each section in this document corresponds to these components and provides a detailed overview of the old names, the new names, and a brief description of their functionalities.
By standardizing our naming conventions, we aim to create a more intuitive and user-friendly environment for both our team and any future contributors. This will not only simplify the process of understanding the code but also make it easier to debug, test, and enhance the project in the future.
Please refer to the table of contents for easy navigation through the document.

## VI Changes
### Main_menu4 -> Main_menu
**Old Name**: `Main_menu4`
**New Name**: `Main_menu`  
**Description:** The main menu function acts as the focal point for navigating and utilizing the diverse range of features and capabilities within our application. When users initiate the application, they encounter a user-friendly menu interface, offering a variety of options that cater to their specific requirements.
Key functionalities include:
- Initiating window plots for data visualization
- Establishing communication with microcontrollers
- Implementing robust error handling mechanisms

### New_plot1 -> Window_plot
**Old Name**: `New_plot1`
**New Name**: `Window_plot`  
**Description:** This window has the capability to visualize and analyze the graphs of the stored signals from the Main_menu.

### from_HW -> Call_signal_fromHW
**Old Name**: `from_HW`
**New Name**: `Call_signal_fromHW`  
**Description**: Through this function you can select the stored signals from the Main_menu.

### from_file -> Call_signal_fromfile
**Old Name**: `from_file`
**New Name**: `Call_signal_fromfile`  
**Desciption**: Through this function you can select the signals stored in a CSV file.

## Functions Changes

### Getting_TRACE DATA -> Fc_Get_TRACEDATA
**Old Name**: `Getting_TRACE DATA`
**New Name**: `Fc_Get_TRACEDATA`  
**Desciption**: This function calls the microcontroller to obtain the signal data and stores them at the time of the request, into the "Signals_value_Array" variable.
	
### String2ArrayDouble -> String2ArrayDbl
**Old Name**: `String2ArrayDouble`
**New Name**: `String2ArrayDbl`  

### Function_update_parameters_frommicro -> Fc_update_params_from_uc
**Old Name**: `Function_update_parameters_frommicro`
**New Name**: `Fc_update_params_from_uc`  

## TCP Client Changes

### TCP Client_33 -> Fc_TCP_Client_33
**Old Name**: `TCP Client_33`
**New Name**: `Fc_TCP_Client_33`  
**Desciption**: This function follows the tcp ip protocol to communicate with the microcontroller depending on the command (CMD) to be used. Version #33
#### List of commands
- 0-> OCP_IF_CMD_TRACE_READ = 0, //LV: SIZE - CMD - ID || RESPONSE: SIZE - DATA
- 1-> OCP_IF_CMD_TRACE_RESET, //SIZE - CMD - ID || RESPONSE: SIZE
- 2-> OCP_IF_CMD_TRACE_GET_SIZE, //SIZE - CMD - ID || RESPONSE: SIZE - TRACE_SIZE (4 bztes)
- 3-> OCP_IF_CMD_TRACE_SET_SIZE, //SIZE - CMD - ID - TRACE_SIZE ( 4 bytes) || RESPONSE: SIZE
- 4-> OCP_IF_CMD_TRACE_GET_NUMBER_SIGNALS, //SIZE - CMD - ID || RESPONSE: SIZE - NB_SIGNALS (4 bytes)
- 5-> OCP_IF_CMD_TRACE_GET_SIGNALS_NAMES, //SIZE - CMD - ID || RESPONSE: SIZE - string
- 6-> OCP_IF_CMD_TRACE_GET_NUMBER_TRACES, //SIZE - CMD  || RESPONSE: SIZE - NB_TRACES (4 bytes)
- 7-> OCP_IF_CMD_TRACE_GET_TRACES_NAMES, //SIZE - CMD || RESPONSE: SIZE - NB_TRACES (N bytes)
- 8-> OCP_IF_CMD_TRACE_GET_ADDRESS, //SIZE - CMD - ID || RESPONSE: SIZE - TRACE ADDRESS (4 bytes)
- 9-> OCP_IF_CMD_CS_STATUS, //SIZE - CMD - ID || RESPONSE: SIZE - CS STATUS (4 bytes)
- 10-> OCP_IF_CMD_CS_ENABLE, //SIZE - CMD - ID || RESPONSE: SIZE
- 11-> OCP_IF_CMD_CS_DISABLE, //SIZE - CMD - ID || RESPONSE: SIZE
- 12-> OCP_IF_CMD_CS_CONTROLLER_IF, //SIZE - CMD - ID - DATA (N) || RESPONSE: SIZE - DATA (M)
- 13-> OCP_IF_CMD_CS_HARDWARE_IF, //SIZE - CMD - ID - DATA (N) || RESPONSE: SIZE - DATA (M)
- 14-> OCP_IF_CMD_CS_GET_NUMBER_CONTROLLERS, //SIZE - CMD || RESPONSE: SIZE - NB CSs
- 15-> OCP_IF_CMD_CS_GET_CONTROLLERS_NAMES, //SIZE - CMD || RESPONSE: SIZE - string NAMES CSs

### TCP Client_34 -> Fc_TCP_Client_34
**Old Name**: `TCP Client_34`
**New Name**: `Fc_TCP_Client_34`  
**Description**: This function follows the tcp ip protocol to communicate with the microcontroller depending on the command (CMD) to be used. Version #34. Change data parameters
#### List of commands
- 0-> OCP_IF_CMD_TRACE_READ = 0, //LV: SIZE - CMD - ID || RESPONSE: SIZE - DATA
- 1-> OCP_IF_CMD_TRACE_RESET, //SIZE - CMD - ID || RESPONSE: SIZE
- 2-> OCP_IF_CMD_TRACE_GET_SIZE, //SIZE - CMD - ID || RESPONSE: SIZE - TRACE_SIZE (4 bztes)
- 3-> OCP_IF_CMD_TRACE_SET_SIZE, //SIZE - CMD - ID - TRACE_SIZE ( 4 bytes) || RESPONSE: SIZE
- 4-> OCP_IF_CMD_TRACE_GET_NUMBER_SIGNALS, //SIZE - CMD - ID || RESPONSE: SIZE - NB_SIGNALS (4 bytes)
- 5-> OCP_IF_CMD_TRACE_GET_SIGNALS_NAMES, //SIZE - CMD - ID || RESPONSE: SIZE - string
- 6-> OCP_IF_CMD_TRACE_GET_NUMBER_TRACES, //SIZE - CMD  || RESPONSE: SIZE - NB_TRACES (4 bytes)
- 7-> OCP_IF_CMD_TRACE_GET_TRACES_NAMES, //SIZE - CMD || RESPONSE: SIZE - NB_TRACES (N bytes)
- 8-> OCP_IF_CMD_TRACE_GET_ADDRESS, //SIZE - CMD - ID || RESPONSE: SIZE - TRACE ADDRESS (4 bytes)
- 9-> OCP_IF_CMD_CS_STATUS, //SIZE - CMD - ID || RESPONSE: SIZE - CS STATUS (4 bytes)
- 10-> OCP_IF_CMD_CS_ENABLE, //SIZE - CMD - ID || RESPONSE: SIZE
- 11-> OCP_IF_CMD_CS_DISABLE, //SIZE - CMD - ID || RESPONSE: SIZE
- 12-> OCP_IF_CMD_CS_CONTROLLER_IF, //SIZE - CMD - ID - DATA (N) || RESPONSE: SIZE - DATA (M)
- 13-> OCP_IF_CMD_CS_HARDWARE_IF, //SIZE - CMD - ID - DATA (N) || RESPONSE: SIZE - DATA (M)
- 14-> OCP_IF_CMD_CS_GET_NUMBER_CONTROLLERS, //SIZE - CMD || RESPONSE: SIZE - NB CSs
- 15-> OCP_IF_CMD_CS_GET_CONTROLLERS_NAMES, //SIZE - CMD || RESPONSE: SIZE - string NAMES CSs

### TCP Client_Get data signals -> Fc_TCP_Client_GetDatasignals
**Old Name**: `TCP Client_Get data signals`
**New Name**: `Fc_TCP_Client_GetDatasignals`  

## Variables Changes
### Variables1 -> Share_Variables
**Old Name**: `Variables1`
**New Name**: `Share_Variables`  
**Desciption**: In this section are all the variables declared and used to share data to the Window_plot from the Main_menu.

### List of variables	
1. Close_all_wplots
2. Global_refresh_time
3. Mode_signal_conf
4. number_signals
5. Signals_name
6. Signals_value_Array
7. Signals_value_string
8. Trace_name