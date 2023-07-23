
// static void initialize_nvs(void)
// {
//     esp_err_t err = nvs_flash_init();
//     if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK( nvs_flash_erase() );
//         err = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(err);
// }

esp_err_t apply_portmap_tab() {
    for (int i = 0; i<IP_PORTMAP_MAX; i++) {
        if (portmap_tab[i].valid) {
            ip_portmap_add(portmap_tab[i].proto, my_ip, portmap_tab[i].mport, portmap_tab[i].daddr, portmap_tab[i].dport);
        }
    }
    return ESP_OK;
}

esp_err_t delete_portmap_tab() {
    for (int i = 0; i<IP_PORTMAP_MAX; i++) {
        if (portmap_tab[i].valid) {
            ip_portmap_remove(portmap_tab[i].proto, portmap_tab[i].mport);
        }
    }
    return ESP_OK;
}

void print_portmap_tab() {
    for (int i = 0; i<IP_PORTMAP_MAX; i++) {
        if (portmap_tab[i].valid) {
            printf ("%s", portmap_tab[i].proto == PROTO_TCP?"TCP ":"UDP ");
            ip4_addr_t addr;
            addr.addr = my_ip;
            printf (IPSTR":%d -> ", IP2STR(&addr), portmap_tab[i].mport);
            addr.addr = portmap_tab[i].daddr;
            printf (IPSTR":%d\n", IP2STR(&addr), portmap_tab[i].dport);
        }
    }
}


// esp_err_t get_portmap_tab() {
//     esp_err_t err;
//     nvs_handle_t nvs;
//     size_t len;

//     err = nvs_open(PARAM_NAMESPACE, NVS_READWRITE, &nvs);
//     if (err != ESP_OK) {
//         return err;
//     }
//     err = nvs_get_blob(nvs, "portmap_tab", NULL, &len);
//     if (err == ESP_OK) {
//         if (len != sizeof(portmap_tab)) {
//             err = ESP_ERR_NVS_INVALID_LENGTH;
//         } else {
//             err = nvs_get_blob(nvs, "portmap_tab", portmap_tab, &len);
//             if (err != ESP_OK) {
//                 memset(portmap_tab, 0, sizeof(portmap_tab));
//             }
//         }
//     }
//     nvs_close(nvs);

//     return err;
// }
----------------------------------------
main.cpp



struct portmap_table_entry {
  u32_t daddr;
  u32_t maddr;
  u16_t mport;
  u16_t dport;
  u8_t proto;
  u8_t valid;
};
struct portmap_table_entry portmap_tab[IP_PORTMAP_MAX];

esp_netif_t* wifiAP;
esp_netif_t* wifiSTA;
uint32_t my_ip;

#define PROTO_TCP 6
#define PROTO_UDP 17

static const char *TAG = "ESP32 NAT router";

esp_err_t apply_portmap_tab() {
    for (int i = 0; i<IP_PORTMAP_MAX; i++) {
        if (portmap_tab[i].valid) {
            ip_portmap_add(portmap_tab[i].proto, portmap_tab[i].maddr, portmap_tab[i].mport, portmap_tab[i].daddr, portmap_tab[i].dport);
        }
    }
    return ESP_OK;
}

esp_err_t delete_portmap_tab() {
    for (int i = 0; i<IP_PORTMAP_MAX; i++) {
        if (portmap_tab[i].valid) {
            ip_portmap_remove(portmap_tab[i].proto, portmap_tab[i].mport);
        }
    }
    return ESP_OK;
}

void print_portmap_tab() {
    for (int i = 0; i<IP_PORTMAP_MAX; i++) {
        if (portmap_tab[i].valid) {
            printf ("%s", portmap_tab[i].proto == PROTO_TCP?"TCP ":"UDP ");
            ip4_addr_t addr;
            addr.addr = portmap_tab[i].maddr;
            printf (IPSTR":%d -> ", IP2STR(&addr), portmap_tab[i].mport);
            addr.addr = portmap_tab[i].daddr;
            printf (IPSTR":%d\n", IP2STR(&addr), portmap_tab[i].dport);
        }
    }
}

