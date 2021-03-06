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
#include "../../../vault.h"
#include "submenus.h"
#include "../../../loading.h"
#include "../../../helpers.h"
#include "../../../uart.h"
#include "../contacts.h"
#include "nano_lib.h"


void menu_nano_update_contact_uart(menu8g2_t *prev){
    const char title[] = "Nano Contacts";
    char buf[100] ;

    menu8g2_t menu;
    menu8g2_copy(&menu, prev);
   
    esp_log_level_set("*", ESP_LOG_ERROR);

    loading_enable();
    loading_text_title("Contact Slot", title);
    flush_uart();
    printf("\nEnter Nano Contact Addres Slot (int >=0): ");
    get_serial_input_int(buf, sizeof(buf));
    loading_disable();
    uint8_t contact_index = atoi(buf);
    if( contact_index >= CONFIG_JOLT_NANO_CONTACTS_MAX ){
        snprintf(buf, sizeof(buf), "Contact index must be smaller than %d.",
                CONFIG_JOLT_NANO_CONTACTS_MAX);
        menu8g2_display_text_title(&menu, buf, title);
        esp_log_level_set("*", CONFIG_LOG_DEFAULT_LEVEL);
        return;
    }

    loading_enable();
    loading_text_title("Contact Name", title);
    char name[CONFIG_JOLT_NANO_CONTACTS_NAME_LEN];
    flush_uart();
    printf("\nContact Name: ");
    get_serial_input(name, sizeof(name));
    loading_disable();

    loading_enable();
    loading_text_title("Contact Address", title);
    uint256_t contact_public_key;
    flush_uart();
    printf("\nNano Address: ");
    get_serial_input(buf, sizeof(buf));
    loading_disable();

    if( E_SUCCESS != nl_address_to_public(contact_public_key, buf) ){
        menu8g2_display_text_title(&menu, "Invalid Address", title);
        esp_log_level_set("*", CONFIG_LOG_DEFAULT_LEVEL);
        return;
    }
   
    esp_log_level_set("*", CONFIG_LOG_DEFAULT_LEVEL);

    vault_rpc_t rpc = {
        .type = NANO_CONTACT_UPDATE,
        .nano_contact_update.name = name,
        .nano_contact_update.index = contact_index,
        .nano_contact_update.public = contact_public_key
    };

    if(vault_rpc(&rpc) != RPC_SUCCESS){
        return;
    }
}
