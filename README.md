🚀 Overview
This repository provides a comprehensive solution for ECG signal acquisition, processing, and analysis, utilizing the ADS1292R analog front-end interfaced with a microcontroller via SPI. ECG signals are generated using the SKX-2000 ECG Signal Simulator, making the system ideal for testing and validation.

📌 Hardware Requirements
Signal Source: SKX-2000 ECG Signal Simulator
Analog Front-End: ADS1292R (refer to provided schematic)
Communication Interface: SPI protocol for microcontroller and ADS1292R

🛠️ Software Features
Real-Time Acquisition: Utilize ADS1292_Read_Data() function from the driver library for continuous data collection.
Data Transmission: ADS1292_Send_CMD() function transmits collected ECG signals to PC via UART at 115200 baud.
Data Storage: Raw ECG signals are saved as .dat files for further analysis.

📊 Signal Processing
MATLAB Analysis: .dat files are imported to MATLAB for visualization, filtering, and advanced processing.
Filtering: Cascade processing with IIR and FIR filters enhances signal clarity.
Heart Rate Calculation: Accurate estimation of heart rate from filtered ECG signals.

🔄 Workflow
flowchart TD
    A[Initialize and Verify ADS1292R via SPI] --> B[Acquire ECG signals from SKX-2000 simulator]
    B --> C[Decode and transmit signals via UART]
    C --> D[Save raw data in .dat format]
    D --> E[MATLAB Processing: Load .dat files]
    E --> F[Apply IIR and FIR Filters]
    F --> G[Estimate Heart Rate]

🛠️ Tools & Dependencies
FreeRTOS: Real-time operating system for efficient data management
MATLAB: Signal processing, visualization, and analysis

💻 Getting Started
Connect and verify hardware setup.
Clone the repository:
git clone https://github.com/koooooooop/qea.git
Flash microcontroller with provided code.
Capture data and analyze using MATLAB scripts provided in the repository.

📈 Results
Effective integration of real-time ECG signal acquisition with robust filtering and analysis achieves clear waveform visualization and accurate heart rate estimation, suitable for research, development, and diagnostic tool evaluation.

📚 Documentation
Refer to provided schematic diagrams and MATLAB scripts included in the repository for further insights and customizations.

🤝 Contribution
Feel free to open issues or contribute enhancements to this project.

📃 License
This project is licensed under the MIT License. See the LICENSE file for details.

