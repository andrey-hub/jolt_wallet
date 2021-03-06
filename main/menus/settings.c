/* Jolt Wallet - Open Source Cryptocurrency Hardware Wallet
 Copyright (C) 2018  Brian Pugh, James Coxon, Michael Smaili
 https://www.joltwallet.com/
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */


#include "esp_log.h"
#include "menu8g2.h"
#include "../vault.h"
#include "submenus.h"
#include "../globals.h"
#include "../wifi.h"
#include "../loading.h"
#include "../helpers.h"

#include "../uart.h"


static void wifi_details(menu8g2_t *prev){
    const char title[] = "WiFi Details";
    menu8g2_t menu;
    menu8g2_copy(&menu, prev);

    char new_ap_info[45];
    get_ap_info(new_ap_info, sizeof(new_ap_info));
    for(;;){
        if(menu8g2_display_text_title(&menu, new_ap_info, title)
                & (1ULL << EASY_INPUT_BACK)){
            return;
        }
    }
}

static void wifi_update(menu8g2_t *prev){
    const char title[] = "WiFi Update";
    menu8g2_t menu;
    menu8g2_copy(&menu, prev);
    menu.post_draw = NULL;
   
    esp_log_level_set("*", ESP_LOG_ERROR);
    loading_enable();
    loading_text("Enter WiFi Credentials via UART");
    
    char wifi_ssid[32];

    flush_uart();
    printf("\nWiFi SSID: ");
    get_serial_input(wifi_ssid, sizeof(wifi_ssid));
    
    char wifi_pass[64];

    flush_uart();
    printf("\nWiFi Password: ");
    get_serial_input(wifi_pass, sizeof(wifi_pass));
   
    nvs_handle wifi_nvs_handle;
    init_nvm_namespace(&wifi_nvs_handle, "user");
    nvs_set_str(wifi_nvs_handle, "wifi_ssid", wifi_ssid);
    nvs_set_str(wifi_nvs_handle, "wifi_pass", wifi_pass);
    esp_err_t err = nvs_commit(wifi_nvs_handle);
    
    nvs_close(wifi_nvs_handle);
    
    loading_disable();
    esp_log_level_set("*", CONFIG_LOG_DEFAULT_LEVEL);
    
    if (err != ESP_OK) {
        menu8g2_display_text_title(&menu, "Error Updating WiFi Settings", title);
    }
    else {
        menu8g2_display_text_title(&menu, "Updated WiFi Settings - Click to Reset", title);
        esp_restart();
    }
}

static void menu_factory_reset(menu8g2_t *prev){
    bool res;
    menu8g2_t menu;
    menu8g2_copy(&menu, prev);

    const char title[] = "Factory Reset?";

    const char *options[] = {"No", "Yes"};
    res = menu8g2_create_simple(&menu, title, options, 2);

    if(res==false || menu.index==0){ //Pressed Back
        return;
    }

    factory_reset();
}

void menu_settings(menu8g2_t *prev){
    menu8g2_t menu;
    menu8g2_copy(&menu, prev);
    const char title[] = "Settings";

    menu8g2_elements_t elements;
    menu8g2_elements_init(&elements, 6);
    menu8g2_set_element(&elements, "Screen Brightness", NULL);
    menu8g2_set_element(&elements, "WiFi Details", &wifi_details);
    menu8g2_set_element(&elements, "WiFi Update (uart)", &wifi_update);
    menu8g2_set_element(&elements, "Bluetooth", NULL);
    menu8g2_set_element(&elements, "Mnemonic Restore", &menu_mnemonic_restore);
    menu8g2_set_element(&elements, "Factory Reset", &menu_factory_reset);
    menu8g2_create_vertical_element_menu(&menu, title, &elements);
    menu8g2_elements_free(&elements);
}
