import pandas as pd
import matplotlib.pyplot as plt

# Top IP Addresses by Total Packets (from IP individual stats)
df_ip = pd.read_csv("output-ip-csv-files/ip-individual-stats.csv")
df_ip = df_ip.rename(columns={"ip1": "ipAddress"})  # in case ip1 needs to be renamed

# Calculate total packets
df_ip["totalPackets"] = df_ip["packetsIn"] + df_ip["packetsOut"]

# Top IPs
top_n = 10
top_ips = df_ip.groupby("ipAddress")["totalPackets"].sum().sort_values(ascending=False).head(top_n)

# Plot 1: Top IP addresses
plt.figure(figsize=(12, 6))
plt.bar(top_ips.index, top_ips.values, color="skyblue")
plt.title(f"Top {top_n} IP Addresses by Total Packets")
plt.xlabel("IP Address")
plt.ylabel("Total Packets (In + Out)")
plt.xticks(rotation=45)
plt.tight_layout()
plt.show()

# 2️⃣ Most Popular Destination Ports (from TCP connection stats)
df_tcp = pd.read_csv("output-tcp-csv-files/tcp-connection-stats.csv")

# Calculate total packets
df_tcp["totalPackets"] = df_tcp["packetsIn"] + df_tcp["packetsOut"]

# Group by destination port
port_traffic = df_tcp.groupby("destPort")["totalPackets"].sum().sort_values(ascending=False).head(10)

# Plot 2: Most popular destination ports
plt.figure(figsize=(10, 5))
plt.bar(port_traffic.index.astype(str), port_traffic.values, color="mediumseagreen")
plt.title("Top Destination Ports by Total Packets")
plt.xlabel("Destination Port")
plt.ylabel("Total Packets")
plt.xticks(rotation=45)
plt.tight_layout()
plt.show()

# 3️⃣ Most Popular Destination IP + Port Combinations
df_tcp["ip_port_combo"] = df_tcp["ip2"] + ":" + df_tcp["destPort"].astype(str)
combo_traffic = df_tcp.groupby("ip_port_combo")["totalPackets"].sum().sort_values(ascending=False).head(10)

# Plot 3: Top IP:Port combinations
plt.figure(figsize=(12, 6))
plt.bar(combo_traffic.index, combo_traffic.values, color="salmon")
plt.title("Top Destination IP:Port Combinations by Total Packets")
plt.xlabel("IP:Port")
plt.ylabel("Total Packets")
plt.xticks(rotation=45, ha='right')
plt.tight_layout()
plt.show()