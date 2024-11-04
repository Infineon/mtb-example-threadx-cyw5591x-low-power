# CYW5591x Low power

This is low power application for CYW5591x devices. The application provides a simple UART interface to manipulate the device state and hence facilitating the system power measurement in different scenarios. This code example should be used along with [AN239828](https://www.infineon.com/an239828): AIROC(TM) CYW55913 Wi-Fi & Bluetooth&reg; connected MCU low-power system design for better understanding.

[View this README on GitHub.](https://github.com/Infineon/mtb-example-threadx-cyw5591x-low-power)

[Provide feedback on this code example.](https://cypress.co1.qualtrics.com/jfe/form/SV_1NTns53sK2yiljn?Q_EED=eyJVbmlxdWUgRG9jIElkIjoiQ0UyNDAwNjAiLCJTcGVjIE51bWJlciI6IjAwMi00MDA2MCIsIkRvYyBUaXRsZSI6IkNZVzU1OTF4IExvdyBwb3dlciIsInJpZCI6ImFtYWwgbWlzaHJhIiwiRG9jIHZlcnNpb24iOiIxLjEuMCIsIkRvYyBMYW5ndWFnZSI6IkVuZ2xpc2giLCJEb2MgRGl2aXNpb24iOiJNQ0QiLCJEb2MgQlUiOiJJQ1ciLCJEb2MgRmFtaWx5IjoiV0lGSSJ9)

## Requirements

- [ModusToolbox&trade; software](https://www.infineon.com/modustoolbox) v3.2 or later (tested with v3.2)
- Board support package (BSP) minimum required version for : v1.0.0.
- Programming language: C
- Associated parts: CYW955913EVK-01


## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm&reg; Embedded Compiler v11.3.1 (`GCC_ARM`) – Default value of `TOOLCHAIN`

## Supported kits (make variable 'TARGET')
- CYW955913EVK-01 Wi-Fi Bluetooth&reg; prototyping kit (`CYW955913EVK-01`) – Default value of `TARGET`



## Hardware setup

This example requires rework to enable power mesurement on the Kit. Please refer to section 7.1: Hardware setup in [AN239828](https://www.infineon.com/an239828): AIROC(TM) CYW55913 Wi-Fi & Bluetooth&reg; connected MCU low-power system design for complete rework details and setup.

## Software setup
Install a terminal emulator if you don't have one. Instructions in this document use [Tera Term](https://teratermproject.github.io/index-en.html).



## Using the code example

### Create the project

The ModusToolbox&trade; tools package provides the Project Creator as both a GUI tool and a command line tool.

<details><summary><b>Use Project Creator GUI</b></summary>

1. Open the Project Creator GUI tool.

   There are several ways to do this, including launching it from the dashboard or from inside the Eclipse IDE. For more details, see the [Project Creator user guide](https://www.infineon.com/ModusToolboxProjectCreator) (locally available at *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/docs/project-creator.pdf*).

2. On the **Choose Board Support Package (BSP)** page, select a kit supported by this code example. See [Supported kits](#supported-kits-make-variable-target).

   > **Note:** To use this code example for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work.

3. On the **Select Application** page:

   a. Select the **Applications(s) Root Path** and the **Target IDE**.

   > **Note:** Depending on how you open the Project Creator tool, these fields may be pre-selected for you.

   b.    Select this code example from the list by enabling its check box.

   > **Note:** You can narrow the list of displayed examples by typing in the filter box.

   c. (Optional) Change the suggested **New Application Name** and **New BSP Name**.

   d. Click **Create** to complete the application creation process.

</details>

<details><summary><b>Use Project Creator CLI</b></summary>

The 'project-creator-cli' tool can be used to create applications from a CLI terminal or from within batch files or shell scripts. This tool is available in the *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/* directory.

Use a CLI terminal to invoke the 'project-creator-cli' tool. On Windows, use the command-line 'modus-shell' program provided in the ModusToolbox&trade; installation instead of a standard Windows command-line application. This shell provides access to all ModusToolbox&trade; tools. You can access it by typing "modus-shell" in the search box in the Windows menu. In Linux and macOS, you can use any terminal application.

The following example clones the "[CYW5591x Low Power](https://github.com/Infineon/mtb-example-threadx-cyw5591x-low-power)" application with the desired name "MyLowPowerApp" configured for the *CYW955913EVK-01* BSP into the specified working directory, *C:/mtb_projects*:

   ```
   project-creator-cli --board-id CYW955913EVK-01 --app-id mtb-example-threadx-cyw5591x-low-power --user-app-name MyLowPowerApp --target-dir "C:/mtb_projects"
   ```

The 'project-creator-cli' tool has the following arguments:

Argument | Description | Required/optional
---------|-------------|-----------
`--board-id` | Defined in the <id> field of the [BSP](https://github.com/Infineon?q=bsp-manifest&type=&language=&sort=) manifest | Required
`--app-id`   | Defined in the <id> field of the [CE](https://github.com/Infineon?q=ce-manifest&type=&language=&sort=) manifest | Required
`--target-dir`| Specify the directory in which the application is to be created if you prefer not to use the default current working directory | Optional
`--user-app-name`| Specify the name of the application if you prefer to have a name other than the example's default name | Optional

> **Note:** The project-creator-cli tool uses the `git clone` and `make getlibs` commands to fetch the repository and import the required libraries. For details, see the "Project creator tools" section of the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at {ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf).

</details>



### Open the project

After the project has been created, you can open it in your preferred development environment.


<details><summary><b>Eclipse IDE</b></summary>

If you opened the Project Creator tool from the included Eclipse IDE, the project will open in Eclipse automatically.

For more details, see the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_ide_user_guide.pdf*).

</details>


<details><summary><b>Visual Studio (VS) Code</b></summary>

Launch VS Code manually, and then open the generated *{project-name}.code-workspace* file located in the project directory.

For more details, see the [Visual Studio Code for ModusToolbox&trade; user guide](https://www.infineon.com/MTBVSCodeUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_vscode_user_guide.pdf*).

</details>


<details><summary><b>Keil µVision</b></summary>

Double-click the generated *{project-name}.cprj* file to launch the Keil µVision IDE.

For more details, see the [Keil µVision for ModusToolbox&trade; user guide](https://www.infineon.com/MTBuVisionUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_uvision_user_guide.pdf*).

</details>


<details><summary><b>IAR Embedded Workbench</b></summary>

Open IAR Embedded Workbench manually, and create a new project. Then select the generated *{project-name}.ipcf* file located in the project directory.

For more details, see the [IAR Embedded Workbench for ModusToolbox&trade; user guide](https://www.infineon.com/MTBIARUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_iar_user_guide.pdf*).

</details>


<details><summary><b>Command line</b></summary>

If you prefer to use the CLI, open the appropriate terminal, and navigate to the project directory. On Windows, use the command-line 'modus-shell' program; on Linux and macOS, you can use any terminal application. From there, you can run various `make` commands.

For more details, see the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>


## Operation

1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector.

2. Program using one of the following and reset the board:

   <details><summary><b>Using Eclipse IDE for ModusToolbox&trade; software</b></summary>

      1. Select the application project in the Project Explorer.

      2. In the **Quick Panel**, scroll down, and click **\<Application Name> Program**.
   </details>

   <details><summary><b>Using CLI</b></summary>

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target. The default toolchain and target are specified in the application's Makefile but you can override those values manually:
      ```
      make program TARGET=APP_<BSP> TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make program TARGET=APP_CYW955913EVK-01 TOOLCHAIN=GCC_ARM
      ```
   </details>
3. Following instructions appear on the terminal on application start:
    * Press **'1'** to initialize WLAN
        - This option allows you to initialize the WLAN CR4 core, the wifi host driver (WHD) and wifi connection manager.
    * Press **'2'** to connect to an AP, Enter AP SSID and password when prompted.
        - This option allows you to connect to an access point. Once selected it will prompt you for the AP's SSID and password.
        
        ``````
        Note: Make sure the terminal being used has New Line Transmit set to CR and not CR+LF as it is used identify the end of user input.
        ``````

    * Press **'3'** to start iperf session
        - This option start's an iperf server. This should only be used to simulate data traffic as it is not optimized to provide best throughput. Start a client on a different machine and connect to the server. `Note: This iperf utility is based on iperf2, Please use the same on the client`
    * Press **'4'** to initialize Bluetooth&reg;.
         - This will initialize the Bluetooth&reg; stack. Note: Bluetooth Initialization should be done before Wi-Fi initialization. If you plan to use both, please follow the recommended order Bluetooth&reg; Init -> Wi-FI Init.
    * Press **'5'** to start Bluetooth&reg; LE advertisements
         - This will start Bluetooth&reg; advertisements with Device name "Low Power".
    * Press **'6'** to lock sleep
         - This will acquire a sleep lock and will prevent the device from sleeping.
    * Press **'7'** to allow sleep
         - This will release the sleep lock and will allow the device to sleep. The application by default starts with a sleep lock to allow manipulation of system state before measuring power.
    * Press **'8'** to disconnect from the AP.
        - This option is used to disconnect an existing connection to an AP.
    * Press **'9'** any time in application to start scan.
        - This option is used to scan for available AP's in the range.
    * Press **'h'** any time in application to print the menu
        - This option is used to request the Start menu options to view the options availabe at any point in the program.

    Use these available commands to interact with the application and achieve the required state for which you want to measure the power. Once done proceed with power measurements.


## Design and implementation

The application provides a simple UART interface for facilitating low power measurements for CYW55913.It starts by taking a sleep lock which is required to interact with the application and issue commands. If the device goes to sleep, the UART communication will be suspended hence the need for sleep lock. Once the desired state is achieved, we can release the lock for the power measurement by issuing command 7. furthermore we can wake the device up using USER BTN (SW4) to make further changes in the state if needed. During GPIO wake, we take a sleep lock to facilitate the same by keeping UART communication active until we have our desired state.

Lets look at the application structure in Table 1 and some of the important functions in table 2.

**Table 1. Source folders Description**

|Folder| Description  |
| ------------------------------------|---------------|
LPSrc | Contains source files related to System low power interface.
WLANSrc| Contains source files with functions related to WLAN handling.
BluetoothSrc| Contains source files with functions related to Bluetooth&reg; LE handling.

**Table 2. Overview of Important functions**
|Functions| Description  |
| ------------------------------------|---------------|
main | Initializes the BSP <br /> Enables Global interrupts <br /> Creates UART task for handling user commands <br /> Resgisters System Power manager callback for low power |
uart_task |Processes the received commands via terminal and implements necessary handling
lp_wlan_init| Initializes wifi connection manager.
connect_to_wifi_ap | Connects to Wi-Fi AP using the user-provided credentials
iperf_util_thread_app | Starts Iperf server for simulating WLAN traffic
network_idle_task | Monitors and suspends network stack when there is no network activity.
tcp_socket_connection_start | Connects to the configured offload TCP server in device configfurator.
pm_callback | Receives the low power transition callbacks from SysPM Module
le_app_management_callback | Receivea management events from the Bluetooth&reg; LE stack and process as per the application.
le_app_gatt_event_callback | This function handles GATT events from the Bluetooth&reg; LE stack


The menu explained in the [operation](#operation) section provides easy way to measure low power in different scenarios. Let's take a look at one such scenario, suppose we want to measure current when we are associated to an AP with no network activity. In this case we need to first initialise the Wifi interface (Command 1) , connect to an AP (command 2 ) and then release the sleep lock to allow device to go to sleep ( command 7). The Low power assistant in the network_idle_task takes care of suspending the network stack in case of inactivity. Make sure to set the appropriate DTIM (Delivery traffic indication message) value in the Access point for which you want the power measurement. For power numbers for various scnearios please refer [AN239828](https://www.infineon.com/an239828): AIROC(TM) CYW55913 Wi-Fi & Bluetooth&reg; connected MCU low-power system design.

```
Note: For cases where both Bluetooth and the Wi-Fi needs to be used make sure to initialise the Bluetooth first.
```
### Offloads:
CYW55913 supports ARP, Packet filter and TCP keep alive offloads. The application by default enables ARP and packet filter offload (Ping discard filter) via a custom design.modus file present inside templates folder. To understand more about these offloads refer the companion Application Note and Low power assistant Documentation. In the case where ARP offload is enabled CM33 (Host) will not wake up for responding to ARP requests and the same can be observed on the power analyser. Sniffer can be used to monitor that ARP requests are getting honoured. Refer to [Low Power Assistant documentation](https://infineon.github.io/lpa/api_reference_manual/html/index.html) for detailed information about offloads including how to configure and test them.

```
Note: LPA on network suspension prints out this message : "Network Stack Suspended, MCU will enter Active power mode" . Please ignore the MCU state mentioned here.
```
```
Note: TCP Keep alive although supported is not yet enabled in the CE, will be added in subsequent release.
```
### System Power Manager (SysPM):
SysPM Hardware abstraction Module allows the users to manage the entry and exit into low power modes. The application modules can register callback to SysPM to get information of different transition states and do any processing if required. The pm_callback function mentioned in Table 2 is one such example of the same. The SysPM module also provides sleep lock and unlock API's which when called will prevent the device from going to sleep.



## Related resources


Resources  | Links
-----------|----------------------------------
Device documentation | Contact infineon sales for device documentation
Development kits | Contact infineon sales for kit samples
Application Notes | [AN239828](https://www.infineon.com/an239828): AIROC(TM) CYW55913 Wi-Fi & Bluetooth&reg; connected MCU low-power system design
Libraries on GitHub  | [Low Power Assistant](https://github.com/Infineon/lpa) - Low power helper library <br /> [mtb-pdl-cat5](https://github.com/Infineon/mtb-pdl-cat5) – CYW55913 peripheral driver library (PDL)  <br /> [mtb-hal-cat5](https://github.com/Infineon/mtb-hal-cat5) – Hardware abstraction layer (HAL) library <br /> [retarget-io](https://github.com/Infineon/retarget-io) – Utility library to retarget STDIO messages to a UART port
Tools  | [Eclipse IDE for ModusToolbox&trade; software](https://www.infineon.com/modustoolbox) – ModusToolbox&trade; software is a collection of easy-to-use software and tools enabling rapid development with Infineon MCUs, covering applications from embedded sense and control to wireless and cloud-connected systems using AIROC&trade; Wi-Fi and Bluetooth&reg; connectivity devices.

<br>


## Other resources

Infineon provides a wealth of data at [www.infineon.com](https://www.infineon.com) to help you select the right device, and quickly and effectively integrate it into your design.


## Document history

Document title: *CE240060* – *CYW5591x Low Power*

 Version | Description of change
 ------- | ---------------------
 1.0.0   | New code example
 1.1.0   | bug fix


<br>

---------------------------------------------------------

© Cypress Semiconductor Corporation, 2020-2024. This document is the property of Cypress Semiconductor Corporation, an Infineon Technologies company, and its affiliates ("Cypress").  This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress’s patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
<br />
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product. CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach").  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications. To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document. Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device. You shall indemnify and hold Cypress, including its affiliates, and its directors, officers, employees, agents, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device. Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress’s published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
<br />
Cypress, the Cypress logo, and combinations thereof, WICED, ModusToolbox, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress or a subsidiary of Cypress in the United States or in other countries. For a more complete list of Cypress trademarks, visit www.infineon.com. Other names and brands may be claimed as property of their respective owners.