esp_err_t add_portmap(u8_t proto, u32_t maddr, u16_t mport, u32_t daddr, u16_t dport) {
    esp_err_t err;
    nvs_handle_t nvs;

    for (int i = 0; i<IP_PORTMAP_MAX; i++) {
        if (!portmap_tab[i].valid) {
            portmap_tab[i].maddr = maddr;
            portmap_tab[i].proto = proto;
            portmap_tab[i].mport = mport;
            portmap_tab[i].daddr = daddr;
            portmap_tab[i].dport = dport;
            portmap_tab[i].valid = 1;
            ip_portmap_add(proto, maddr, mport, daddr, dport);
            return ESP_OK;
        }
    }
    return ESP_ERR_NO_MEM;
}

esp_err_t del_portmap(u8_t proto, u16_t mport) {
    esp_err_t err;
    nvs_handle_t nvs;

    for (int i = 0; i<IP_PORTMAP_MAX; i++) {
        if (portmap_tab[i].valid && portmap_tab[i].mport == mport && portmap_tab[i].proto == proto) {
            portmap_tab[i].valid = 0;
            ip_portmap_remove(proto, mport);
            return ESP_OK;
        }
    }
    return ESP_OK;
}


    // prejeb:
    // vTaskDelay(5000 / portTICK_PERIOD_MS);
    // printf("STOPPING AP\n");
    // c_nw->ApStop();

    // vTaskDelay(5000 / portTICK_PERIOD_MS);
    // printf("STOPPING STA\n");
    // c_nw->StStop();


    // wifi_ap_config_t wifi_config_ap_b = {};
    // strcpy((char*)wifi_config_ap_b.ssid, (const char*)EXAMPLE_ESP_WIFI_SSID);
    // strcpy((char*)wifi_config_ap_b.password, (const char*)EXAMPLE_ESP_WIFI_PASS);
    // wifi_config_ap_b.authmode = WIFI_AUTH_WPA_WPA2_PSK;
	// wifi_config_ap_b.max_connection = 8;

    // NetworkController::wifi_ap_conf_t g;
    // g.config = wifi_config_ap_b;
    // g.id = 2;
    // c_nw->ApAddConfiguration(g);
    //c_nw->ApStart(1);

    // ip4_addr_t m_addr; 
    // IP4_ADDR(&m_addr, 10, 0, 0, 184);
    // u_int16_t m_port = 12;
    
    // ip4_addr_t d_addr; 
    // IP4_ADDR(&d_addr, 50, 0, 0, 2);
    // u_int16_t d_port = 12345;

    // ip4_addr_t addr; 
    // IP4_ADDR(&addr, 50, 0, 0, 1);

    // ip4_addr_t aaddr; 
    // IP4_ADDR(&aaddr, 10, 0, 0, 100);
    
    // ip_napt_enable(addr.addr, 1); 

    // vTaskDelay(5000 / portTICK_PERIOD_MS);
    // printf("STARTING NAPT\n");
   
    // // ip_portmap_add(IP_PROTO_TCP, d_addr.addr, 4000, m_addr.addr, 4000); 
    // my_ip = m_addr.addr;

    // //                     184    12       2             12345
    // add_portmap(PROTO_TCP, my_ip, m_port, d_addr.addr, d_port);
    // //                      1         16     100      4000 
    // add_portmap(PROTO_TCP, d_addr.addr, 16, aaddr.addr, 4000);

    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // apply_portmap_tab();
    // print_portmap_tab();
    
    
    //ip_napt_enable(0XA000064, 1);
    //c_pf->ipInfo.ip.addr



    //ip_portmap_add(IP_PROTO_TCP, m_addr.addr, m_port, d_addr.addr, d_port);
    //ip_portmap_add(IP_PROTO_TCP, d_addr.addr, d_port, m_addr.addr, m_port); 
    //ip_portmap_add(IP_PROTO_TCP, m_addr.addr, d_port, d_addr.addr, m_port); 
    //ip_portmap_add(IP_PROTO_TCP, d_addr.addr, m_port, m_addr.addr, d_port); 