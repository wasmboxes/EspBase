### Kako im radi NAPT

ip_portmap_add(proto, my_ip, mport, daddr, dport);
proto = TCP
my_ip - interface ip
daddr - destination address 
dport - destination port


- ip_napt_enable radi samo za IP AP-a 

```
ip4_addr_t m_addr; // 0XA000064
IP4_ADDR(&m_addr, 50, 0, 0, 1);
ip_napt_enable(m_addr.addr, 1); 

```

- ip_portmap_add 

```
esp32> portmap add TCP 12 50.0.0.3 12345
I (459695) ESP32 NAT router: New portmap table stored.
esp32> show
I (461745) cmd_router: ssid b
I (461745) cmd_router: passwd odjedandoosam
I (461745) cmd_router: ap_ssid mesh
I (461755) cmd_router: ap_passwd 12345678
STA SSID: b Password: odjedandoosam
AP SSID: mesh Password: 12345678
AP IP address: 50.0.0.1
Uplink AP connected
IP: 10.0.0.184
1 Stations connected
TCP 10.0.0.184:10 -> 10.0.0.100:40000
TCP 10.0.0.184:12 -> 50.0.0.3:12345  -- RADI
TCP 10.0.0.184:12346 -> 50.0.0.2:12345
esp32> 
```

```
if (portmap_tab[i].valid) {
            ip_portmap_add(portmap_tab[i].proto, my_ip, portmap_tab[i].mport, portmap_tab[i].daddr, portmap_tab[i].dport);
        }
```

```
   ip_portmap_add(
       portmap_tab[i].proto,
       my_ip, 
       portmap_tab[i].mport,
       portmap_tab[i].daddr,
       portmap_tab[i].dport
    );

    TCP 10.0.0.184:12 -> 50.0.0.3:12345  -- RADI
        my_ip     m_port  d_addr  d_port
```