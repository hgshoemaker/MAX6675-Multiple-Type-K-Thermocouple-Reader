# LabVIEW VISA Mode Integration Example

This directory contains examples for integrating the MAX6675 thermocouple reader with various programming languages and platforms using the VISA command-response mode.

## Files

### visa_test.py
A comprehensive Python test script that demonstrates all VISA commands and features:
- Device identification
- Temperature readings (all channels and individual)
- System information queries
- Error handling
- Performance testing

**Usage:**
1. Install pyserial: `pip install pyserial`
2. Update COM_PORT in the script
3. Run: `python visa_test.py`

### LabVIEW Integration Notes

#### VISA Resource Configuration
```
Resource Name: ASRL3::INSTR (replace 3 with your COM port)
Baud Rate: 9600
Data Bits: 8
Stop Bits: 1
Parity: None
Flow Control: None
Termination Character: \n (0x0A)
```

#### Basic LabVIEW VI Structure

1. **Initialization Block:**
   - VISA Open
   - VISA Configure Serial Port
   - VISA Write: "VSON\n"

2. **Main Loop:**
   - VISA Write: "MEAS:TEMP? ALL\n"
   - VISA Read
   - String Split (delimiter: ",")
   - String to Number Array
   - Update displays/charts

3. **Cleanup:**
   - VISA Write: "MODE:HUMAN\n" (optional)
   - VISA Close

#### Error Handling in LabVIEW
- Check for -999.0 values (sensor errors)
- Use VISA error cluster for communication errors
- Implement timeout handling
- Use "SYST:ERR?" for system status

### MATLAB Example (Basic)
```matlab
% Configure serial port
s = serialport("COM3", 9600);
configureTerminator(s, "LF");

% Enable VISA mode
writeline(s, "VSON");
pause(1);

% Read all temperatures
writeline(s, "MEAS:TEMP? ALL");
data = readline(s);
temperatures = str2double(split(data, ','));

% Display results
disp('Temperature readings:');
for i = 1:length(temperatures)
    if temperatures(i) == -999.0
        fprintf('Channel %d: ERROR\\n', i);
    else
        fprintf('Channel %d: %.2f°C\\n', i, temperatures(i));
    end
end

% Cleanup
clear s;
```

### C# Example (Basic)
```csharp
using System;
using System.IO.Ports;

class Program
{
    static void Main()
    {
        SerialPort port = new SerialPort("COM3", 9600);
        port.NewLine = "\n";
        port.Open();
        
        // Enable VISA mode
        port.WriteLine("VSON");
        System.Threading.Thread.Sleep(1000);
        
        // Read identification
        port.WriteLine("*IDN?");
        string id = port.ReadLine();
        Console.WriteLine($"Device: {id}");
        
        // Read temperatures
        port.WriteLine("MEAS:TEMP? ALL");
        string temps = port.ReadLine();
        Console.WriteLine($"Temperatures: {temps}");
        
        port.Close();
    }
}
```

## Command Reference

### Essential Commands for Integration
- `VSON` - Enable VISA mode
- `*IDN?` - Device identification  
- `MEAS:TEMP? ALL` - Read all temperatures
- `MEAS:TEMP? CH<n>` - Read specific channel
- `HELP?` - List all commands
- `MODE:HUMAN` - Return to human mode

### Data Format
- Temperature values: floating point with 2 decimal places
- Error values: -999.0 (indicates sensor fault/disconnect)
- CSV format: comma-separated values for multiple readings
- Termination: \n (newline character)

## Best Practices

1. **Always enable VISA mode first**: Send `VSON` before other commands
2. **Handle timeouts**: Set appropriate timeouts for VISA operations
3. **Check for errors**: Look for -999.0 values in temperature readings
4. **Use appropriate delays**: Allow 100ms between commands minimum
5. **Proper cleanup**: Return to human mode when done (optional)

## Troubleshooting

**No response to commands:**
- Ensure VISA mode is enabled with `VSON`
- Check termination character settings (\n)
- Verify baud rate (9600)

**Invalid temperature readings:**
- -999.0 indicates sensor error or disconnection
- Check thermocouple wiring
- Allow sensors to stabilize

**Communication errors:**
- Verify COM port number
- Close other applications using the serial port
- Check cable connections
