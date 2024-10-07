## Communication Unit (ESP32)

### Overview

The communication units (ESP32) are responsible for subscribing to sensor data published by the coordinator. Once the data is received, the communication unit shares this information with external systems. The communication unit also manages protocol-specific operations, such as handling communication over WiFi, LoRa, or 4G, and reports its own health status.

### MQTT Topics

- **`/data/communication_unit/{CommUnitID}/protocol/{Protocol}/{InterfaceID}/{RequestOrResponse}/status`**:  
  - **Description**: Topic where the communication unit publishes protocol-specific statuses and responses (e.g., WiFi, LoRa, 4G). `{RequestOrResponse}` indicates the type of message, and status provides additional information on the message's success, failure, or error status.  
  - **Rationale**: Separates protocol operations for easier debugging and monitoring of network-level events.

- **`/data/communication_unit/{CommUnitID}/protocol/{Protocol}/{InterfaceID}/message`**:  
  - **Description**: The communication unit publishes the actual content of protocol messages here.  
  - **Rationale**: Isolates protocol message content for better data handling and potential future auditing or logging.

- **`/data/communication_unit/{CommUnitID}/health/{Metric}`**:  
  - **Description**: The communication unit publishes its own health metrics, such as CPU usage, memory status, or battery life.  
  - **Rationale**: Allows the coordinator to monitor the operational status of communication units, ensuring they function correctly.

- **`/data/coordinator/sensor/{SensorType}/{SensorID}/{MeasurementType}`**:  
  - **Description**: Communication units subscribe to this topic to receive sensor data published by the coordinator.  
  - **Rationale**: The communication unit processes this data and sends it to external systems as needed.

### Communication Unit Operation

1. **Data Reception**:  
   The communication unit subscribes to sensor data topics like `/data/coordinator/sensor/{SensorType}/{SensorID}/{MeasurementType}` to receive data from the coordinator's sensors. This data is then shared externally via the unit’s protocol-specific interfaces.

2. **Protocol Handling**:  
   The communication unit manages protocol-specific operations and publishes the results (e.g., request status, response content) to `/data/communication_unit/{CommUnitID}/protocol/{Protocol}/{InterfaceID}/{RequestOrResponse}/status` and `/data/communication_unit/{CommUnitID}/protocol/{Protocol}/{InterfaceID}/message`.

3. **Health Reporting**:  
   The communication unit continuously publishes its health metrics to `/data/communication_unit/{CommUnitID}/health/#` for the coordinator to monitor.

4. **Wake-Up, Restart, and Shutdown Control**:  
   The communication unit listens to wake-up commands on `/control/wakeup/communication_unit/{CommUnitID}`, restart commands on `/control/restart/communication_unit/{CommUnitID}/{RestartType}`, and shutdown commands on `/control/shutdown/communication_unit/{CommUnitID}/{ShutdownType}`. It responds to control signals as needed to conserve energy and maintain operational efficiency.

5. **Connection Maintenance**:  
   The communication unit uses persistent MQTT sessions and QoS 1 to ensure message delivery, automatically reconnecting to the broker if the connection is lost.

---

### Setting up Blackbox for File Encryption

To securely encrypt sensitive files in this project using **StackExchange Blackbox**, follow these steps:

#### 1. Install `BlackBox`
You can automatically install StackExchange Blackbox via the following commands:

```bash
git clone https://github.com/StackExchange/blackbox.git
cd blackbox
sudo make copy-install
```
This will copy the necessary files into `/usr/local/bin`.

#### 2. Obtain the Encoded GPG Keys
The **public** and **private** Base64-encoded GPG keys are stored in the repository's "Secrets." 
Ask the project maintainer to share the keys with you if you do not have access yet.

You will receive:
- A **Base64-encoded public key**
- A **Base64-encoded private key**

#### 3. Import the Public Key
Once you receive the **Base64-encoded public key**, use the following command to decode and import it:

```bash
echo "base64_encoded_public_key" | base64 --decode | gpg --import
```

- Replace `base64_encoded_public_key` with the actual Base64-encoded string of the public key.

#### 4. Import the Private Key
After importing the public key, you'll also need to import the **private key** for decryption purposes. To do that, use the following command:

```bash
echo "base64_encoded_private_key" | base64 --decode | gpg --import
```

- Replace `base64_encoded_private_key` with the actual Base64-encoded string of the private key.

#### 5. Verify the Import
You can verify if both keys were successfully imported with the following command:

```bash
gpg --list-secret-keys
```

This will list the GPG keys on your system, and you should see both the public and private key associated with your GPG email.

#### 6. Decrypt Files with `BlackBox`
With both the public and private keys imported, you can now decrypt the files in your project:

```bash
blackbox_decrypt_all_files
```

This command will decrypt all files that were encrypted with Blackbox, using your imported GPG keys.